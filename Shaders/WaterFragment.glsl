#version 330 core

uniform sampler2D reflectTex;
uniform sampler2D refractTex;
uniform sampler2D dudvMap;
uniform sampler2D bumpTex;
uniform sampler2D depthTex;


uniform float scrollFactor;
uniform vec4 lightColour;

in Vertex{
  vec3 colour;
  vec2 texCoord;
  vec3 worldPos;
  vec4 clipPos;
  vec3 toCamVec;
  vec3 fromLightVec;
} IN;

out vec4 fragColour;

const float distortStrength = 0.09;
const float shineDamper = 60.0;
const float reflectivity = 0.2;
void main(void){
  vec2 ndc = (IN.clipPos.xy/ IN.clipPos.w)/2.0f + 0.5f;
  vec2 refractCoords = ndc;
  vec2 reflectCoords = vec2(ndc.x, -ndc.y);

  float near = 1;
  float far = 15000;
  float depth = texture(depthTex, refractCoords).r;
  float floorDist = 2.0 * near * far/(far + near - (2.0 * depth - 1.0) * (far-near));
  depth = gl_FragCoord.z;
  float waterDist = 2.0 * near * far/(far + near - (2.0 * depth - 1.0) * (far-near));
  float waterDepth = floorDist - waterDist;

  vec2 distort1 = distortStrength * (texture(dudvMap, vec2(IN.texCoord.x + scrollFactor, IN.texCoord.y)).rg * 2.0 - 1.0);
  vec2 distort2 =  distortStrength * (texture(dudvMap, vec2(-IN.texCoord.x + scrollFactor, IN.texCoord.y + scrollFactor)).rg * 2.0 - 1.0);
  vec2 totalDistort = (distort1 + distort2) * clamp(waterDepth / 5.0,0.0,1.0);

  refractCoords += totalDistort;
  refractCoords = clamp(refractCoords, 0.001, 0.999);
  reflectCoords += totalDistort;
  reflectCoords.x = clamp(reflectCoords.x, 0.001, 0.999);
  reflectCoords.y = clamp(reflectCoords.y, -0.999, -0.001);

  vec4 reflectColour = texture(reflectTex, reflectCoords);
  vec4 refractColour = texture(refractTex, ndc) * 0.1;

  refractColour = mix(refractColour, vec4(142.0/255.0,208.0/255.0,230.0/255.0,1.0),  clamp(waterDepth/300.0, 0.0, 1.0));
  vec4 bumpColour = texture(bumpTex, totalDistort);
  vec3 normal = vec3(bumpColour.r * 2.0 - 1.0, bumpColour.b * 3.0, bumpColour.g *2.0 - 1.0);
  normal = normalize(normal);

  vec3 viewVec = normalize(IN.toCamVec);
  float refFactor = dot(viewVec, normal);
  refFactor = pow(refFactor, 0.9);
  refFactor = clamp(refFactor, 0.0, 1.0);

  vec3 reflectedLight = reflect(normalize(IN.fromLightVec),normal);
  float specular = max(dot(reflectedLight,viewVec), 0.0);
  specular = pow(specular, shineDamper);
  vec3 specHighlights = lightColour.xyz * specular * reflectivity * clamp(waterDepth / 5.0,0.0,1.0);

  fragColour = mix(reflectColour,refractColour ,refFactor);
  fragColour = mix(fragColour,vec4(142.0/255.0,208.0/255.0,230.0/255.0,1.0),0.2);
  fragColour += vec4(specHighlights, 1.0);
  fragColour.a = clamp(waterDepth / 5.0,0.0,1.0);
}
