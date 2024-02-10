#pragma once

#ifndef GLFW_KEY_LAST
#include <GLFW/glfw3.h>
#endif

#include <iostream>

using namespace std;

class Input {
public:
	Input();
	~Input();
	bool getKeyDown(int key);
	bool getKeyJustDown(int key);
	void update();
	void keyCallback(int& key, int& action);
	int arrayLength;
private:
	bool* down;
	bool* jdown;
};