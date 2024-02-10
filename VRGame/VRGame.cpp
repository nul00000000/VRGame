#include "VRGame.h"
#include "Input.h"
#include "shaders/shader.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "primitives/model.h"
#include <openvr.h>
#include "fbo.h"
#include "perlin.h"
#include "chunk.h"
#include "world.h"
#include "particle.h"

#define WINDOW_TITLE "haha title go brrrrrrrrrrrrr"

using namespace std;

Input* input = new Input();

BasicShader* shader;

World* world;

Particle* leftP;
Particle* rightP;

FBO* rightFBO;
FBO* leftFBO;

vr::Texture_t rightTex;
vr::Texture_t leftTex;

vr::IVRSystem* vrsys;
vr::TrackedDevicePose_t* tdp;
vr::VRActiveActionSet_t* aas;

glm::mat4* tracked;

vr::VRActionHandle_t actionPlace = 0;
vr::VRActionHandle_t actionBreak = 0;
vr::VRActionHandle_t actionMove = 0;

vr::VRActionSetHandle_t actionSetMain = 0;

uint8_t currentBlock = 1;

void errorCallback(GLint err, const GLchar* desc) {
	fprintf(stderr, "Debugging time: %s\n", desc);
}

void keyCallback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods) {
	input->keyCallback(key, action);
}

glm::mat4* eyes = new glm::mat4[2];

glm::vec3 camPos(0, 0, 0);
glm::vec3 camVel(0, 0, 0);

GLfloat a = 0;

bool wireframe = false;

glm::mat4 hmdToGLM(vr::HmdMatrix44_t hmd) {
	glm::mat4 dest;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			dest[i][j] = hmd.m[j][i];
		}
	}
	return dest;
}

glm::mat4 hmdToGLM(vr::HmdMatrix34_t hmd) {
	glm::mat4 dest;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			dest[i][j] = hmd.m[j][i];
		}
		dest[i][3] = 0.0f;
	}
	dest[3][3] = 1.0f;
	return dest;
}

void createEyesViewMatrix(glm::vec3 translation, glm::mat4 hmdTrans) {
	eyes[0] = hmdToGLM(vrsys->GetEyeToHeadTransform(vr::Eye_Left));
	eyes[0] = glm::translate(translation) * hmdTrans * eyes[0];
	eyes[1] = hmdToGLM(vrsys->GetEyeToHeadTransform(vr::Eye_Right));
	eyes[1] = glm::translate(translation) * hmdTrans * eyes[1];
}

