#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"

class SceneNode
{
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1), Shader* s = NULL, Vector4 p = Vector4(1,1,1,1));
	~SceneNode(void);

	void SetTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	Shader* GetShader() const { return shader; }
	void SetShader(Shader* s) {
		shader = s; 
		progNum = shader->GetProgram();
	}

	GLuint GetShaderNum() const { return progNum; }
	virtual void SetUniforms(const OGLRenderer& r);

	void AddChild(SceneNode* s);

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() const { return texture; }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

	static bool CompareByShaderProgram(SceneNode* a, SceneNode* b) {
		return (b->GetShaderNum() < a->GetShaderNum());
	}

	bool upMats() { return updateMats; }
	bool upLight() { return updateLights; }

	void SetLight(Light* l) { light = l; }
	Light* GetLight() { return light; }

	void SetCamera(Camera* c) { camera = c; }
	Camera* GetCamera() { return camera; }

	void SetPlane(Vector4 p) { waterPlane = p; }
	void SetAllPlanes(Vector4 p);
	Vector4 getPlane() { return waterPlane; }

	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer& r);
	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }

protected:
	SceneNode* parent;
	Mesh* mesh;
	Shader* shader;
	static Shader* defaultShader;
	GLuint progNum;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;

	Matrix4 viewMat;
	Matrix4 projMat;
	Vector4 colour;
	std::vector<SceneNode*> children;

	void SetTextureRepeating(GLuint target, bool state);
	bool updateMats;
	bool updateLights;

	float distanceFromCamera;
	float boundingRadius;
	GLuint texture;

	Light* light;
	Camera* camera;
	Vector4 waterPlane;
};


