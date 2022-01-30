#include "Skeleton.h"
bool Skeleton::texLoaded = false;
vector<GLuint> Skeleton::matTextures = vector<GLuint>();

Skeleton::Skeleton(Mesh* m, Shader* s, Light* l, Camera* c, MeshMaterial* mm, Vector3 pos, float sc, MeshAnimation* ma, int startFrame)
{
	this->SetMesh(m);
	this->SetShader(s);
	this->SetCamera(c);
	this->material = mm;
	this->SetColour(Vector4(1, 1, 1, 0.9f));
	this->scale = sc;
	this->SetModelScale(Vector3(scale, scale, scale));
	this->SetTransform(Matrix4::Translation(pos - Vector3(0, scale * 6, 0)));
	this->anim = ma;
	Light* li = new Light();
	li->SetPosition(pos + Vector3(-100, 200, -100));
	li->SetRadius(1000);
	this->SetLight(li);
	if (!Skeleton::texLoaded) {
		Skeleton::texLoaded = true;
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

	currentFrame = startFrame;
	frameTime = 0.0f;
}

void Skeleton::SetUniforms(const OGLRenderer& r)
{
	Matrix4 model = this->GetWorldTransform() * Matrix4::Scale(this->GetModelScale());
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "plane"), 1, (float*)& waterPlane);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	vector<Matrix4> frameMatrices;
	const Matrix4* invBindPose = mesh->GetInverseBindPose();
	const Matrix4* frameData = anim->GetJointData(currentFrame);
	for (unsigned int i = 0; i < mesh->GetJointCount(); ++i) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}
	int j = glGetUniformLocation(shader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
}

void Skeleton::Draw(const OGLRenderer& r)
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

void Skeleton::Update(float dt)
{
	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		frameTime += 1.0f / anim->GetFrameRate();
	}
	SceneNode::Update(dt);
}