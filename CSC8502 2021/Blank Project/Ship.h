#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshMaterial.h"
class Shader;
class Light;
class Camera;
class Ship : public SceneNode
{
public:
	Ship(Mesh* m, Shader* s, Light* l, Camera* c, MeshMaterial* mm, Vector3 pos, float sc);
	~Ship() {};
	void SetUniforms(const OGLRenderer& r) override;
	void Draw(const OGLRenderer& r) override;
	void Update(float dt) override;

protected:
	MeshMaterial* material;
	static bool texLoaded;
	static vector<GLuint> matTextures;
	float scale = 8;
	Vector3 position;

	Vector3 yDir = Vector3(0, 1, 0);
	float bobSize = 7.0f;
	float bobSpeed = 10.0f;
	float heightMin;
	float heightMax;
};

