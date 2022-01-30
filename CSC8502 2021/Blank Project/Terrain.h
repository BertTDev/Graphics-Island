#pragma once
#include "../nclgl/SceneNode.h"
class Shader;
class Light;
class Camera;
class HeightMap;
class Terrain : public SceneNode
{
public:
	Terrain(Mesh* m, Shader* s, Light* l, Camera* c, Vector4 wp);
	~Terrain() { delete heightMap; };

	HeightMap* GetHeightMap() { return heightMap; }

	void SetUniforms(const OGLRenderer& r) override;
protected:
	GLuint dirtTex;
	GLuint dirtBump;
	GLuint dirtRough;
	GLuint rockTex;
	GLuint rockBump;
	GLuint rockRough;
	GLuint sandTex;
	GLuint sandBump;
	GLuint sandRough;
	HeightMap* heightMap;
};