void update(GLdouble delta) {
	a += delta;
	if (input->getKeyJustDown(GLFW_KEY_F)) {
		wireframe = !wireframe;
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	vr::VRCompositor()->WaitGetPoses(tdp, vr::k_unMaxTrackedDeviceCount, nullptr, NULL);
	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
		if (tdp[i].bPoseIsValid) {
			tracked[i] = hmdToGLM(tdp[i].mDeviceToAbsoluteTracking);
		}
	}
	if (input->getKeyJustDown(GLFW_KEY_1)) {
		currentBlock = 1;
	} else if (input->getKeyJustDown(GLFW_KEY_2)) {
		currentBlock = 2;
	} else if (input->getKeyJustDown(GLFW_KEY_3)) {
		currentBlock = 3;
	}
	vr::VRActiveActionSet_t set = { actionSetMain, vr::k_ulInvalidInputValueHandle, NULL };
	vr::VRInput()->UpdateActionState(&set, sizeof(vr::VRActiveActionSet_t), 1);
	vr::InputDigitalActionData_t input = {0};
	vr::InputDigitalActionData_t input2 = { 0 };
	vr::InputAnalogActionData_t input3 = { 0 };
	vr::VRInput()->GetDigitalActionData(actionPlace, &input, sizeof(vr::InputDigitalActionData_t), vr::k_ulInvalidInputValueHandle);
	vr::VRInput()->GetDigitalActionData(actionBreak, &input2, sizeof(vr::InputDigitalActionData_t), vr::k_ulInvalidInputValueHandle);
	vr::VRInput()->GetAnalogActionData(actionMove, &input3, sizeof(vr::InputAnalogActionData_t), vr::k_ulInvalidInputValueHandle);
	glm::quat rotL;
	glm::vec3 scale;
	glm::vec3 posL;
	glm::quat rotR;
	glm::vec3 posR;
	glm::vec3 skew;
	glm::vec4 pers;
	glm::decompose(tracked[1], scale, rotL, posL, skew, pers);
	glm::decompose(tracked[2], scale, rotR, posR, skew, pers);
	leftP->pos = posL + camPos;
	leftP->rot = rotL;
	rightP->pos = posR + camPos;
	rightP->rot = rotR;
	rotL = glm::conjugate(rotL);
	rotR = glm::conjugate(rotR);
	cout << camPos.y << endl;
	camVel.y -= 9.8f * delta;
	glm::vec3 movement = glm::vec3(0, 0, -0.04 * input3.x) * rotL;
	movement.y = 0;
	camVel += movement;
	camPos += camVel;
	camVel.x *= 0.8f;
	camVel.y *= 0.99f;
	camVel.z *= 0.8f;
	camPos.y -= 2;
	if (world->inSolid(camPos)) {
		camPos = world->mansp(camPos);
		camVel.y = 0;
	}
	camPos.y += 2;
	if (input.bState || input2.bState) {
		world->setBlock(round((posR.x + camPos.x) / BLOCK_SIZE), round((posR.y + camPos.y) / BLOCK_SIZE), round((posR.z + camPos.z) / BLOCK_SIZE), input.bState ? currentBlock : 0);
	}
	world->update();
	//camPos.y = (double) world->getHighestBlock((int)camPos.x / BLOCK_SIZE, (int)camPos.z / BLOCK_SIZE) * BLOCK_SIZE;
}

//void GLAPIENTRY
//MessageCallback(GLenum source,
//	GLenum type,
//	GLuint id,
//	GLenum severity,
//	GLsizei length,
//	const GLchar* message,
//	const void* userParam)
//{
//	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
//		(type == GL_DEBUG_TYPE_ERROR ? " GL ERROR " : ""),
//		type, severity, message);
//}

void render() {
	shader->loadLights();
	createEyesViewMatrix(camPos, tracked[vr::k_unTrackedDeviceIndex_Hmd]);
	
	glm::mat4 proj;
	proj = hmdToGLM(vrsys->GetProjectionMatrix(vr::Eye_Right, 0.1, 100));
	glm::vec3 sky = world->getSkyColor();

	glEnable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_FRAMEBUFFER, rightFBO->rbHandle);
	glViewport(0, 0, rightFBO->width, rightFBO->height);
	glClearColor(sky.x, sky.y, sky.z, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	world->render(shader, glm::inverse(eyes[1]), proj);
	leftP->render(shader);
	rightP->render(shader);
	glDisable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, rightFBO->rbHandle);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightFBO->handle);

	glBlitFramebuffer(0, 0, rightFBO->width, rightFBO->height, 0, 0, rightFBO->width, rightFBO->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	proj = hmdToGLM(vrsys->GetProjectionMatrix(vr::Eye_Left, 0.1, 100));

	glEnable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_FRAMEBUFFER, leftFBO->rbHandle);
	glViewport(0, 0, leftFBO->width, leftFBO->height);
	glClearColor(sky.x, sky.y, sky.z, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	world->render(shader, glm::inverse(eyes[0]), proj);
	leftP->render(shader);
	rightP->render(shader);
	glDisable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, leftFBO->rbHandle);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftFBO->handle);

	glBlitFramebuffer(0, 0, leftFBO->width, leftFBO->height, 0, 0, leftFBO->width, leftFBO->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	glClearColor(sky.x, sky.y, sky.z, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	world->render(shader, glm::inverse(eyes[0]), proj);
	leftP->render(shader);
	rightP->render(shader);

	leftTex.eColorSpace = vr::EColorSpace::ColorSpace_Gamma;
	leftTex.eType = vr::ETextureType::TextureType_OpenGL;
	leftTex.handle = (void*)(uintptr_t)(leftFBO->resolveHandle);

	rightTex.eColorSpace = vr::EColorSpace::ColorSpace_Gamma;
	rightTex.eType = vr::ETextureType::TextureType_OpenGL;
	rightTex.handle = (void*)(uintptr_t)(rightFBO->resolveHandle);
	vr::VRCompositor()->Submit(vr::Eye_Left, &leftTex, NULL);
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightTex, NULL);
}

