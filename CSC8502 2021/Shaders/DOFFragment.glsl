#version 330 core

uniform sampler2D sceneTex;
uniform sampler2D blurTex;
uniform sampler2D depthTex;

in Vertex{
  vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void){
  vec3 scene = texture(sceneTex, IN.texCoord).xyz;
  vec3 blur = texture(blurTex, IN.texCoord).xyz;
  float depth = texture(depthTex, IN.texCoord).x;
  float startZ = 0.8;
  depth = clamp(((depth-startZ)*(1.0/startZ)),0.0,1.0);
  fragColour.xyz = mix(scene,blur,depth);
  fragColour.a = 1.0;
}
