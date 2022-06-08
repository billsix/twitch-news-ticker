#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec4 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, -0.5, 1.0);
    TexCoords = vec4(gl_Position.xy, vertex.zw);
}