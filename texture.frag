#version 330 core

in vec4 v_Color;
in vec2 v_TexCoords;
//smooth in vec2 texcords;
uniform sampler2D u_Texture;
out vec4 f_Color;

void main(){
    vec4 texColor = texture(u_Texture, v_TexCoords);
    if (texColor.rgb == vec3(0.0f, 0.0f, 0.0f))
        f_Color = vec4(v_TexCoords.xy, 0.0f, 1.0f);
    else
        f_Color = texColor * vec4(1.0f, 1.0f, 1.0f, 0.2);
}