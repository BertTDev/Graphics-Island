#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshMaterial.h"
class Shader;
class Light;
class Camera;
class Tree : public SceneNode
{
public:
	Tree(Mesh* m, Shader* s, Light* l, Camera* c, MeshMaterial* mm, Vector3 pos, float sc);
	~Tree() {};
	void SetUniforms(const OGLRenderer& r) override;
	void Draw(const OGLRenderer& r) override;
protected:
	GLuint treeDiffuse;
	GLuint treeBump;
	MeshMaterial* material;
	static bool texLoaded;
	static vector<GLuint> matTextures;
	float scale = 8;
};

