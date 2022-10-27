#ifndef CGRAOBJECT
#include "cgraobject.hpp"
#define CGRAOBJECT
#endif
#include <cmath>
#ifndef VA
#include "VertexArray.cpp"
#define VA
#endif

#ifndef VB
#include "VertexBuffer.cpp"
#define VB
#endif

#ifndef VBL
#include "VertexBufferLayout.hpp"
#define VBL
#endif

#ifndef IB
#include "IndexBuffer.cpp"
#define IB
#endif

// Classe para print mat4

void PrintMat4(glm::mat4 Matrix) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      std::cout << Matrix[j][i] << "\t";
    }
    std::cout << "\n";
  }
}

// Classe base
CGRAobject::CGRAobject() { modeltr = glm::mat4(1.0); }

CGRAobject::~CGRAobject() {}

void CGRAobject::setModelTransformation(glm::mat4 &modeltransf) {
  modeltr = modeltransf;
}

void CGRAobject::setShader(DEECShader *shaderprog) { shader = shaderprog; }

void CGRAobject::SetUniform4f(glm::vec4 data, std::string uniformName) {
  int data_location =
      glGetUniformLocation(shader->shaderprogram, uniformName.c_str());
  glUniform4f(data_location, data[0], data[1], data[2], data[3]);
}
void CGRAobject::SetColor(glm::vec4 in_color) { color = in_color; }

void CGRAobject::drawIt(glm::mat4 V, glm::mat4 P) {}

/*--------------------------+
|         Compostos         |
+---------------------------*/

CGRACompound::CGRACompound(CGRAobject &Base) {
  Object = &Base;
  TransformFromMother =
      glm::mat4(1.0f); // Referencial da base é o referencial inicial do objecto
}

CGRACompound::~CGRACompound() {}

void CGRACompound::PushChild(CGRACompound *Child, glm::mat4 connection) {
  Child->SetTransformFromMother(connection);
  Children.push_back(Child);
}

void CGRACompound::DrawTree(glm::mat4 V, glm::mat4 P) {
  glm::mat4 newModelTr(1.0f), oldModelTr(1.0f);
  newModelTr = TransformFromMother * Object->modeltr;
  oldModelTr = Object->modeltr;
  Object->setModelTransformation(newModelTr);
  Object->drawIt(V, P);
  Object->setModelTransformation(oldModelTr);
  for (const auto &elemt : Children) {
    elemt->DrawTree(V, P);
  }
}

void CGRACompound::SetTransformFromMother(glm::mat4 &modeltransf) {
  TransformFromMother = modeltransf;
  modeltr = TransformFromMother * modeltr;
}

void CGRACompound::PropagateModelTransformation(glm::mat4 &modeltransf) {
  Object->modeltr = modeltransf * Object->modeltr;
  for (const auto &elemt : Children) {
    elemt->TransformFromMother = modeltransf * elemt->TransformFromMother;
  }
}

/*--------------------------+
|         Extrusões         |
+---------------------------*/

CGRAExtrusion::CGRAExtrusion(std::vector<glm::vec3> pontos) {
  std::vector<float> vtx_info;
  unsigned int numPontos = pontos.size();
//  std::cout << "num de pontos: " << numPontos << "\n";
  float x_new = 0.0f, y_new = 0.0f, z_new = 0.0f, x, y, z;
  // Push dos pontos iniciais
  for (const auto &elemt : pontos) {
    vtx_info.emplace_back(elemt[0]);
    vtx_info.emplace_back(elemt[1]);
    vtx_info.emplace_back(elemt[2]);
  }
  for (const auto &elemt : pontos) {
    x = elemt[0];
    y = elemt[1];
    z = elemt[2];
    x_new = x;
    y_new = y;
    z_new = z + 1.0f;
    // Push novos vértices
    vtx_info.emplace_back(x_new);
    vtx_info.emplace_back(y_new);
    vtx_info.emplace_back(z_new);
  }

  std::vector<unsigned int> indices;

  for (unsigned int i = 0; i < numPontos; i++) {
    indices.emplace_back(i);             // Entre [0, numPontos]
    indices.emplace_back(i + numPontos); // Entre [numPontos, 2 * numPontos - 1]
    indices.emplace_back((i + 1) % numPontos +
                         numPontos); // Entre [numPontos, 2 * numPontos - 1]

/*    std::cout << "i = " << i << ": " << i << ", " << i + numPontos << ", "
              << i + 1 + numPontos << "\t";*/

    indices.emplace_back(i);                   // Entre [0, numPontos]
    indices.emplace_back((i + 1) % numPontos); // Entre [0, numPontos]
    indices.emplace_back(((i + 1) % numPontos) +
                         numPontos); // Entre [numPontos, 2* numPontos - 1]
/*    std::cout << "i = " << i << ": " << i << ", " << ((i + 1) % numPontos)
              << ", " << ((i + 1) % numPontos) + numPontos << "\n";*/
  }

  m_VB.Push(GL_ARRAY_BUFFER, vtx_info.size() * sizeof(float), vtx_info.data(),
            GL_STATIC_DRAW);

  m_Layout.Push<float>(3, "Vertex Coordinates");
  m_IB.Push(indices.data(), indices.size());
  m_VA.AddBuffer(m_VB, m_Layout);
}

