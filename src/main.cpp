#include <cassert>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <map>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

#include "Camera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "TextureCube.h"
#include "FrameBuffer.h"

using namespace std;
using namespace Eigen;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = "./"; // Where the resources are loaded from

shared_ptr<Camera> camera;
shared_ptr<Program> progReflect;
shared_ptr<Program> progRefract;
shared_ptr<Program> progRotate;
shared_ptr<Program> progSky;
shared_ptr<Shape> shape;
shared_ptr<Shape> sky;
shared_ptr<TextureCube> textureCube;
shared_ptr<FrameBuffer> frameBuf;

bool keyToggles[256] = {false}; // only for English keyboards!

// This function is called when a GLFW error occurs
static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

// This function is called when a key is pressed
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

// This function is called when the mouse is clicked
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	// Get the current mouse position.
	double xmouse, ymouse;
	glfwGetCursorPos(window, &xmouse, &ymouse);
	// Get current window size.
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if(action == GLFW_PRESS) {
		bool shift = (mods & GLFW_MOD_SHIFT) != 0;
		bool ctrl  = (mods & GLFW_MOD_CONTROL) != 0;
		bool alt   = (mods & GLFW_MOD_ALT) != 0;
		camera->mouseClicked((float)xmouse, (float)ymouse, shift, ctrl, alt);
	}
}

// This function is called when the mouse moves
static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if(state == GLFW_PRESS) {
		camera->mouseMoved((float)xmouse, (float)ymouse);
	}
}

static void char_callback(GLFWwindow *window, unsigned int key)
{
	keyToggles[key] = !keyToggles[key];
}

// If the window is resized, capture the new size and reset the viewport
static void resize_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// This function is called once to initialize the scene and OpenGL
static void init()
{
	// Initialize time.
	glfwSetTime(0.0);
	
	// Set background color.
	glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);
	
	camera = make_shared<Camera>();
	camera->setInitDistance(3.0f);

	//I CAN'T BELIEVE I SPENT 3 HOURS TRYING TO DEBUG SOME MISSING DLL ISSUE WHEN
	//ALL I HAD TO DO WAS INITIALIZE THE FRAMEBUFFER OBJECT UNBELIEVABLE
	frameBuf = make_shared<FrameBuffer>();
	frameBuf->init();

	progSky = make_shared<Program>();
	progSky->setShaderNames(RESOURCE_DIR + "sky_vert.glsl", RESOURCE_DIR + "sky_frag.glsl");
	progSky->setVerbose(false);
	progSky->init();
	progSky->addAttribute("aPos");
	progSky->addAttribute("aNor");
	progSky->addUniform("P");
	progSky->addUniform("MV");
	progSky->addUniform("cubemap");
	
	progReflect = make_shared<Program>();
	progReflect->setShaderNames(RESOURCE_DIR + "reflect_vert.glsl", RESOURCE_DIR + "reflect_frag.glsl");
	progReflect->setVerbose(false);
	progReflect->init();
	progReflect->addAttribute("aPos");
	progReflect->addAttribute("aNor");
	progReflect->addUniform("P");
	progReflect->addUniform("V");
	progReflect->addUniform("M");
	progReflect->addUniform("Mit");
	progReflect->addUniform("cPos");
	progReflect->addUniform("cubemap");
	
	progRefract = make_shared<Program>();
	progRefract->setShaderNames(RESOURCE_DIR + "reflect_vert.glsl", RESOURCE_DIR + "refract_frag.glsl");
	progRefract->setVerbose(false);
	progRefract->init();
	progRefract->addAttribute("aPos");
	progRefract->addAttribute("aNor");
	progRefract->addUniform("P");
	progRefract->addUniform("V");
	progRefract->addUniform("M");
	progRefract->addUniform("Mit");
	progRefract->addUniform("cPos");
	progRefract->addUniform("cubemap");

	progRotate = make_shared<Program>();
	progRotate->setShaderNames(RESOURCE_DIR + "rotate_vert.glsl", RESOURCE_DIR + "rotate_frag.glsl");
	progRotate->setVerbose(false);
	progRotate->init();
	progRotate->addAttribute("aPos");
	progRotate->addAttribute("aNor");
	progRotate->addUniform("P");
	progRotate->addUniform("V");
	progRotate->addUniform("M");
	progRotate->addUniform("Mit");
	progRotate->addUniform("time");
	progRotate->addUniform("cPos");
	progRotate->addUniform("cubemap");
	progRotate->addUniform("switchPos");
	progRotate->addUniform("lightPos");
	
	sky = make_shared<Shape>();
	sky->loadMesh(RESOURCE_DIR + "sphere.obj");
	sky->init();
	
	shape = make_shared<Shape>();
	shape->loadMesh(RESOURCE_DIR + "sphere2.obj");
	shape->init();
	
	textureCube = make_shared<TextureCube>();
	textureCube->setFilenames(RESOURCE_DIR + "cubemapFG/left.jpg",
							  RESOURCE_DIR + "cubemapFG/right.jpg",
							  RESOURCE_DIR + "cubemapFG/bottom.jpg",
							  RESOURCE_DIR + "cubemapFG/top.jpg",
							  RESOURCE_DIR + "cubemapFG/front.jpg",
							  RESOURCE_DIR + "cubemapFG/back.jpg");
	textureCube->init();
	GLSL::checkError(GET_FILE_LINE);
}

