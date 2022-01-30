#include "Camera.h"
#include "Window.h"
#include <algorithm>
Camera::Camera(float pitch, float yaw, Vector3 position)
{
	this->pitch = pitch;
	this->yaw = yaw;
	this->position = position;
	trackNodes[0] = position;
	trackNodes[1] = Vector3(1178,50,1270);
	trackNodes[2] = Vector3(1250, 94, 1460);
	trackNodes[3] = Vector3(1337, 94, 1571);
	trackNodes[4] = Vector3(1476, 83, 1474);
	trackNodes[5] = Vector3(4965,55, 856);
	trackNodes[6] = Vector3(6726, 55, 1912);
	trackNodes[7] = Vector3(6989,63,3762);
	trackNodes[8] = Vector3(6222, 154, 3955);
	trackNodes[9] = Vector3(5442, 427, 4037);
	trackNodes[10] = Vector3(4960, 427, 4548);
	trackNodes[11] = Vector3(4712, 323, 5049);
	trackNodes[12] = Vector3(3828, 323, 5341);
	trackNodes[13] = Vector3(2605, 141, 6403);
	trackNodes[14] = Vector3(326, 87, 5102);
}
void Camera::UpdateCamera(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::max(std::min(pitch, 90.0f), -90.0f);

	if (yaw < 0) yaw += 360.0f;
	if (yaw > 360.0f) yaw -= 360.0f;

	UpdateMovement(dt);
}

Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Scale(Vector3(scale, scale, scale)) *
		Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
}

void Camera::UpdateMovement(float dt)
{
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);

	if (zTimer > 0.0f) zTimer -= dt;
	else if (Window::GetKeyboard()->KeyDown(KEYBOARD_Z)) {
		trackMode = !trackMode;
		zTimer += 5.0f;
	}

	float speed = speedMulti * dt;
	if (trackMode) {
		Vector3 movDir = trackNodes[currentNode] - position;
		if (movDir.Length() < 5 * trackSpeedMulti) currentNode = (currentNode + 1) % TRACK_SIZE;
		movDir.Normalise();
		position += movDir * trackSpeedMulti * dt;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q)) if (trackSpeedMulti > 1) trackSpeedMulti -= 0.2f;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_E)) if (trackSpeedMulti < 500)trackSpeedMulti += 0.2f;
	}
	else {
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) position += forward * speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) position -= forward * speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) position -= right * speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) position += right * speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) position.y += speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) position.y -= speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q)) if (speedMulti > 1) speedMulti -= 0.2f;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_E)) if (speedMulti < 10000)speedMulti += 0.2f;
	}
}
