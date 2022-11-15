#version 330 core

in vec4 v_Color;
in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_FragPos;


uniform sampler2D u_Texture;
uniform vec3 u_AmbientLightColor;
uniform vec3 u_LightColor;
uniform vec3 u_LightPos;
uniform vec3 u_CameraPosition;

out vec4 f_Color;

void main(){
    float AmbientStrength = 0.8f;
//    float DiffuseStrength = 1.0f;
    float DirectionalDiffStrength = 0.5f;
    float SpecularStrength = 0.5f;
    float Shininess = 32.0f;
    vec3 FragColor = vec3(1.0f);
    vec3 DirectionalLightDirection = vec3(1.0f, 0.0f, 0.0f);
  //  vec3 LightDirection = normalize(u_LightPos - v_FragPos);
  //  float DiffDot = max(0.0f, dot(v_Normal, LightDirection));
    float DirectionalDiffDot = max(0.0f, dot(v_Normal, DirectionalLightDirection));
    vec3 DirectionalDiffuseLight = u_AmbientLightColor * DirectionalDiffDot;
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml
    vec3 ReflectDirection = reflect(-DirectionalLightDirection, v_Normal);
    vec3 ViewDirection = normalize(u_CameraPosition - v_FragPos);
    float SpecularPower = pow(dot(ViewDirection, ReflectDirection), Shininess);
    
    vec4 texColor = texture(u_Texture, v_TexCoords);
    vec3 AmbientLight = u_AmbientLightColor * AmbientStrength;
    if (texColor.rgb == vec3(0.0f, 0.0f, 0.0f)){
            f_Color = v_Color;
        }
    else {
        FragColor = texColor.rgb * v_Color.rgb;
        vec3 AmbientReflectedComponent =  min(vec3(1.0f), FragColor * AmbientLight);
        vec3 DirectionalDiffuseComponent = min(vec3(1.0f), FragColor * DirectionalDiffDot * DirectionalDiffStrength);
        vec3 SpecularLightComponent = min(vec3(1.0f), /*v_Color.rgb*/ AmbientLight * SpecularPower * SpecularStrength);
        vec3 ColorSum = /*AmbientReflectedComponent + DirectionalDiffuseComponent*/ + SpecularLightComponent;
        //f_Color = min(vec4(1.0f), texColor * v_Color);
        f_Color = min(vec4(1.0f), vec4(ColorSum.rgb, 1.0f));
        f_Color.rgb = SpecularLightComponent;
    }
      

//    f_Color = vec4(v_TexCoords.xy, 0.0f, 1.0f);
        //f_Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}