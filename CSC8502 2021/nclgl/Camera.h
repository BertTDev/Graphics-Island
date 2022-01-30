#pragma once
#include "Matrix4.h"
#include "Vector3.h"
const int TRACK_SIZE = 15;
class Camera
{
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
	};

	Camera(float pitch, float yaw, Vector3 position);

	~Camera(void) {};
	void UpdateCamera(float dt = 1.0f);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }
	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }
	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }
	void invertPitch() { pitch = -pitch; }

	float GetSpeedMulti() const { return speedMulti; }
	void SetSpeedMulti(float s) { speedMulti = s; }
protected:
	void UpdateMovement(float dt);

	float yaw;
	float pitch;
	float scale = 1.0f;
	float speedMulti = 5.0f;
	float trackSpeedMulti = 5.0f;
	Vector3 position;

	float zTimer = 0;
	bool trackMode = true;
	Vector3 trackNodes[TRACK_SIZE];
	int currentNode = 0;
};


