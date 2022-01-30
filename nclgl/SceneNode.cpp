#include "SceneNode.h"
Shader* SceneNode::defaultShader = NULL;
SceneNode::SceneNode(Mesh* mesh, Vector4 colour, Shader* s, Vector4 p) {
	this->mesh = mesh;
	this->colour = colour;
	if (s == NULL) {
		if (SceneNode::defaultShader == NULL)  SceneNode::defaultShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
		s = SceneNode::defaultShader;
	}
	this->shader = s;
	this->waterPlane = p;
	progNum = shader->GetProgram();
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	texture = 0;
	updateMats = true;
	updateLights = false;
}
SceneNode::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}
void SceneNode::SetUniforms(const OGLRenderer& r)
{
	Matrix4 model = this->GetWorldTransform() * Matrix4::Scale(this->GetModelScale());
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)& this->GetColour());
	texture = this->GetTexture();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTexture"), 0);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "plane"), 1, (float*)& waterPlane);
}
void SceneNode::AddChild(SceneNode* s) {
	children.push_back(s);
	s->parent = this;
}
void SceneNode::Draw(const OGLRenderer& r) {
	SetUniforms(r);
	if (mesh) mesh->Draw();
}

void SceneNode::SetTextureRepeating(GLuint target, bool state)
{
	glBindTexture(GL_TEXTURE_2D, target);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, state ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, state ? GL_REPEAT : GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneNode::SetAllPlanes(Vector4 p)
{
	this->waterPlane = p;
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->SetAllPlanes(p);
	}
}

void SceneNode::Update(float dt) {
	if (parent) worldTransform = parent->worldTransform * transform;
	else worldTransform = transform;
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) (*i)->Update(dt);
}