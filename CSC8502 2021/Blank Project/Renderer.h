#pragma once
#include "../nclgl/OGLRenderer.h"
#include"../nclgl/Frustum.h"
class Camera;
class Shader;
class HeightMap;
class SceneNode;

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

	void setWireframeMode();
	void SetBlur();
protected:
	void GenerateWaterTextures();
	void BuildSkybox();
	void BuildShaders();
	void BuildScene();
	void BuildFBOs();

	void RenderReflectionBuffer();
	void RenderRefractionBuffer();
	void RenderWithWater();
	void DrawScene();
	void DrawPostProcess();
	void PresentScene();

	void DrawWater();
	void DrawSkybox();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* processShader;
	Shader* sceneShader;
	Shader* objectShader;
	Shader* dofShader;
	SceneNode* root;

	HeightMap* heightMap;
	Mesh* quad;

	Light* light;
	Camera* camera;

	GLuint cubeMap;

	void GenerateScreenTexture(GLuint& into, bool depth = false);

	GLuint waterTex;
	GLuint waterDudv;
	GLuint waterBump;
	float dudvFactor = 0.03f;
	float dudvScroll;

	GLuint reflectionFBO;
	GLuint reflectionColourTex;
	GLuint reflectionDepthTex;
	
	GLuint refractionFBO;
	GLuint refractionColourTex;
	GLuint refractionDepthTex;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[3];
	GLuint bufferDepthTex;

	float waterLevel = 30;
	float xPressTimer;
	float vPressTimer;

	int POST_PASSES = 30;
	bool showBlur = false;
	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
	GLuint currentShader = 1000000;
};

