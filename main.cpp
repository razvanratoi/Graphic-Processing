//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>
#include <chrono>
#include <thread>

int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 light2Dir;
glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;
glm::vec3 light2Color;
GLuint light2ColorLoc;

gps::Camera myCamera(
				glm::vec3(0.0f), 
				glm::vec3(0.0f, 0.1f, 0.1f),
				glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 2.f;

bool pressedKeys[1024];
float angleY = 0.0f;
float carousel = 0.0f;
float dropY = 50.f;
float heliY = 0.0f;
int isRaining = 0;
int animate = 0;
float scale = 1.f;
int manual = 0;
GLfloat lightAngle;

gps::Model3D nanosuit;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D leaf;
gps::Model3D scene;
gps::Model3D jos;
gps::Model3D sus;
gps::Model3D rain[2];
gps::Model3D heli;
gps::Model3D rotor;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

std::vector<const GLchar*> faces;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

int withFog = 0;
GLuint withFogLoc;

int night = 0;
int felinar = 1;
float t = 0.f;
int i = 0;



GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

float lastX = 400, lastY = 300;
bool firstMouse = true;
float pitch = 0.0f;
float yaw = -90.0f;
void mouse_callback(GLFWwindow* glWindow, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch -= yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	myCamera.rotate(-pitch, yaw);
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_Z]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_X]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_C]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_F]) {
		withFog = 1;
	}

	if (pressedKeys[GLFW_KEY_G]) {
		withFog = 0;
	}

	if (pressedKeys[GLFW_KEY_B]) {
		night = 0;
	}

	if (pressedKeys[GLFW_KEY_N]) {
		night = 1;
	}

	if (pressedKeys[GLFW_KEY_K]) {
		felinar = 1;
	}

	if (pressedKeys[GLFW_KEY_O]) {
		felinar = 0;
	}

	if (pressedKeys[GLFW_KEY_1]) {
		isRaining = 1;
	}

	if (pressedKeys[GLFW_KEY_2]) {
		isRaining = 0;
	}

	if (pressedKeys[GLFW_KEY_SPACE]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_R]) {
		i = 0;
		t = 0.f;
		animate = 1;
	}

	if (pressedKeys[GLFW_KEY_LEFT]) {
		scale -= 0.05f;
	}

	if (pressedKeys[GLFW_KEY_RIGHT]) {
		scale += 0.05f;
	}

	if (pressedKeys[GLFW_KEY_Y]) {
		manual = 1;
	}

	if (pressedKeys[GLFW_KEY_U]) {
		manual = 0;
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "Gheorgheni 2050", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	//glfwSetCursorPosCallback(glWindow, mouse_callback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//for input from mouse
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(glWindow, mouse_callback);


	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display 
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	//skybox
	faces.push_back("skybox/right.bmp");
	faces.push_back("skybox/left.bmp");
	faces.push_back("skybox/top.bmp");
	faces.push_back("skybox/bottom.bmp");
	faces.push_back("skybox/front.bmp");
	faces.push_back("skybox/back.bmp");
	mySkyBox.Load(faces);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	//nanosuit.LoadModel("objects/nanosuit/nanosuit.obj");
	lightCube.LoadModel("objects/cube/cube.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
	scene.LoadModel("objects/city13.obj");
	sus.LoadModel("objects/sus1.obj");
	jos.LoadModel("objects/jos.obj");
	heli.LoadModel("objects/heli.obj");
	rotor.LoadModel("objects/rotor1.obj");

	rain[0].LoadModel("objects/rain.obj");
	rain[1] = rain[0];
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
	depthMapShader.useShaderProgram();

	//skybox
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

}

glm::mat4 computeLightSpaceTrMatrix() {
	//TODO - Return the light-space transformation matrix
																							//glm::vec3(17.9f, 0.f, -33.94f)
	glm::mat4 lightView = glm::lookAt(glm::inverseTranspose(glm::mat3( lightRotation)) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	const GLfloat near_plane = 0.2f, far_plane = 1100.0f;
	glm::mat4 lightProjection = glm::ortho(-500.f, 500.f, -500.f, 500.0f, near_plane, far_plane);

	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void initUniforms() {

	myCustomShader.useShaderProgram();
	glm::mat4 lightSpaceTrMatrix = computeLightSpaceTrMatrix();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	withFogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "withFog");
	glUniform1i(withFogLoc, withFog);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "night"), night);

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "felinar"), felinar);
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(504.75f, 280.79f, -720.02f); // (-95.55f, -67.37f, 120.21f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");	
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir));

	light2Dir = glm::vec3(-529.43f, 14.23f, 93.86f);
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "light2Dir"), 1, glm::value_ptr(light2Dir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	light2Color = glm::vec3(0.5f, 0.5f, 0.0f); //yellow light
	light2ColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "light2Color");
	glUniform3fv(light2ColorLoc, 1, glm::value_ptr(light2Color));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceTrMatrix));



	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


	depthMapShader.useShaderProgram();
	
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	
	

	skyboxShader.useShaderProgram();
	//skybox
	view = myCamera.getViewMatrix();


	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));


	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));

	

}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	// 
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach depth texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	//nu le vrei active tot timpul
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void drawObjects(gps::Shader shader, bool depthPass) {
	
	shader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -12.0f, 32.0f));
	model = glm::scale(model, glm::vec3(1.5f * scale));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	scene.Draw(shader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-780.f, -20.f, -102.f));
	model = glm::scale(model, glm::vec3(20.0f * scale));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	jos.Draw(shader);

	if (carousel > 360.0f) {
		carousel = 0.0f;
	}

	carousel += 0.5f;
	float rotation;
	if (manual)
		rotation = angleY;
	else
		rotation = carousel;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-780.f, -20.f, -138.32f));
	model = glm::scale(model, glm::vec3(20.0f * scale));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.f, 1.f, 0.f));
	
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	sus.Draw(shader);

	if (dropY == -50.f)
		dropY = 200.f;
	dropY -= 5.f;
	if (isRaining) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, dropY, 0.0f));
		model = glm::scale(model, glm::vec3(5.f * scale));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		rain[0].Draw(shader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, dropY + 300.f, 0.0f));
		model = glm::scale(model, glm::vec3(5.f * scale));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		rain[1].Draw(shader);
	}

	heliY += 0.1f;

	model = glm::rotate(glm::mat4(1.f), glm::radians(heliY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(300.f, 240.f, 370.f));
	model = glm::scale(model, glm::vec3(2.f * scale));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	heli.Draw(shader);

	model = glm::rotate(glm::mat4(1.f), glm::radians(heliY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(300.f -2.f, 240.f, 370.f));
	model = glm::scale(model, glm::vec3(2.f * scale));
	model = glm::rotate(model, glm::radians(heliY * 5), glm::vec3(0.0f, 1.f, 0.f));
	model = glm::translate(model, glm::vec3(0.f, 0.f, 5.f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	rotor.Draw(shader);
}


void renderScene() {

	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map

	myCustomShader.useShaderProgram();
	glUniform1i(withFogLoc, withFog);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "night"), night);

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	

	
	drawObjects(depthMapShader, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//render scene

	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir));
		 
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "felinar"), felinar);
		
		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		//draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model,lightDir);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);
	}
	skyboxShader.useShaderProgram();
	mySkyBox.Draw(skyboxShader, view, projection);

	glm::vec3 points[10];
	points[0] = glm::vec3(526.35f, 295.06f, -716.01f);
	points[1] = glm::vec3(-108.46f, 29.13f, -240.27f);
	points[2] = glm::vec3(-569.78, 3.26f, -229.14f);
	points[3] = glm::vec3(-569.78, 3.26f, -229.14f);
	points[4] = glm::vec3(-722.87f, 0.f, 17.78f);
	points[5] = glm::vec3(-722.87f, 0.f, 17.78f);
	points[6] = glm::vec3(-767.32f, 0.f, 110.97f);
	points[7] = glm::vec3(-767.32f, 0.f, 110.97f);
	points[8] = glm::vec3(-767.32f, 0.f, -110.97f);

	if (t > 1.f) {
		t = 0.f;
		i++;
	}
	if (i == 8)
		animate = 0;
	if (animate) {
		myCamera.animate(i, points, t);
		if (i % 2 == 0)
			t += 0.005f;
		else {
			if (t > 0.00002f) {
				t = 0.f;
				i++;
			}else
			t += 0.000001f;
			
		}
	}

}

void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();

	

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();		

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
