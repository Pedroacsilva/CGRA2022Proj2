#include "Camera.cpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include <cmath>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL

#ifndef GL_SETUP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GL_SETUP
#endif

/*#ifndef STB_I
#include "vendor/stb_image/stb_image.cpp"
#include "vendor/stb_image/stb_image_print.cpp"
#define STB_I
#endif

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"
#include "vendor/imgui/imgui_impl_opengl3.h"*/

#include <iostream>
#include <string>

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

#include "cgraobject.cpp"
#include "deecshader.h"

#define GLEW_STATIC

Camera camera(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
              0.0f);

/*Camera cameraCarro(glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f),
   0.0f, 0.0f);*/

DEECShader *basicShader;
GLFWwindow *window;
bool wire = false, visitorPOV = true;
float yaw = -90.0f;
// float lastX = 400, lastY = 300, yaw = -90.0f, pitch = 0.0f;
float deltaTime = 0.0f, lastFrame = 0.0f;
bool firstMouse = true;
float cameraSpeed = 4.0f;

/*void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_TRUE)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_TRUE)
    camera.m_Position += camera.m_Front * cameraSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_TRUE)
    camera.m_Position -= camera.m_Front * cameraSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_TRUE)
    camera.m_Position += camera.m_Right * cameraSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_TRUE)
    camera.m_Position -= camera.m_Right * cameraSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_TRUE) {
    wire = !wire;
    (wire) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
           : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}*/