CGRAExtrusion::~CGRAExtrusion() {}

void CGRAExtrusion::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();
  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  //  std::cout << "mvp_location: " << mvp_location << "\n";
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  SetUniform4f(color, "u_Colors");
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*--------------------------+
|         Revoluções        |
+---------------------------*/

CGRARevolution::CGRARevolution(std::vector<glm::vec3> pontos) {
  const float PI = 3.14f;
  const float thetaStep = 2 * PI / 10;
  float theta = 0.0f;
  std::vector<float> vtx_info;
  float x_new = 0.0f, y_new = 0.0f, x, y, z;
  // Aplicar 10 rotações ao redor do eixo z aos pontos recebidos.
  for (int i = 0; i < 10; i++) {
    for (const auto &elemt : pontos) {
      x = elemt[0];
      y = elemt[1];
      z = elemt[2];
      x_new = static_cast<float>(std::cos(theta) * x - std::sin(theta) * y);
      y_new = static_cast<float>(std::sin(theta) * x + std::cos(theta) * y);
      vtx_info.emplace_back(x_new);
      vtx_info.emplace_back(y_new);
      vtx_info.emplace_back(z);
    }
    theta += thetaStep;
  }

  // Indices:
  // No fim das rotações, teremos 10 vezes mais vértices.
  // Seja P_i o i-ésimo ponto inicial e P^n_i o i-ésimo ponto na n-ésima
  // iteração da rotação. Pretendemos unir triângulos compostos pelos vértices
  // (P^n_i, P^n_(i+1), P^(n+1)_(i+1)) e pelos vértices (P^n_i, P^(n+1)_i,
  // P^(n+1)_(i+1))

  std::vector<int> indices;
  int p1, p2;
  int pStep = pontos.size();
  // Esta implementação funciona apenas para 2 pontos!! Terei de fazer outro
  // nested loop para iterar (ver caso esfera) p1 entre 1 até Nº de pontos -1.
  // Se tiver tempo quando o projecto estiver estável, trabalho nisso. Por
  // enquanto, apenas quero   qualquer coisa a ser desenhada no ecra

  // Iterar ao longo das rotações
  for (unsigned int i = 0; i < 9; i++) {
    // Iterar ao longo dos pontos
    for (unsigned int j = 0; j < pontos.size(); j++) {
      p1 = i * pStep;
      p2 = p1 + 1;
      indices.emplace_back(p1);
      indices.emplace_back(p2);
      indices.emplace_back(p2 + pStep);

      indices.emplace_back(p1);
      indices.emplace_back(p2 + pStep);
      indices.emplace_back(p1 + pStep);
    }
  }

  // Caso degenerado, vértices vão OOB. Agora que me lembro, podia ter feito
  // indice % limite.

  indices.emplace_back(pStep * 9);
  indices.emplace_back(pStep * 9 + 1);
  indices.emplace_back(1);
  indices.emplace_back(pStep * 9);
  indices.emplace_back(1);
  indices.emplace_back(0);

  m_VB.Push(GL_ARRAY_BUFFER, vtx_info.size() * sizeof(float), vtx_info.data(),
            GL_STATIC_DRAW);
  m_Layout.Push<float>(3, "Vertex Coordinates");
  m_IB.Push(indices.data(), indices.size());
  m_VA.AddBuffer(m_VB, m_Layout);
}

CGRARevolution::~CGRARevolution() {}

void CGRARevolution::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();

  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  SetUniform4f(color, "u_Colors");
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*--------------------------+
|         Quadrado          |
+---------------------------*/

CGRASquare::CGRASquare() {
  float face_positions[] = {
      // Vertex Coordinates
      -0.5f, -0.5f, 0.0f, // 0
      0.5f,  -0.5f, 0.0f, // 1
      0.5f,  0.5f,  0.0f, // 2
      -0.5f, 0.5f,  0.0f  // 3
  };

  unsigned int indices[] = {0, 1, 2, 2, 3, 0};
  m_VB.Push(GL_ARRAY_BUFFER, sizeof(face_positions), face_positions,
            GL_STATIC_DRAW);
  m_IB.Push(indices, 6);
  m_Layout.Push<float>(3, "Vertex Coordinates");
  m_VA.AddBuffer(m_VB, m_Layout);
  setShader(shader);
}

