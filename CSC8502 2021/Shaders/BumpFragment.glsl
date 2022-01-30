#version 330 core

uniform sampler2D dirtTex;
uniform sampler2D dirtBump;
uniform sampler2D rockTex;
uniform sampler2D rockBump;
uniform sampler2D sandTex;
uniform sampler2D sandBump;
uniform sampler2D dirtRough;
uniform sampler2D rockRough;
uniform sampler2D sandRough;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex{
  vec4 colour;
  vec2 texCoord;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
  vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
  vec3 incident = normalize(lightPos - IN.worldPos);
  vec3 viewDir = normalize(cameraPos - IN.worldPos);
  vec3 halfDir = normalize(incident + viewDir);

  mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal),normalize(IN.normal));

  vec4 diffuse;
  vec3 bumpNormal;
  float roughness = 0;
  float rockLevel = 200;
  float dirtLevel = 50;
  float mergeLevel = 100;

  if(IN.worldPos.y >= rockLevel + mergeLevel){
    diffuse = texture(rockTex, IN.texCoord);
    bumpNormal = texture(rockBump, IN.texCoord).rgb;
    roughness = texture(rockRough,IN.texCoord).r;
  }
   else if(IN.worldPos.y >= rockLevel){
     vec4 rdiffuse = texture(rockTex, IN.texCoord);
     vec3 rbumpNormal = texture(rockBump, IN.texCoord).rgb;
     float rroughness = texture(rockRough,IN.texCoord).r;
     vec4 ddiffuse = texture(dirtTex, IN.texCoord);
     vec3 dbumpNormal = texture(dirtBump, IN.texCoord).rgb;
     float droughness = texture(dirtRough,IN.texCoord).r;

     float h = (IN.worldPos.y - rockLevel)/mergeLevel;
     diffuse = (rdiffuse * h) + (ddiffuse * (1-h));
     bumpNormal = (rbumpNormal * h) + (dbumpNormal * (1-h));
     roughness = (rroughness * h) + (droughness * (1-h));
  }
  else if(IN.worldPos.y >= dirtLevel + mergeLevel){
    diffuse = texture(dirtTex, IN.texCoord);
    bumpNormal = texture(dirtBump, IN.texCoord).rgb;
    roughness = texture(dirtRough,IN.texCoord).r;
  }
  else if(IN.worldPos.y >= dirtLevel) {
    vec4 rdiffuse = texture(dirtTex, IN.texCoord);
    vec3 rbumpNormal = texture(dirtBump, IN.texCoord).rgb;
    float rroughness = texture(dirtRough,IN.texCoord).r;
    vec4 ddiffuse = texture(sandTex, IN.texCoord);
    vec3 dbumpNormal = texture(sandBump, IN.texCoord).rgb;
    float droughness = texture(sandRough,IN.texCoord).r;

    float h = (IN.worldPos.y - dirtLevel)/mergeLevel;
    diffuse = (rdiffuse * h) + (ddiffuse * (1-h));
    bumpNormal = (rbumpNormal * h) + (dbumpNormal * (1-h));
    roughness = (rroughness * h) + (droughness * (1-h));
  }
   else {
    diffuse = texture(sandTex, IN.texCoord);
    bumpNormal = texture(sandBump, IN.texCoord).rgb;
    roughness = texture(sandRough,IN.texCoord).r;
  }

  bumpNormal = normalize(TBN * normalize((bumpNormal* 2.0) - 1.0));

  float lambert = max(dot(incident,bumpNormal), 0.0f);
  float distance = length(lightPos - IN.worldPos);
  float attenuation = 1.0f - clamp(distance / lightRadius,0.0,1.0);

  float specFactor = clamp(dot(halfDir, bumpNormal) ,0.0,1.0);
  specFactor = pow(specFactor, 60 - (60 * roughness));

  vec3 surface = (diffuse.rgb * lightColour.rgb);
  fragColour.rgb = surface * lambert * attenuation;
  fragColour.rgb += (lightColour.rgb * specFactor)* attenuation*0.33;
  fragColour.rgb += surface * 0.55;
  fragColour.a = 1;
}
