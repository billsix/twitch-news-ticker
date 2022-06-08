#version 330 core
in vec4 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    if(TexCoords.x < -0.92)
      discard;
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords.zw).r);
    color = vec4(textColor, 1.0) * sampled;
}