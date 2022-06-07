#version 330 core

in VS_OUT {
  vec2 uv;
} fs_in;
uniform sampler2D breakingNewsTexture;

out vec4 color;

void main(){

  //if(fs_in.uv[0] < 0 || fs_in.uv[1] > 1.0) discard;
  //if(fs_in.uv[1] < 0 || fs_in.uv[1] > 1.0) discard;
  vec4 lookup =  texture( breakingNewsTexture, fs_in.uv);
  if(lookup.a < 0.5) discard;
  color = lookup;

}
