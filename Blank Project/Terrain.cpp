#include "Terrain.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"

Terrain::Terrain(Mesh* m, Shader* s, Light* l, Camera* c, Vector4 wp )
{
	this->SetMesh(m);
	this->SetShader(s);
	this->SetModelScale(Vector3(1, 1, 1));
	this->light = l;
	this->camera = c;
	this->waterPlane = wp;
	this->heightMap = (HeightMap*)m;
	updateMats = true;
	updateLights = true;

	dirtTex = SOIL_load_OGL_texture(TEXTUREDIR"Dirt Diffuse.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	dirtBump = SOIL_load_OGL_texture(TEXTUREDIR"Dirt Bump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	dirtRough = SOIL_load_OGL_texture(TEXTUREDIR"Dirt Rough.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockTex = SOIL_load_OGL_texture(TEXTUREDIR"Rock Diffuse.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockBump = SOIL_load_OGL_texture(TEXTUREDIR"Rock Bump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockRough = SOIL_load_OGL_texture(TEXTUREDIR"Rock Rough.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	sandTex = SOIL_load_OGL_texture(TEXTUREDIR"Sand Diffuse.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	sandBump = SOIL_load_OGL_texture(TEXTUREDIR"Sand Bump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	sandRough = SOIL_load_OGL_texture(TEXTUREDIR"Sand Rough.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	SetTextureRepeating(dirtTex, true);
	SetTextureRepeating(dirtBump, true);
	SetTextureRepeating(dirtRough, true);
	SetTextureRepeating(rockTex, true);
	SetTextureRepeating(rockBump, true);
	SetTextureRepeating(rockRough, true);
	SetTextureRepeating(sandBump, true);
	SetTextureRepeating(sandTex, true);
	SetTextureRepeating(sandRough, true);

}

void Terrain::SetUniforms(const OGLRenderer& r)
{
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "dirtTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dirtTex);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "dirtBump"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, dirtBump);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "rockTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, rockTex);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "rockBump"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, rockBump);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "sandTex"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, sandTex);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "sandBump"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, sandBump);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "dirtRough"), 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, dirtRough);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "rockRough"), 7);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, rockRough);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "sandRough"), 8);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, sandRough);


	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "plane"), 1, (float*)& waterPlane);
}
