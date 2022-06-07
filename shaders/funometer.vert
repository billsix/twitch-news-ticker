#version 330 core

layout (location = 0) in vec2 vPosition;

out VS_OUT {
  vec2 uv;
} vs_out;

uniform mat4 mvpMatrix;

void
main(){
   vs_out.uv = (vPosition + 1.0) / 2.0;
   gl_Position = mvpMatrix * vec4(vPosition,0.0,1.0);
}
