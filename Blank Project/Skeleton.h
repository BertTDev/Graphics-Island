#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/MeshAnimation.h"
class Shader;
class Light;
class Camera;
class Skeleton : public SceneNode
{
public:
	Skeleton(Mesh* m, Shader* s, Light* l, Camera* c, MeshMaterial* mm, Vector3 pos, float sc, MeshAnimation* ma, int startFrame);
	~Skeleton() {};
	void SetUniforms(const OGLRenderer& r) override;
	void Draw(const OGLRenderer& r) override;
	void Update(float dt) override;
protected:
	MeshMaterial* material;
	MeshAnimation* anim;
	static bool texLoaded;
	static vector<GLuint> matTextures;
	float scale = 8;
	int currentFrame;
	float frameTime;
};

