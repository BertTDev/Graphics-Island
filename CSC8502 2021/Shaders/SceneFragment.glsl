#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform int useNormalTexture;
uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform float ambientVal;

in Vertex{
  vec2 texCoord;
  vec3 normal;
  vec4 colour;
  vec3 worldPos;
  vec3 tangent;
  vec3 binormal;
} IN;

out vec4 fragColour;

void main(void) {
  vec3 incident = normalize(lightPos - IN.worldPos);
  vec3 viewDir = normalize(cameraPos - IN.worldPos);
  vec3 halfDir = normalize(incident + viewDir);

   vec4 diffuse;
   diffuse = texture(diffuseTex, IN.texCoord);

   vec3 normal;
   if(useNormalTexture > 0){
      normal = texture(normalTex,IN.texCoord).rgb;
      mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal),normalize(IN.normal));
      normal = normalize(TBN * normalize((normal* 2.0) - 1.0));
  }
  else normal = IN.normal;

  float lambert = max(dot(incident,normal), 0.0f);
  float distance = length(lightPos - IN.worldPos);
  float attenuation = 1.0 - clamp(distance / lightRadius,0.0,1.0);

  float specFactor = clamp(dot(halfDir, normal),0.0,1.0);
  specFactor = pow(specFactor, 60);

  vec3 surface = (diffuse.rgb * lightColour.rgb);
  fragColour.rgb = surface * lambert * attenuation;
  fragColour.rgb += (lightColour.rgb * specFactor)* attenuation*0.33;
  fragColour.rgb += surface * ambientVal;
  fragColour.a = diffuse.a;
}
