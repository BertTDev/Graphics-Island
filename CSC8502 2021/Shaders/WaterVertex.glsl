#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform vec3 cameraPos;
uniform vec3 lightPos;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out Vertex{
  vec3 colour;
  vec2 texCoord;
  vec3 worldPos;
  vec4 clipPos;
  vec3 toCamVec;
  vec3 fromLightVec;
} OUT;

const float tileScale = 20.0;
void main(void){
  mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
  OUT.texCoord = vec2(position.x/2.0 +0.5, position.y/2.0 + 0.5) * tileScale;
  vec4 worldPos = (modelMatrix * vec4(position, 1.0));
  vec4 cp = (projMatrix* viewMatrix) * worldPos;
  OUT.clipPos = cp;
  OUT.toCamVec = cameraPos - worldPos.xyz;
  OUT.fromLightVec = worldPos.xyz - lightPos;
  OUT.worldPos = worldPos.xyz;
  gl_Position = cp;
}
