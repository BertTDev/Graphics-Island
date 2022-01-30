#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/MeshAnimation.h"
#include "Terrain.h"
#include "Tree.h"
#include "Ship.h"
#include "Skeleton.h"
#include "../nclgl/CubeRobot.h"
#include <algorithm>

const int POST_PASSES = 8;
Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR"noise.png");
	GenerateWaterTextures();
	BuildSkybox();
	if (!cubeMap) return;
	BuildShaders();
	if (!reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader->LoadSuccess()) return;

	Vector3 heightmapSize = heightMap->GetHeightMapSize();
	camera = new Camera(-45.0f, 0.0f, (Vector3(175.0f, 100.0f, 350.0f)));
	light = new Light(heightmapSize * Vector3(0.5f, 8.0f, 0.5f), Vector4(0.9f, 0.9f, 0.9f, 1.0f), heightmapSize.x * 10);
	projMatrix = Matrix4::Perspective(1, 8000, (float)width / (float)height, 45);

	BuildScene();
	BuildFBOs();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glCullFace(GL_BACK);

	dudvScroll = 0.0f;
	xPressTimer = 0;
	vPressTimer = 0;
	init = true;
}

Renderer::~Renderer(void) {
	delete root;
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera();
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	root->Update(dt);
	dudvScroll += dudvFactor * dt;
	if (dudvScroll >= 1) dudvScroll = 0;
	if(xPressTimer > 0) xPressTimer -= dt;
	if (vPressTimer > 0) vPressTimer -= dt;
}
void Renderer::setWireframeMode()
{
	if (xPressTimer <= 0) {
		quad->wireframes();
		xPressTimer = 0.1f;
	}
}

void Renderer::SetBlur() {
	if (vPressTimer <= 0) {
		showBlur = !showBlur;
		vPressTimer = 0.1f;
	}
}
void Renderer::GenerateWaterTextures()
{
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(waterTex, true);
	waterDudv = SOIL_load_OGL_texture(TEXTUREDIR"waterdudvmap.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(waterDudv, true);
	waterBump = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(waterBump, true);
}
void Renderer::BuildSkybox()
{
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"SkyBack.png", TEXTUREDIR"SkyFront.png",
		TEXTUREDIR"SkyTop.png", TEXTUREDIR"SkyBottom.png",
		TEXTUREDIR"SkyRight.png", TEXTUREDIR"SkyLeft.png",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
}
void Renderer::BuildShaders()
{
	reflectShader = new Shader("WaterVertex.glsl", "WaterFragment.glsl");
	skyboxShader = new Shader("SkyboxVertex.glsl", "SkyboxFragment.glsl");
	objectShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	lightShader = new Shader("BumpVertex.glsl", "BumpFragment.glsl");
	processShader = new Shader("TexturedVertex.glsl", "ProcessFrag.glsl");
	sceneShader = new Shader("CombineVert.glsl", "DOFFragment.glsl");
}
void Renderer::BuildScene()
{
	root = new SceneNode();
	SceneNode* terrain = new Terrain(heightMap, lightShader, light, camera, Vector4(0, 1, 0, waterLevel));
	terrain->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	root->AddChild(terrain);


	Mesh* tree = Mesh::LoadFromMeshFile("PalmTreeDualStraight.msh");
	MeshMaterial* treeMat = new MeshMaterial("PalmTreeDualStraight.mat");

	Mesh* ship = Mesh::LoadFromMeshFile("Colonial Ship.msh");
	MeshMaterial* shipMat = new MeshMaterial("Colonial Ship.mat");

	Mesh* skeleton = Mesh::LoadFromMeshFile("Skeleton@Skin.msh");
	MeshMaterial* skeleMat = new MeshMaterial("Skeleton@Skin.mat");
	MeshAnimation* skeleAnim = new MeshAnimation("Idle.anm");


	Vector3 pos;
	for (int i = 0; i < 100; ++i) {
		do {
			pos = heightMap->GetRandomVertexPos();
		} while (pos.y > 250 || pos.y < 70);
		Tree* t = new Tree(tree, objectShader, light, camera, treeMat, pos, 8);
		root->AddChild(t);
	}
	Vector3 shipPos = heightMap->GetHeightMapSize()/6;
	shipPos = Vector3(shipPos.x, waterLevel, shipPos.z);
	Ship* shipNode = new Ship(ship, objectShader, light, camera, shipMat, shipPos, 5);
	root->AddChild(shipNode);

	Shader* skeleShader = new Shader("SkinningVertex.glsl", "TexturedFragment.glsl");
	const int SKELE_NUM = 2;
	Vector3 skelePos[SKELE_NUM];
	skelePos[0] = Vector3(30, 135, 20);
	skelePos[1] = Vector3(-32, 120, 130);
	for (int i = 0; i < SKELE_NUM; ++i) {
		Skeleton* skele = new Skeleton(skeleton, skeleShader, light, camera, skeleMat, skelePos[i], 16, skeleAnim, i * 20);
		shipNode->AddChild(skele);
	}
}
void Renderer::BuildFBOs()
{
	glGenFramebuffers(1, &reflectionFBO);
	glGenFramebuffers(1, &refractionFBO);

	GenerateScreenTexture(reflectionColourTex);
	GenerateScreenTexture(reflectionDepthTex, true);
	GenerateScreenTexture(refractionColourTex);
	GenerateScreenTexture(refractionDepthTex, true);

	GLenum buffers[1] = {
		GL_COLOR_ATTACHMENT0
	};

	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, reflectionDepthTex, 0);
	glDrawBuffers(1, buffers);

	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionDepthTex, 0);
	glDrawBuffers(1, buffers);

	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 3; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &processFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::RenderReflectionBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width, height);
	root->SetAllPlanes(Vector4(0, 1, 0, -waterLevel + 1.0f));

	float distance = 2 * (camera->GetPosition().y - waterLevel);
	camera->SetPosition(camera->GetPosition() - Vector3(0, distance, 0));
	camera->invertPitch();
	viewMatrix = camera->BuildViewMatrix();
	DrawSkybox();
	DrawNodes();
	camera->SetPosition(camera->GetPosition() + Vector3(0, distance, 0));
	camera->invertPitch();
	viewMatrix = camera->BuildViewMatrix();
}
void Renderer::RenderRefractionBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width, height);
	root->SetAllPlanes(Vector4(0, -1, 0, waterLevel));
	DrawSkybox();
	DrawNodes();
}
void Renderer::RenderWithWater()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, width, height);
	root->SetAllPlanes(Vector4(0, -1, 0, 100000));
	DrawSkybox();
	DrawWater();
	DrawNodes();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::RenderScene() {
	DrawScene();
	DrawPostProcess();
	PresentScene();
}