// This function is called every frame to draw the scene.
static void render()
{
	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	camera->setAspect((float)width/(float)height);
	
	// Matrix stacks
	auto P = make_shared<MatrixStack>();
	auto V = make_shared<MatrixStack>();
	auto M = make_shared<MatrixStack>();
	
	// Apply projection matrix
	P->pushMatrix();
	camera->applyProjectionMatrix(P);
	// Apply view matrix
	V->pushMatrix();
	camera->applyViewMatrix(V);

	// Draw shape
	Vector4f vecLight = Vector4f(-250.0f, 250.0f, -500.0f, 1.0f);
	Matrix4f Mit = M->topMatrix().inverse();
	Matrix4f C = V->topMatrix().inverse(); // camera matrix is the view matrix inverse
	Vector3f cPos = C.block<3, 1>(0, 3); // camera position in world coords is in the last column
	
	P->pushMatrix();
	V->pushMatrix();
	//Render scene into framebuffer to create cubemap
	frameBuf->bindFBO(frameBuf->getFBOid(), 2048, 2048);
	//Draw the scene from in each x/y/z direction
	for (int i = 0; i < 6; i++) {
		frameBuf->drawToSide(V, P, i);

		// Draw sky sphere
		progSky->bind();
		textureCube->bind(progSky->getUniform("cubemap"));
		M->pushMatrix();
			M->scale(500.0f);
			glUniformMatrix4fv(progSky->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
			Matrix4f MV = V->topMatrix() * M->topMatrix();
			glUniformMatrix4fv(progSky->getUniform("MV"), 1, GL_FALSE, MV.data());
			sky->draw(progSky);
		M->popMatrix();
		textureCube->unbind();
		progSky->unbind();

		//Draw moving spheres
		progRotate->bind();
		M->pushMatrix();
			M->scale(0.5f);
			glUniformMatrix4fv(progRotate->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
			glUniformMatrix4fv(progRotate->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
			glUniformMatrix4fv(progRotate->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
			glUniform3fv(progRotate->getUniform("cPos"), 1, cPos.data());
			glUniform3fv(progRotate->getUniform("lightPos"), 1, vecLight.segment<3>(0).data());
			glUniform1f(progRotate->getUniform("time"), glfwGetTime() * 2.0f);
			glUniform1i(progRotate->getUniform("switchPos"), 1);
			shape->draw(progRotate);
		M->popMatrix();
		progRotate->unbind();

		progRotate->bind();
		M->pushMatrix();
			M->scale(0.5f);
			glUniformMatrix4fv(progRotate->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
			glUniformMatrix4fv(progRotate->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
			glUniformMatrix4fv(progRotate->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
			glUniform3fv(progRotate->getUniform("cPos"), 1, cPos.data());
			glUniform3fv(progRotate->getUniform("lightPos"), 1, vecLight.segment<3>(0).data());
			glUniform1f(progRotate->getUniform("time"), glfwGetTime() * 2.0f);
			glUniform1i(progRotate->getUniform("switchPos"), 2);
			shape->draw(progRotate);
		M->popMatrix();
		progRotate->unbind();

		progRotate->bind();
		M->pushMatrix();
		M->scale(0.5f);
		glUniformMatrix4fv(progRotate->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
		glUniformMatrix4fv(progRotate->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
		glUniformMatrix4fv(progRotate->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
		glUniform3fv(progRotate->getUniform("cPos"), 1, cPos.data());
		glUniform3fv(progRotate->getUniform("lightPos"), 1, vecLight.segment<3>(0).data());
		glUniform1f(progRotate->getUniform("time"), glfwGetTime() * 2.0f);
		glUniform1i(progRotate->getUniform("switchPos"), 3);
		shape->draw(progRotate);
		M->popMatrix();
		progRotate->unbind();
	}
	P->popMatrix();
	V->popMatrix();

	//Render to screen
	shared_ptr<Program> prog;
	frameBuf->bindFBO(0, (float) width, (float) height);
	//Pick what fragment model we want, for demo purposes
	if (keyToggles[(unsigned) 'r'])
		prog = progRefract;
	else
		prog = progReflect;

	prog->bind();

	//Enable/Disable dynamic cube map, for demo purposes
	if (keyToggles[(unsigned) 'q']) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, frameBuf->getTextureID());
		glUniform1i(prog->getUniform("cubemap"), 0);
	}
	else {
		textureCube->bind(prog->getUniform("cubemap"));
	}

	//Redraw all objects to the screen
	M->pushMatrix();	
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
		glUniformMatrix4fv(prog->getUniform("Mit"), 1, GL_TRUE, Mit.data());
		glUniform3fv(prog->getUniform("cPos"), 1, cPos.data());
		shape->draw(prog);
	M->popMatrix();
	textureCube->unbind();
	prog->unbind();

	progSky->bind();
	textureCube->bind(progSky->getUniform("cubemap"));
	M->pushMatrix();
		M->scale(500.0f);
		glUniformMatrix4fv(progSky->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
		Matrix4f MV = V->topMatrix() * M->topMatrix();
		glUniformMatrix4fv(progSky->getUniform("MV"), 1, GL_FALSE, MV.data());
		sky->draw(progSky);
	M->popMatrix();
	textureCube->unbind();
	progSky->unbind();

	progRotate->bind();
	M->pushMatrix();
		M->scale(0.5f);
		glUniformMatrix4fv(progRotate->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
		glUniformMatrix4fv(progRotate->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
		glUniformMatrix4fv(progRotate->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
		glUniform3fv(progRotate->getUniform("cPos"), 1, cPos.data());
		glUniform3fv(progRotate->getUniform("lightPos"), 1, vecLight.segment<3>(0).data());
		glUniform1f(progRotate->getUniform("time"), glfwGetTime() * 2.0f);
		glUniform1i(progRotate->getUniform("switchPos"), 1);
		shape->draw(progRotate);
	M->popMatrix();
	progRotate->unbind();

	progRotate->bind();
	M->pushMatrix();
		M->scale(0.5f);
		glUniformMatrix4fv(progRotate->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
		glUniformMatrix4fv(progRotate->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
		glUniformMatrix4fv(progRotate->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
		glUniform3fv(progRotate->getUniform("cPos"), 1, cPos.data());
		glUniform3fv(progRotate->getUniform("lightPos"), 1, vecLight.segment<3>(0).data());
		glUniform1f(progRotate->getUniform("time"), glfwGetTime() * 2.0f);
		glUniform1i(progRotate->getUniform("switchPos"), 2);
		shape->draw(progRotate);
	M->popMatrix();
	progRotate->unbind();

	progRotate->bind();
	M->pushMatrix();
		M->scale(0.5f);
		glUniformMatrix4fv(progRotate->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
		glUniformMatrix4fv(progRotate->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
		glUniformMatrix4fv(progRotate->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
		glUniform3fv(progRotate->getUniform("cPos"), 1, cPos.data());
		glUniform3fv(progRotate->getUniform("lightPos"), 1, vecLight.segment<3>(0).data());
		glUniform1f(progRotate->getUniform("time"), glfwGetTime() * 2.0f);
		glUniform1i(progRotate->getUniform("switchPos"), 3);
		shape->draw(progRotate);
	M->popMatrix();
	progRotate->unbind();

	V->popMatrix();
	P->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Dynamic Cubemaps", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	GLSL::checkVersion();
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set mouse button callback.
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// Set the window resize call back.
	glfwSetFramebufferSizeCallback(window, resize_callback);
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
