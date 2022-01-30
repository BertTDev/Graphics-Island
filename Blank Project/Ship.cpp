#include "Ship.h"
#include <cmath>
bool Ship::texLoaded = false;
vector<GLuint> Ship::matTextures = vector<GLuint>();

Ship::Ship(Mesh* m, Shader* s, Light* l, Camera* c, MeshMaterial* mm, Vector3 pos, float sc)
{
	this->SetMesh(m);
	this->SetShader(s);
	this->SetCamera(c);
	this->material = mm;
	this->SetColour(Vector4(1, 1, 1, 0.9f));
	this->scale = sc;
	this->SetModelScale(Vector3(scale, scale, scale));
	this->SetTransform(Matrix4::Translation(pos));
	this->position = pos + Vector3(0,5,0);
	if (!Ship::texLoaded) {
		Ship::texLoaded = true;
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

	heightMin = this->position.y - bobSize;
	heightMax = this->position.y + bobSize;

	Light* li = new Light();
	li->SetPosition( pos + Vector3(-200,300,-200));
	li->SetRadius(1000);
	this->SetLight(li);

	updateMats = true;
	updateLights = true;
}

void Ship::SetUniforms(const OGLRenderer& r)
{
	Matrix4 model = this->GetWorldTransform() * Matrix4::Scale(this->GetModelScale());
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)& this->GetColour());
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "plane"), 1, (float*)& waterPlane);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "useNormalTexture"), 0);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "ambientVal"), 0.3);
}

void Ship::Draw(const OGLRenderer& r)
{
	SetUniforms(r);
	if (this->mesh) {
		for (int i = 0; i < this->mesh->GetSubMeshCount(); ++i) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, matTextures[i]);
			mesh->DrawSubMesh(i);
		}
	}
}

void Ship::Update(float dt)
{
	if (position.y >= heightMax) {
		yDir = Vector3(0, -1, 0);
		position.y = heightMax;
	}
	if (position.y <= heightMin) {
		yDir = Vector3(0, 1, 0);
		position.y = heightMin;
	}
	Vector3 movDir = yDir * bobSpeed * dt;
	position.y += movDir.y;
	this->SetTransform(Matrix4::Translation(this->position));

	SceneNode::Update(dt);
}
