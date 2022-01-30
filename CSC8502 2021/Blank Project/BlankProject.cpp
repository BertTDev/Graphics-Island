#include "../NCLGL/window.h"
#include "Renderer.h"

Renderer* renderer;
Window*  w;
bool lockMouse = true;
float lockTimer = 0;
float updateTitleTimer = 0;

void UpdateKeys(float dt) {
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_X)) renderer->setWireframeMode();
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_V)) renderer->SetBlur();
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) Shader::ReloadAllShaders();

	if (lockTimer > 0.0f) lockTimer -= dt;
	else if (Window::GetKeyboard()->KeyDown(KEYBOARD_C)) {
		lockMouse = !lockMouse;
		lockTimer = 0.1f;
		w->LockMouseToWindow(lockMouse);
		w->ShowOSPointer(!lockMouse);
	}
}

void UpdateFPS(float dt) {
	if (updateTitleTimer <= 0) {
		float fps = 1.0f / dt;
		std::string title = "Graphics Island - FPS: " + std::to_string(fps);
		w->SetTitle(title);
		updateTitleTimer += 0.1f;
	}
	else updateTitleTimer -= dt;
}

int main() {
	w = new Window("Graphics Island: ", 1024, 720, false);
	if (!w->HasInitialised()) return -1;
	w->LockMouseToWindow(lockMouse);
	w->ShowOSPointer(!lockMouse);

	renderer = new Renderer(*w);
	if (!renderer->HasInitialised()) return -1;

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		renderer->UpdateScene(dt);
		renderer->RenderScene();
		renderer->SwapBuffers();
		UpdateKeys(dt);
		UpdateFPS(dt);
	}
	delete renderer;
	delete w;
	return 0;
}