#pragma once
#include <string>
#include <vector>
#include "Mesh.h"
class HeightMap : public Mesh
{
public:
	HeightMap(const std::string& name);
	~HeightMap(void) {};

	Vector3 GetHeightMapSize() const { return heightmapSize; }
	Vector3 GetRandomVertexPos();
protected:
	void GenerateRandIndices();
	void ManipulateHeightMap();

	unsigned char* data;
	int iWidth;
	int iHeight;
	int iChans;

	Vector3 vertexScale = Vector3(16.0f, 1.0f, 16.0f);
	Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);
	float baseLevel = -10.0f;

	Vector3 heightmapSize;
	vector<GLuint> randIndicesPos;
	GLuint currentPos;
};