CGRASquare::~CGRASquare() {}

void CGRASquare::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();
  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  SetUniform4f(color, "u_Colors");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*--------------------------+
|         Cubo              |
+---------------------------*/

CGRACube::CGRACube() {
  float face_positions[] = {
      // Vertex Coordinates
      -0.5f, -0.5f, -0.5f, // 0
      0.5f, -0.5f, -0.5f,  // 1
      0.5f, 0.5f, -0.5f,   // 2
      -0.5f, 0.5f, -0.5f,  // 3
      //---------------------//
      -0.5f, -0.5f, 0.5f, // 4
      0.5f, -0.5f, 0.5f,  // 5
      0.5f, 0.5f, 0.5f,   // 6
      -0.5f, 0.5f, 0.5f,  // 7
      //---------------------//
      0.5f, -0.5f, 0.5f,  // 8
      0.5f, -0.5f, -0.5f, // 9
      0.5f, 0.5f, -0.5f,  // 10
      0.5f, 0.5f, 0.5f,   // 11
      //---------------------//
      -0.5f, -0.5f, 0.5f,  // 12
      -0.5f, -0.5f, -0.5f, // 13
      -0.5f, 0.5f, -0.5f,  // 14
      -0.5f, 0.5f, 0.5f,   // 15
      //---------------------//
      -0.5f, 0.5f, 0.5f,  // 16
      0.5f, 0.5f, 0.5f,   // 17
      0.5f, 0.5f, -0.5f,  // 18
      -0.5f, 0.5f, -0.5f, // 19
      //---------------------//
      -0.5f, -0.5f, 0.5f,  // 20
      0.5f, -0.5f, 0.5f,   // 21
      0.5f, -0.5f, -0.5f,  // 22
      -0.5f, -0.5f, -0.5f, // 23
  };

  unsigned int indices[] = {0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,
                            8,  9,  10, 10, 11, 8,  12, 13, 14, 14, 15, 12,
                            16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

  m_VB.Push(GL_ARRAY_BUFFER, sizeof(face_positions), face_positions,
            GL_STATIC_DRAW);
  m_IB.Push(indices, 6 * 6);
  m_Layout.Push<float>(3, "Vertex Coordinates");
  m_VA.AddBuffer(m_VB, m_Layout);
}

CGRACube::~CGRACube() {}

void CGRACube::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();
  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  SetUniform4f(color, "u_Colors");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*--------------------------+
|         Esfera            |
+---------------------------*/

CGRASphere::CGRASphere() {

  /*    Eqs paramétricas
x = (r * cos(phi) * cos(theta))
y = (r * cos(phi) * sin(theta))
z = r * sin(phi)
xy = r *  cos(phi)            */
  const float radius = 1.0f;
  const float PI = 3.14f;
  float x, y, z, xy;
  float sectorStep = 2 * PI / 10;
  float stackStep = PI / 10;
  float sectorAngle, stackAngle;
  std::vector<float> vtx_info;

  for (int i = 0; i <= 10; i++) {
    stackAngle = PI / 2 - i * stackStep; // Começar no pólo norte
    xy = radius * std::cos(stackAngle);
    z = radius * std::sin(stackAngle);
    for (int j = 0; j <= 10; j++) { // Rodar linha longitudinal
      sectorAngle = j * sectorStep;
      x = xy * std::cos(sectorAngle);
      y = xy * std::sin(sectorAngle);
      // XYZ
      vtx_info.emplace_back(x);
      vtx_info.emplace_back(y);
      vtx_info.emplace_back(z);
    }
  }
  m_VB.Push(GL_ARRAY_BUFFER, vtx_info.size() * sizeof(float), vtx_info.data(),
            GL_STATIC_DRAW);
  m_Layout.Push<float>(3, "Vertex Coordinates");

  // Preparar IBO
  std::vector<int> indices;
  int p1, p2;
  for (int i = 0; i < 10; i++) {
    p1 = i * 11;
    p2 = p1 + 11;
    for (int j = 0; j < 10; j++, p1++, p2++) {
      // Fragmentos do pólo norte podem ser descritos por um triângulo dado que
      // os vértices com phi = 180 colapsam no mesmo ponto.
      if (i != 0) {
        indices.emplace_back(p1);
        indices.emplace_back(p2);
        indices.emplace_back(p1 + 1);
      }
      // Fragmentos do pólo sul podem ser descritos por um triângulo dado que os
      // vértices com phi = 180 colapsam no mesmo ponto.
      if (i != 9) {
        indices.emplace_back(p1 + 1);
        indices.emplace_back(p2);
        indices.emplace_back(p2 + 1);
      }
    }
  }
  m_IB.Push(indices.data(), indices.size());
  m_VA.AddBuffer(m_VB, m_Layout);
}

CGRASphere::~CGRASphere() {}

void CGRASphere::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();

  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  SetUniform4f(color, "u_Colors");
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*--------------------------+
|         Cilindro          |
+---------------------------*/

CGRACylinder::CGRACylinder() {
  const float raio = 0.5f;
  const float PI = 3.14f;
  float theta = 0.0f;
  std::vector<float> vtx_info;
  // Círculo inferior
  for (int i = 0; i < 10; i++) {
    // XYZ
    vtx_info.emplace_back(std::cos(theta) * raio);
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(std::sin(theta) * raio);
    theta += 2 * PI / 10;
  }
  theta = 0.0f;
  // Círculo superior
  for (int i = 0; i < 10; i++) {
    // XYZ
    vtx_info.emplace_back(std::cos(theta) * raio);
    vtx_info.emplace_back(-0.5f);
    vtx_info.emplace_back(std::sin(theta) * raio);
    theta += 2 * PI / 10;
  }
  m_Layout.Push<float>(3, "Vertex Coordinates");
  m_VB.Push(GL_ARRAY_BUFFER, vtx_info.size() * sizeof(float), vtx_info.data(),
            GL_STATIC_DRAW);
  std::vector<int> indices;
  for (int i = 0; i < 10; i++) {
    indices.emplace_back(i);
    indices.emplace_back(i + 1);
    indices.emplace_back(i + 10);
    indices.emplace_back(i + 10);
    indices.emplace_back(i + 1);
    indices.emplace_back(i + 11);
  }
  // Caso degenerado
  indices.pop_back();
  indices.pop_back();
  indices.pop_back();
  indices.emplace_back(0);
  indices.emplace_back(9);
  indices.emplace_back(10);

  m_IB.Push(indices.data(), indices.size());
  m_VA.AddBuffer(m_VB, m_Layout);
}

CGRACylinder::~CGRACylinder() {}

void CGRACylinder::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();

  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  SetUniform4f(color, "u_Colors");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*--------------------------+
|         Cone              |
+---------------------------*/

CGRACone::CGRACone() {
  const float raio = 0.5f;
  const float PI = 3.14f;
  float theta = 0.0f;
  std::vector<float> vtx_info;
  // Vértice ápice primeiro (índice 0)
  vtx_info.emplace_back(0.0f);
  vtx_info.emplace_back(1.0f);
  vtx_info.emplace_back(0.0f);

  // Vértices do círculo no chao (x e z variam, y = 0.0f)
  for (int i = 0; i < 10; i++) {
    // XYZ
    vtx_info.emplace_back(std::cos(theta) * raio);
    vtx_info.emplace_back(-0.5f);
    vtx_info.emplace_back(std::sin(theta) * raio);
    theta += 2 * PI / 10;
  }
  theta = 0.0f;
  for (int i = 0; i < 10; i++) {
    // XYZ
    vtx_info.emplace_back(std::cos(theta) * raio);
    vtx_info.emplace_back(-0.5f);
    vtx_info.emplace_back(std::sin(theta) * raio);
    theta += 2 * PI / 10;
  }
  m_Layout.Push<float>(3, "Vertex Coordinates");
  m_VB.Push(GL_ARRAY_BUFFER, vtx_info.size() * sizeof(float), vtx_info.data(),
            GL_STATIC_DRAW);
  std::vector<int> indices;
  // Ligar vértice ápice aos do chão
  for (int i = 0; i < 10; i++) {
    indices.emplace_back(0);
    indices.emplace_back(i + 1);
    indices.emplace_back(i + 10);
    indices.emplace_back(0);
    indices.emplace_back(i + 1);
    indices.emplace_back(i + 11);
  }
  // Caso degenerado
  indices.pop_back();
  indices.pop_back();
  indices.pop_back();
  indices.emplace_back(0);
  indices.emplace_back(9);
  indices.emplace_back(10);

  m_IB.Push(indices.data(), indices.size());
  m_VA.AddBuffer(m_VB, m_Layout);
}

CGRACone::~CGRACone() {}

void CGRACone::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();

  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  SetUniform4f(color, "u_Colors");
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}
