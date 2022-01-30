#version 400

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 plane;

in vec3 position;
in vec2 texCoord;
in vec4 jointWeights;
in ivec4 jointIndices;

uniform mat4 joints[128];

out Vertex{
  vec2 texCoord;
  vec4 colour;
} OUT;

void main(void){

  vec4 localPos = vec4(position,1.0f);
  vec4 skelPos = vec4(0,0,0,0);
  vec4 col = vec4(0,0,0,1);
  for(int i = 0; i < 4; ++i){
    int jointIndex = jointIndices[i];
    float jointWeight = jointWeights[i];
    if(jointWeight > 0 && i < 4) col[i] = 1;
    skelPos += joints[jointIndex] * localPos * jointWeight;
  }
  vec4 worldPos = modelMatrix * skelPos;
  gl_ClipDistance[0] = dot(worldPos, plane);
  mat4 mvp = projMatrix * viewMatrix * modelMatrix;
  gl_Position = mvp * vec4(skelPos.xyz,1.0);
  OUT.texCoord = texCoord;
  OUT.colour = col;
}