glm::vec4 GetTranslationFromMat4(const glm::mat4 &modeltr) {
  glm::vec4 outVector;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (i == 3)
        outVector[j] = modeltr[i][j];
    }
  }
  return outVector;
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_TRUE)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_TRUE)
    camera.m_Position += camera.m_Front * cameraSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_TRUE)
    camera.m_Position -= camera.m_Front * cameraSpeed * deltaTime;

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_TRUE) {
    yaw -= cameraSpeed * deltaTime * 40;
    glm::vec3 direction(1.0f);
    direction[0] = cos(glm::radians(yaw));
    direction[1] = 0.0f;
    direction[2] = sin(glm::radians(yaw));
    camera.m_Front = glm::normalize(direction);
    camera.m_Right =
        glm::normalize(glm::cross(camera.m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
    /*    std::cout << "Yaw: " << yaw << "\n";
        std::cout << "CameraUp: " << camera.m_Up[0] <<", " << camera.m_Up[1] <<
       ", " << camera.m_Up[2] <<".\n";*/
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_TRUE) {
    yaw += cameraSpeed * deltaTime * 80;
    glm::vec3 direction(1.0f);
    direction[0] = std::cos(glm::radians(yaw));
    direction[1] = 0.0f;
    direction[2] = std::sin(glm::radians(yaw));
    camera.m_Front = glm::normalize(direction);
    camera.m_Right =
        glm::normalize(glm::cross(camera.m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
    /*    std::cout << "Yaw: " << yaw << "\n";
        std::cout << "CameraUp: " << camera.m_Up[0] <<", " << camera.m_Up[1] <<
       ", " << camera.m_Up[2] <<".\n";*/
  }
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_TRUE) {
    wire = !wire;
    (wire) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
           : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  if (glfwGetKey(window, GLFW_KEY_V) == GLFW_TRUE)
    visitorPOV = !visitorPOV;
}

/*void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  camera.m_Front = glm::normalize(direction);
  camera.m_Right =
      glm::normalize(glm::cross(camera.m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
}*/

int main(int argc, char const *argv[]) {

  // Inicialização de Glfw e Glew

  if (!glfwInit()) {
    std::cout << "Error initializing GLFW.\n";
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Projecto 1 - Pedro Silva", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, KeyCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  //  glfwSetCursorPosCallback(window, mouse_callback);

  if (glewInit() != GLEW_OK) {
    std::cout << "Error initializing GLEW.\n";
    return -1;
  }

  // Carregar & linkar shaders

  basicShader = new DEECShader;
  std::cout << "loading shaders.\n";
  if (basicShader->loadShaders("texture.vert", "texture.frag") == GL_FALSE) {
    printf("ERROR LOADING SHADERS.\n");
    exit(EXIT_FAILURE);
  }
  std::cout << "linking shaders.\n";
  if (basicShader->linkShaderProgram() == GL_FALSE) {
    printf("ERROR LINKING SHADERS.\n");
    exit(EXIT_FAILURE);
  }
  basicShader->startUsing();

  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

  // Ligar teste de profundidade. O fragmento com menor profundidade será
  // desenhado.

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Cor do céu. Aproveitar para o cenário
  glClearColor(0.53f, 0.8f, 0.92f, 1.0f);

  // Matriz de projecção: perspectiva com near plane = 0.5f e far plane = 30.0f. FOV = 80º
  glm::mat4 proj = glm::perspective(glm::radians(80.0f), 1.0f, 0.5f, 30.0f);

  // Declarar objectos
  CGRASquare chao;
  CGRACube carroCorpo, carro2Corpo;
  CGRASquare cartaz;
  CGRASphere sol;
  CGRACylinder pneu;
  CGRACylinder carroPneu, carroPneu2, carroPneu3, carroPneu4, arvoreTronco;
  CGRACone cone, arvoreFolhas;
  CGRACompound carroCorpoObj(carroCorpo);
  CGRACompound carroPneuObj(carroPneu);
  CGRACompound carroPneu2Obj(carroPneu2);
  CGRACompound carro2CorpoObj(carro2Corpo);
  CGRACompound carro2PneuObj(carroPneu3);
  CGRACompound carro2Pneu2Obj(carroPneu4);
  CGRACompound arvoreTroncoObj(arvoreTronco);
  CGRACompound arvoreFolhasObj(arvoreFolhas);

  std::vector<glm::vec3> revoPontos;
  revoPontos.emplace_back(glm::vec3(1.2f, 0.0f, 0.0f));
  revoPontos.emplace_back(glm::vec3(0.5f, 0.0f, 0.0f));

  std::vector<glm::vec3> extrPontos;
  extrPontos.emplace_back(glm::vec3(-1.0f, 2.0f, 0.0f));
  extrPontos.emplace_back(glm::vec3(1.0f, 1.0f, 0.0f));
  extrPontos.emplace_back(glm::vec3(1.0f, -1.0f, 0.0f));
  extrPontos.emplace_back(glm::vec3(-1.0f, -1.0f, 0.0f));

  std::vector<glm::vec3> conePontos;
  conePontos.emplace_back(1.0f, 0.0f, -1.0f);
  conePontos.emplace_back(0.0f, 0.0f, 0.0f);

  CGRARevolution track(revoPontos);
  CGRAExtrusion trophy(extrPontos);
  CGRARevolution cone2(conePontos);

  carroCorpo.setTexture("carrotexture.ppm");
  sol.setTexture("tattoo.ppm");
  arvoreFolhas.setTexture("treetexture.ppm");
//  arvoreTronco.setTexture("trunktexture.ppm");
//  arvoreFolhas.setTexture("treetexture.ppm");
//  arvoreTronco.setChessTexture(true);
//  sol.setChessTexture(true);
  pneu.setChessTexture(true);
  track.setShader(basicShader);
  cartaz.setShader(basicShader);
  trophy.setShader(basicShader);
  cone.setShader(basicShader);
  cone2.setShader(basicShader);
  pneu.setShader(basicShader);
  sol.setShader(basicShader);
  chao.setShader(basicShader);
  carroCorpo.setShader(basicShader);
  carro2Corpo.setShader(basicShader);
  carroPneu.setShader(basicShader);
  carroPneu2.setShader(basicShader);
  carroPneu3.setShader(basicShader);
  carroPneu4.setShader(basicShader);
  arvoreFolhas.setShader(basicShader);
  arvoreTronco.setShader(basicShader);

  glm::mat4 chaoPosition(1.0f);
  glm::mat4 cartazPosition(1.0f);
  glm::mat4 trackPosition(1.0f);
  glm::mat4 solPosition(1.0f);
  glm::mat4 pneuPosition(1.0f);
  glm::mat4 conePosition(1.0f);
  glm::mat4 cone2Position(1.0f);
  glm::mat4 trophyPosition(1.0f);
  glm::mat4 carroCorpoPosition(1.0f);
  glm::mat4 carro2CorpoPosition(1.0f);
  glm::mat4 carroPneuPosition(1.0f);
  glm::mat4 arvoreFolhasPosition(1.0f);
  glm::mat4 arvoreTroncoPosition(1.0f);

  // Deslocar objectos na cena.

  cartazPosition = glm::scale(cartazPosition, glm::vec3(5.0f, 2.5f, 3.0f));
  cartazPosition = glm::translate(cartazPosition, glm::vec3(1.5f, 0.0f, -2.0f));
  cartazPosition = glm::rotate(cartazPosition, glm::degrees(120.0f),
                               glm::vec3(0.0f, 1.0f, 0.0f));

  chaoPosition = glm::scale(chaoPosition, glm::vec3(15.0f, 15.0f, 15.0f));
  chaoPosition = glm::rotate(chaoPosition, glm::degrees(90.0f),
                             glm::vec3(1.0f, 0.0f, 0.0f));
  chaoPosition = glm::translate(chaoPosition, glm::vec3(0.0f, 0.0f, -0.001f));

  solPosition = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 8.0f, -5.0f));

  std::vector<glm::vec3> pneuLocations = {glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec3(1.0f, 0.0f, 0.0f),
                                          glm::vec3(1.0f, 1.0f, 0.0f)};

  std::vector<glm::vec3> coneLocations = {
      glm::vec3(5.9f, -0.4f, -3.0f), glm::vec3(5.9f, 0.0f, -1.5f),
      glm::vec3(5.9f, 0.0f, 0.0f), glm::vec3(5.9f, 0.5f, 1.5f),
      glm::vec3(5.9f, 0.75f, 3.0f)};

  pneuPosition = glm::scale(pneuPosition, glm::vec3(0.6f, 0.1f, 0.2f));
  pneuPosition = glm::translate(pneuPosition, glm::vec3(-1.0f, 1.5f, 0.0f));

  conePosition = glm::scale(conePosition, glm::vec3(0.3f, 0.7f, 0.3f));
  conePosition = glm::translate(conePosition, glm::vec3(2.0f, 0.0f, 0.0f));

  cone2Position = glm::scale(cone2Position, glm::vec3(0.3f, 0.7f, 0.3f));
  cone2Position = glm::translate(cone2Position, glm::vec3(-5.0f, 0.7f, 0.0f));
  cone2Position = glm::rotate(cone2Position, glm::degrees(90.0f),
                              glm::vec3(1.0f, 0.0f, 0.0f));

  trackPosition =
      glm::translate(trackPosition, glm::vec3(0.0f, 0.00001f, 0.0f));
  trackPosition = glm::rotate(trackPosition, glm::degrees(90.0f),
                              glm::vec3(1.0f, 0.0f, 0.0f));
  trackPosition = glm::scale(trackPosition, glm::vec3(5.0f, 5.0f, 5.0f));

  trophyPosition = glm::translate(trophyPosition, glm::vec3(0.0f, 1.0f, -1.5f));

  carroCorpoPosition =
      glm::translate(carroCorpoPosition, glm::vec3(5.0f, 1.0f, 0.0f));
  carro2CorpoPosition =
      glm::translate(carro2CorpoPosition, glm::vec3(7.0f, 1.0f, -2.0f));

  // Pneus do carro

  glm::mat4 pneu2corpo(1.0f);
  pneu2corpo = glm::scale(pneu2corpo, glm::vec3(0.1f, 0.5f, 0.1f));
  pneu2corpo = glm::translate(pneu2corpo, glm::vec3(1.0f, -1.5f, 2.5f));
  pneu2corpo =
      glm::rotate(pneu2corpo, glm::degrees(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  glm::mat4 pneu22corpo = pneu2corpo;
  pneu22corpo = glm::translate(pneu22corpo, glm::vec3(3.0f, 0.0f, 0.0f));

  // Conectar pneus do carro aos corpos respectivos.

  carroCorpoObj.PushChild(&carroPneuObj, pneu2corpo);
  carroCorpoObj.PushChild(&carroPneu2Obj, pneu22corpo);

  carro2CorpoObj.PushChild(&carro2PneuObj, pneu2corpo);
  carro2CorpoObj.PushChild(&carro2Pneu2Obj, pneu22corpo);

  arvoreFolhasPosition =
      glm::translate(arvoreFolhasPosition, glm::vec3(0.0f, 1.2f, 0.0f));
  arvoreFolhasPosition =
      glm::scale(arvoreFolhasPosition, glm::vec3(4.5f, 1.5f, 1.0f));

  // Conectar folhas da árvore ao tronco.
  arvoreTroncoObj.PushChild(&arvoreFolhasObj, arvoreFolhasPosition);

  arvoreTroncoPosition =
      glm::translate(arvoreTroncoPosition, glm::vec3(-7.0f, 0.3f, 0.0f));
  arvoreTroncoPosition =
      glm::scale(arvoreTroncoPosition, glm::vec3(0.3f, 1.0f, 1.0f));

  // Definir transformadas iniciais.
  chao.setModelTransformation(chaoPosition);
  cartaz.setModelTransformation(cartazPosition);
  sol.setModelTransformation(solPosition);
  pneu.setModelTransformation(pneuPosition);
  cone.setModelTransformation(conePosition);
  cone2.setModelTransformation(cone2Position);
  track.setModelTransformation(trackPosition);
  trophy.setModelTransformation(trophyPosition);
  carroCorpoObj.PropagateModelTransformation(carroCorpoPosition);
  carro2CorpoObj.PropagateModelTransformation(carro2CorpoPosition);
  arvoreTroncoObj.PropagateModelTransformation(arvoreTroncoPosition);

  // Definição de cores.
  glm::vec4 grassColor = glm::vec4(0.3f, 0.5f, 0.27f, 1.0f);
  glm::vec4 treeColor = glm::vec4(0.2f, 0.8f, 0.27f, 1.0f);
  glm::vec4 trunkColor = glm::vec4(0.39f, 0.26f, 0.13f, 1.0f);
  glm::vec4 blackColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::vec4 trackColor = glm::vec4(0.8f);
  glm::vec4 trophyColor = glm::vec4(0.97f, 0.65f, 0.01f, 1.0f);
  glm::vec4 pneuColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
  glm::vec4 solColor = glm::vec4(0.98f, 0.99f, 0.06f, 1.0f);
  glm::vec4 coneColor = glm::vec4(1.0f, 0.49f, 0.0f, 1.0f);
  glm::vec4 carroColor = glm::vec4(0.9f, 0.3f, 0.2f, 1.0f);
  glm::vec4 carro2Color = glm::vec4(0.5f, 0.1f, 0.9f, 1.0f);


  // Transmitir cores como variáveis uniformes
  trophy.SetColor(trophyColor);
  track.SetColor(trackColor);
  pneu.SetColor(pneuColor);
  carroPneu.SetColor(pneuColor);
  carroPneu2.SetColor(pneuColor);
  carroPneu3.SetColor(pneuColor);
  carroPneu4.SetColor(pneuColor);
  carroCorpo.SetColor(carroColor);
  carro2Corpo.SetColor(carro2Color);
  sol.SetColor(solColor);
  cone.SetColor(coneColor);
  cone2.SetColor(coneColor);
  chao.SetColor(grassColor);
  cartaz.SetColor(blackColor);
  arvoreFolhas.SetColor(treeColor);
  arvoreTronco.SetColor(trunkColor);

  // Preparar câmara para rastrear posição de um condutor.
  glm::vec3 cameraCarroPosition(1.0f);

  float carroTheta = 0.0f;

  glm::mat4 visitorView(1.0f);
  glm::mat4 driverView(1.0f);
  glm::mat4 activeView(1.0f);
  glm::vec4 driverPosition(1.0f);
  glm::vec3 driverPositionVec3(1.0f);
  glm::vec3 driverFront(1.0f);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 carrosPos(1.0f);
    // Incrementar rotação dos carros
    carrosPos = glm::rotate(carrosPos, glm::degrees(0.0001f),
                            glm::vec3(0.0f, 1.0f, -0.2f));

    // Obter posição do piloto (e incrementar y um bocado)
    driverPosition = GetTranslationFromMat4(carroCorpoObj.Object->modeltr);
    driverPositionVec3[0] = driverPosition[0];
    driverPositionVec3[1] =
        driverPosition[1] + 1.0f; // Acrescentar alguma altura ao condutor para
                                  // nao estar dentro do carro
    driverPositionVec3[2] = driverPosition[2];

    // Frente do condutor = up vector x posição condutor
    driverFront = glm::normalize(
        glm::cross(glm::vec3(0.0, 1.0f, 0.0f), driverPositionVec3));

    driverView =
        glm::lookAt(driverPositionVec3, driverPositionVec3 + driverFront,
                    // driverPositionVec3 + glm::vec3(0.0f, 0.0f, -1.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));

    if (visitorPOV)
      // Obter matriz de vista do visitor.
      activeView = camera.GetViewMatrix();
    else
      // Definir matriz de vista como a do condutor.
      activeView = driverView;

    // Desenhar chao
    chao.drawIt(activeView, proj);

    // Desenhar cartaz
    cartaz.drawIt(activeView, proj);

    // Desenhar pista
    track.drawIt(activeView, proj);

    // Desenhar trofeu
    trophy.drawIt(activeView, proj);

    // Desenhar pneus
    for (const auto &elemt : pneuLocations) {
      glm::mat4 location = pneuPosition;
      location = glm::translate(location, elemt);
      pneu.setModelTransformation(location);
      pneu.drawIt(activeView, proj);
    }

    // Desenhar cones
    for (const auto &elemt : coneLocations) {
      glm::mat4 location(1.0f);
      location = glm::translate(location, elemt);
      cone.setModelTransformation(location);
      cone.drawIt(activeView, proj);
    }

    // Desenhar sol
    sol.drawIt(activeView, proj);

    // Desenhar cone (revolucao)
    cone2.drawIt(activeView, proj);

    // Desenhar árvore
    arvoreTroncoObj.DrawTree(activeView, proj);

    // Desenhar carro
    carroCorpoObj.PropagateModelTransformation(carrosPos);
    carro2CorpoObj.PropagateModelTransformation(carrosPos);

    carroCorpoObj.DrawTree(activeView, proj);
    carro2CorpoObj.DrawTree(activeView, proj);

    glfwSwapBuffers(window);
    glfwPollEvents();
    carroTheta += deltaTime * 0.02f;
  }

  glfwTerminate();
  return 0;
}
