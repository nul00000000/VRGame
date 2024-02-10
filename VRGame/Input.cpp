#include "Input.h"

Input::Input() {
	arrayLength = GLFW_KEY_LAST + 1;
	down = new bool[arrayLength];
	jdown = new bool[arrayLength];
	for (int i = 0; i < arrayLength; i++) {
		down[i] = false;
		jdown[i] = false;
	}
}

Input::~Input() {
	delete[] down;
	delete[] jdown;
}

bool Input::getKeyDown(int key) {
	return down[key];
}

bool Input::getKeyJustDown(int key) {
	return jdown[key];
}

//should always go at the end of a tick
void Input::update() {
	for (int i = 0; i < arrayLength; i++) {
		jdown[i] = false;
	}
}

void Input::keyCallback(int& key, int& action) {
	if (key < 0) {
		printf("Recieved unknown key code: %n", &key);
	}
	if (action == GLFW_PRESS) {
		down[key] = true;
		jdown[key] = true;
	} else if(action == GLFW_RELEASE) {
		down[key] = false;
	}
}