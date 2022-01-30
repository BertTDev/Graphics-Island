#include "HeightMap.h"
#include <iostream>
#include <algorithm>
#include <random>
HeightMap::HeightMap(const std::string& name)
{
	data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);
	if (!data) {
		std::cout << "Heightmap can't load file!\n";
		return;
	}
	iWidth *= 2;
	iHeight *= 2;
	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	GenerateRandIndices();
	ManipulateHeightMap();
	
	SOIL_free_image_data(data);

	int i = 0;
	for (int z = 0; z < iHeight - 1; ++z) {
		for (int x = 0; x < iWidth - 1; ++x) {
			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;

			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}

	GenerateNormals();
	GenerateTangents();
	BufferData();

	heightmapSize.x = vertexScale.x * (iWidth - 1);
	heightmapSize.y = vertexScale.y * 255.0f;
	heightmapSize.z = vertexScale.z * (iHeight - 1);
}

Vector3 HeightMap::GetRandomVertexPos()
{
	currentPos = (currentPos + 1) % randIndicesPos.size();
	return vertices[indices[randIndicesPos[currentPos]]];
}

void HeightMap::GenerateRandIndices()
{
	randIndicesPos = vector<GLuint>();
	for (GLuint i = 0; i < numIndices; ++i) {
		randIndicesPos.push_back(i);
	}
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(randIndicesPos.begin(), randIndicesPos.end(), g);
}

void HeightMap::ManipulateHeightMap()
{
	for (int z = 0; z < iHeight; ++z) {
		for (int x = 0; x < iWidth; ++x) {
			float y;
			int offset = (z * iWidth) + x;
			if (z < iHeight / 4 || z>(3 * iHeight / 4) || x < iWidth / 4 || x >(3 * iWidth / 4)) {
				y = baseLevel;
			}
			else {
				int dataOffset = ((z - (iHeight / 4)) * (iWidth / 2)) + (x - (iWidth / 4));
				y = ((float)data[dataOffset] / 256);
				float midX = ((float)(x - iWidth / 4) / 128) - 1.0f;
				float midZ = ((float)(z - iHeight / 4) / 128) - 1.0f;
				float para = -0.5f * (midX * midX + midZ * midZ);
				y *= 3.0f * para + 2;

				y += 5 * para + 2;
				y *= 0.5f;
				y *= 256;
				if (y < baseLevel) y = baseLevel;
			}
			vertices[offset] = Vector3(x, y, z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}
	}
}