void Renderer::BuildNodeLists(SceneNode* from) {
	Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));
	if (from->GetColour().w < 1.0f) transparentNodeList.push_back(from);
	else nodeList.push_back(from);
	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) BuildNodeLists((*i));
}
void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByShaderProgram);
}
void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}
void Renderer::DrawNodes() {
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	for (const auto& i : nodeList)				DrawNode(i);
	for (const auto& i : transparentNodeList)	DrawNode(i);
}
void Renderer::DrawNode(SceneNode* n) {
	if (currentShader != n->GetShaderNum()) {
		BindShader(n->GetShader());
		currentShader = n->GetShaderNum();
	}
	if (n->GetMesh()) {
		if(n->upMats()) UpdateShaderMatrices();
		if (n->upLight()) SetShaderLight(*n->GetLight());
		n->Draw(*this);
	}
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::DrawScene()
{
	projMatrix = Matrix4::Perspective(1, 8000, (float)width / (float)height, 45.0f);
	glEnable(GL_CLIP_DISTANCE0);
	glCullFace(GL_BACK);
	BuildNodeLists(root);
	SortNodeLists();
	RenderReflectionBuffer();
	RenderRefractionBuffer();
	RenderWithWater();
	ClearNodeLists();
}
void Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(processShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(processShader->GetProgram(), "sceneTex"), 0);
	bool firstPass = true;
	for (int i = 0; i < POST_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[2], 0);
		if(firstPass) glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		else  glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		firstPass = false;
		quad->Draw();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[2]);
		quad->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(sceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glActiveTexture(GL_TEXTURE1);
	if(showBlur) glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
	else glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "sceneTex"), 0);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "blurTex"), 1);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "depthTex"), 2);
	quad->Draw();
}

void Renderer::DrawWater() {
	BindShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform1f(glGetUniformLocation(reflectShader->GetProgram(), "scrollFactor"), dudvScroll);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "reflectTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "refractTex"), 1);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "dudvMap"), 2);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "bumpTex"), 3);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "depthTex"), 4);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionColourTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractionColourTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, waterDudv);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, waterBump);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, refractionDepthTex);

	Vector3 hSize = heightMap->GetHeightMapSize();
	modelMatrix = Matrix4::Translation((hSize * 0.5f) + Vector3(0, (-255.0f/2.0f) + waterLevel, 0)) *
		Matrix4::Scale(hSize) *
		Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Rotation(180,Vector3(0,1,0));

	UpdateShaderMatrices();
	quad->Draw();
}