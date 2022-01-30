#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 nodeColour;
uniform vec4 plane;

in vec3 position;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex{
  vec2 texCoord;
  vec3 normal;
  vec4 colour;
  vec3 worldPos;
  vec3 tangent;
  vec3 binormal;
} OUT;

void main(void){

  vec4 worldPos = (modelMatrix * vec4(position,1));
  gl_ClipDistance[0] = dot(worldPos, plane);
  OUT.worldPos = worldPos.xyz;
  gl_Position = (projMatrix*viewMatrix* worldPos);

  mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
  vec3 wNormal = normalize(normalMatrix* normalize(normal));
  vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

  OUT.normal = wNormal;
  OUT.tangent = wTangent;
  OUT.binormal = cross(wTangent,wNormal) * tangent.w;

  OUT.texCoord = texCoord;
  OUT.colour = nodeColour;
}