int main(int argc, char** args) {
	srand(time(NULL));
	if (!glfwInit()) {
		fprintf(stderr, "GLFW initiation killed itself\n");
		return 1;
	}
	glfwSetErrorCallback(errorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	GLFWwindow* window = glfwCreateWindow(1280, 720, WINDOW_TITLE, NULL, NULL);
	if (!window) {
		fprintf(stderr, "Window creation failed, your graphics driver may not support OpenGL 4.5 or something, which is required. \nThere's (probably) no troubleshooting this, believe me.\nYou will die of nausea if you try to use whatever shit graphics card you have for VR anyway.");
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glfwSetKeyCallback(window, keyCallback);

	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(MessageCallback, 0);

	shader = new BasicShader(1280, 720);
	world = new World(12345);
	camPos.y = CHUNK_SIZE * BLOCK_SIZE;
	GLfloat* cursorMesh = new GLfloat[36]{
		0, 0.02, 0, 0, -0.02, 0, 0, 0, -0.02,
		0, -0.02, 0, 0, 0.02, 0, 0, 0, 0.02,
		0.02, 0, 0, -0.02, 0, 0, 0, 0, -0.02,
		-0.02, 0, 0, 0.02, 0, 0, 0, 0, 0.02
	};
	Particle::initModel();
	leftP = new Particle();
	rightP = new Particle();

	vr::EVRInitError pe;

	if (!vr::VR_IsHmdPresent())
	{
		cerr << "No HMD attached to the system" << endl;
		return 1;
	}

	if (!vr::VR_IsRuntimeInstalled())
	{
		cerr << "OpenVR Runtime not found" << endl;
		return 1;
	}

	vrsys = vr::VR_Init(&pe, vr::EVRApplicationType::VRApplication_Scene);

	if (pe != vr::EVRInitError::VRInitError_None) {
		cerr << vr::VR_GetVRInitErrorAsEnglishDescription(pe);
		return 1;
	}

	if (!vr::VRCompositor()) {
		cerr << "uh oh no vr compositor go BRRRRRR" << endl;
	}

	vr::VRInput()->SetActionManifestPath("C:/Users/Admin/Documents/manifest.json");

	vr::VRInput()->GetActionHandle("/actions/main/in/place", &actionPlace);
	vr::VRInput()->GetActionHandle("/actions/main/in/break", &actionBreak);
	vr::VRInput()->GetActionHandle("/actions/main/in/move", &actionMove);
	vr::VRInput()->GetActionSetHandle("/actions/main", &actionSetMain);

	tdp = new vr::TrackedDevicePose_t[vr::k_unMaxTrackedDeviceCount];
	aas = new vr::VRActiveActionSet_t[2];
	tracked = new glm::mat4[vr::k_unMaxTrackedDeviceCount];

	cout << "OpenVR loaded" << endl;

	uint32_t w;
	uint32_t h;
	vrsys->GetRecommendedRenderTargetSize(&w, &h);
	rightFBO = new FBO(w, h);
	leftFBO = new FBO(w, h);

	GLdouble a;
	GLdouble delta = 0;
	shader->use();
	while (!glfwWindowShouldClose(window)) {
		a = glfwGetTime();
		update(delta);
		render();

		input->update();
		glfwPollEvents();

		//Should always be at the end
		glfwSwapBuffers(window);
		delta = glfwGetTime() - a;
	}
	vr::VR_Shutdown();
	glfwDestroyWindow(window);
	glfwTerminate();
	delete world;
	delete shader;
	delete input;
	delete rightFBO;
	delete leftFBO;
	delete[] tdp;
	delete[] aas;
	delete[] eyes;
	return 0;
}