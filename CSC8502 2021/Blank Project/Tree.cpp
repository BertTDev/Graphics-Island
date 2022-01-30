#include "Tree.h"
bool Tree::texLoaded = false;
vector<GLuint> Tree::matTextures = vector<GLuint>();

Tree::Tree(Mesh* m, Shader* s, Light* l, Camera* c, MeshMaterial* mm, Vector3 pos, float sc)
{
	this->SetMesh(m);
	this->SetShader(s);
	this->SetCamera(c);
	this->material = mm; this->SetColour(Vector4(1, 1, 1, 0.9f));
	this->scale = sc;
	this->SetModelScale(Vector3(scale, scale, scale));
	this->SetTransform(Matrix4::Translation(pos - Vector3(0, scale * 6, 0)));
	Light* li = new Light();
	li->SetPosition(pos + Vector3(-100, 200, -100));
	li->SetRadius(1000);
	this->SetLight(li);
	if (!Tree::texLoaded) {
	Tree::texLoaded = true;
		for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
			const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);
			const string* filename = nullptr;
			matEntry->GetEntry("Diffuse", &filename);
			string path = TEXTUREDIR + *filename;
			GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
			SetTextureRepeating(texID, true);
			matTextures.emplace_back(texID);
		}
	}
}

void Tree::SetUniforms(const OGLRenderer& r)
{
	Matrix4 model = this->GetWorldTransform() * Matrix4::Scale(this->GetModelScale());
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)& this->GetColour());
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "plane"), 1, (float*)& waterPlane);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "useNormalTexture"), 1);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "ambientVal"), 0.9);
}

void Tree::Draw(const OGLRenderer& r)
{
	SetUniforms(r);
	if (this->mesh) {
		for (int i = (this->mesh->GetSubMeshCount() -1); i >= 0 ; i--) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, matTextures[i]);
			mesh->DrawSubMesh(i);
		}
	}
}
