#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "ShaderProgram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using namespace std;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseMotionCallback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void render(double time, ShaderProgram& shaderProgram, ShaderProgram& axisShaderProgram, ShaderProgram& lightShaderProgram);
void consoleCallback();
void changeLight(float amount);
GLuint createSphere(float radius, int stacks, int slices);
GLuint createAxis();
GLuint createLight();

GLuint sphereVAO;
GLuint axisVAO;
GLuint lightVAO;

bool leftMouseButtonPressed = false;
float mouseXPosLast = 0.0f, mouseYPosLast = 0.0f;
float phi = 0.0f, theta = 0.0f;
float pix2angle = 0.5f;
bool firstMouse = true;
float R = 10.f;
float zoom = 45.0f;

int selectedLight = 0;
int previousSelectedLight = 0;
int selectedColor = 0;
int previousSelectedColor = 0;
int selectedPosition = 0;
int previousSelectedPosition = 0;

bool upPressed = false, downPressed = false;

vector<int> indices;
vector<int> lightsIndices;

struct Light {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

vector<Light> lights;

int main() {

	if (!glfwInit()) {
		cout << "Failed to initialize GLFW" << endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Test OpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGL(glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mouseMotionCallback);
	glfwSetScrollCallback(window, scroll_callback);

	ShaderProgram shaderProgram("vertexShader.glsl", "fragmentShader.glsl");
	ShaderProgram axisShaderProgram("axisVtShader.glsl", "axisfrShader.glsl");
	ShaderProgram lightShaderProgram("lightVtShader.glsl", "lightfrShader.glsl");

	sphereVAO = createSphere(4.f, 50, 50);
	axisVAO = createAxis();
	lightVAO = createLight();

	//Create lights

	Light light1;
	light1.position = glm::vec3(3.0f, 0.0f, 0.0f);
	light1.ambient = glm::vec3(0.05f, 0.3f, 0.05f);
	light1.diffuse = glm::vec3(0.05f, 0.3f, 0.05f);
	light1.specular = glm::vec3(0.05f, 0.3f, 0.05f);
	light1.constant = 1.0f;
	light1.linear = 0.09f;
	light1.quadratic = 0.032f;
	lights.emplace_back(light1);

	Light light2;
	light2.position = glm::vec3(-3.0f, 3.0f, 3.0f);
	light2.ambient = glm::vec3(0.4f, 0.05f, 0.05f);
	light2.diffuse = glm::vec3(0.4f, 0.05f, 0.05f);
	light2.specular = glm::vec3(0.4f, 0.05f, 0.05f);
	light2.constant = 1.0f;
	light2.linear = 0.09f;
	light2.quadratic = 0.032f;
	lights.emplace_back(light2);

	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		consoleCallback();
		render(glfwGetTime(), shaderProgram, axisShaderProgram, lightShaderProgram);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &sphereVAO);

	glfwTerminate();
	return 0;
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
		selectedLight = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		selectedLight = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		selectedLight = 2;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && selectedLight != 0) {
		selectedColor = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && selectedLight != 0) {
		selectedColor = 2;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && selectedLight != 0) {
		selectedColor = 3;
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && selectedLight != 0) {
		selectedPosition = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && selectedLight != 0) {
		selectedPosition = 2;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && selectedLight != 0) {
		selectedPosition = 3;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (selectedLight != 0 && (selectedColor != 0 || selectedPosition != 0)) {
			changeLight(0.05f);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (selectedLight != 0 && (selectedColor != 0 || selectedPosition != 0)) {
			changeLight(-0.05f);
		}
	}
}

void render(double time, ShaderProgram& shaderProgram, ShaderProgram& axisShaderProgram, ShaderProgram& lightShaderProgram) {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderProgram.use();

	glm::mat4 model = glm::mat4(1.0f);

	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraPosition.x = R * cos(glm::radians(theta)) * cos(glm::radians(phi));
	cameraPosition.y = R * sin(glm::radians(phi));
	cameraPosition.z = R * sin(glm::radians(theta)) * cos(glm::radians(phi));

	glm::vec3 upVector;

	if (cos(glm::radians(phi)) >= 0.0f)
		upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	else
		upVector = glm::vec3(0.0f, -1.0f, 0.0f);

	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), upVector);
	projection = glm::perspective(glm::radians(zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	GLuint viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	GLuint projectionLoc = glGetUniformLocation(shaderProgram.ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	GLuint modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	GLuint cameraLoc = glGetUniformLocation(shaderProgram.ID, "viewPos");
	glUniform3f(cameraLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Light properties
	shaderProgram.setVec3("light[0].position", lights[0].position);
	shaderProgram.setVec3("light[0].ambient", lights[0].ambient);
	shaderProgram.setVec3("light[0].diffuse", lights[0].diffuse);
	shaderProgram.setVec3("light[0].specular", lights[0].specular);
	shaderProgram.setFloat("light[0].constant", lights[0].constant);
	shaderProgram.setFloat("light[0].linear", lights[0].linear);
	shaderProgram.setFloat("light[0].quadratic", lights[0].quadratic);

	shaderProgram.setVec3("light[1].position", lights[1].position);
	shaderProgram.setVec3("light[1].ambient", lights[1].ambient);
	shaderProgram.setVec3("light[1].diffuse", lights[1].diffuse);
	shaderProgram.setVec3("light[1].specular", lights[1].specular);
	shaderProgram.setFloat("light[1].constant", lights[1].constant);
	shaderProgram.setFloat("light[1].linear", lights[1].linear);
	shaderProgram.setFloat("light[1].quadratic", lights[1].quadratic);

	// Sphere
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Axis
	axisShaderProgram.use();
	GLuint viewLocAxis = glGetUniformLocation(axisShaderProgram.ID, "view");
	glUniformMatrix4fv(viewLocAxis, 1, GL_FALSE, glm::value_ptr(view));
	GLuint projectionLocAxis = glGetUniformLocation(axisShaderProgram.ID, "projection");
	glUniformMatrix4fv(projectionLocAxis, 1, GL_FALSE, glm::value_ptr(projection));
	GLuint modelLocAxis = glGetUniformLocation(axisShaderProgram.ID, "model");
	glUniformMatrix4fv(modelLocAxis, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);

	//Lights

	lightShaderProgram.use();
	GLuint viewLocLight = glGetUniformLocation(lightShaderProgram.ID, "view");
	glUniformMatrix4fv(viewLocLight, 1, GL_FALSE, glm::value_ptr(view));
	GLuint projectionLocLight = glGetUniformLocation(lightShaderProgram.ID, "projection");
	glUniformMatrix4fv(projectionLocLight, 1, GL_FALSE, glm::value_ptr(projection));
	GLuint modelLocLight = glGetUniformLocation(lightShaderProgram.ID, "model");

	for (int i = 0; i < lights.size(); i++) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, lights[i].position);
		model = glm::scale(model, glm::vec3(0.1f));
		glUniformMatrix4fv(modelLocLight, 1, GL_FALSE, glm::value_ptr(model));

		lightShaderProgram.setVec3("aColor", lights[i].ambient);

		glBindVertexArray(lightVAO);
		glDrawElements(GL_TRIANGLES, (unsigned int)lightsIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	glUseProgram(0);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		leftMouseButtonPressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		leftMouseButtonPressed = false;
		firstMouse = true;
	}
}

void mouseMotionCallback(GLFWwindow* window, double xpos, double ypos) {
	if (leftMouseButtonPressed) {
		if (firstMouse)
		{
			mouseXPosLast = xpos;
			mouseYPosLast = ypos;
			firstMouse = false;
		}

		float deltaX = xpos - mouseXPosLast;
		mouseXPosLast = xpos;
		theta += deltaX * pix2angle;

		float deltaY = ypos - mouseYPosLast;
		mouseYPosLast = ypos;
		phi += deltaY * pix2angle;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (zoom >= 10.0f && zoom <= 80.0f)
		zoom -= yoffset;
	if (zoom <= 10.0f)
		zoom = 10.0f;
	if (zoom >= 80.0f)
		zoom = 80.0f;
}

GLuint createSphere(float radius, int stacks, int slices) {
	std::vector<float> vertices;
	std::vector<float> normals;

	float x, y, z, xy;
	float nx, ny, nz, lengthInv = 1.0f / radius;

	float sectorStep = 2 * 3.141592 / slices;
	float stackStep = 3.141592 / stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; ++i)
	{
		stackAngle = 3.141592 / 2 - i * stackStep;
		xy = radius * cosf(stackAngle);
		z = radius * sinf(stackAngle);

		for (int j = 0; j <= slices; ++j)
		{
			sectorAngle = j * sectorStep;

			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			normals.push_back(nx);
			normals.push_back(ny);
			normals.push_back(nz);
		}
	}

	int k1, k2;
	for (int i = 0; i < stacks; ++i)
	{
		k1 = i * (slices + 1);
		k2 = k1 + slices + 1;

		for (int j = 0; j < slices; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (stacks - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	GLuint vao, vbo1, vbo2, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo1);
	glGenBuffers(1, &vbo2);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo1);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
}

GLuint createAxis() {
	float length = 20.0f;

	float vertices[] = {
		-length, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		length, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		0.0f, -length, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, length, 0.0f, 0.0f, 1.0f, 0.0f,

		0.0f, 0.0f, -length, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, length , 0.0f, 0.0f, 1.0f
	};

	GLuint vao, vbo;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
}

GLuint createLight() {

	int light_amount = 2;
	int light_vertices = 20;
	float radius = 2.0f;

	vector<float> vertices;

	float x, y, z, xy;

	float sectorStep = 2 * 3.141592 / light_vertices;
	float stackStep = 3.141592 / light_vertices;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= light_vertices; ++i)
	{
		stackAngle = 3.141592 / 2 - i * stackStep;
		xy = radius * cosf(stackAngle);
		z = radius * sinf(stackAngle);
		for (int j = 0; j <= light_vertices; ++j)
		{
			sectorAngle = j * sectorStep;
			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	int k1, k2;
	for (int i = 0; i < light_vertices; ++i)
	{
		k1 = i * (light_vertices + 1);
		k2 = k1 + light_vertices + 1;

		for (int j = 0; j < light_vertices; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				lightsIndices.push_back(k1);
				lightsIndices.push_back(k2);
				lightsIndices.push_back(k1 + 1);
			}

			if (i != (light_vertices - 1))
			{
				lightsIndices.push_back(k1 + 1);
				lightsIndices.push_back(k2);
				lightsIndices.push_back(k2 + 1);
			}
		}
	}

	GLuint vao, vbo1, ebo;
	glGenVertexArrays(1, &vao);

	glGenBuffers(1, &vbo1);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo1);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lightsIndices.size() * sizeof(unsigned int), lightsIndices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
}

void consoleCallback() {
	if (selectedLight != previousSelectedLight) {
		system("cls");
		if (selectedLight == 0)
			cout << "Wybrane swiatlo: Brak" << endl;
		else
			cout << "Wybrane swiatlo: " << selectedLight << endl;
		previousSelectedLight = selectedLight;
	}

	if (selectedColor != previousSelectedColor && selectedLight != 0) {
		selectedPosition = 0;
		system("cls");
		if (selectedColor == 1) {
			cout << "Wybrane swiatlo: " << selectedLight << endl;
			cout << "Wybrany kolor: Czerwony" << endl;
		}
		else if (selectedColor == 2) {
			cout << "Wybrane swiatlo: " << selectedLight << endl;
			cout << "Wybrany kolor: Zielony" << endl;
		}
		else if (selectedColor == 3) {
			cout << "Wybrane swiatlo: " << selectedLight << endl;
			cout << "Wybrany kolor: Niebieski" << endl;
		}
		previousSelectedColor = selectedColor;
	}

	if (selectedPosition != previousSelectedPosition && selectedLight != 0) {
		selectedColor = 0;
		system("cls");
		if (selectedPosition == 1) {
			cout << "Wybrane swiatlo: " << selectedLight << endl;
			cout << "Poruszanie obiektem w osi: X" << endl;
		}
		else if (selectedPosition == 2) {
			cout << "Wybrane swiatlo: " << selectedLight << endl;
			cout << "Poruszanie obiektem w osi: Y" << endl;
		}
		else if (selectedPosition == 3) {
			cout << "Wybrane swiatlo: " << selectedLight << endl;
			cout << "Poruszanie obiektem w osi: Z" << endl;
		}
		previousSelectedPosition = selectedPosition;
	}
}

void changeLight(float amount) {
	system("cls");
	cout << "Wybrane swiatlo: " << selectedLight << endl;

	if (selectedColor == 1) {
		cout << "Wybrany kolor: Czerwony" << endl;
		float r = lights[selectedLight - 1].ambient.r;
		r += amount;

		if (r > 1.0f)
			r = 1.0f;
		else if (r < 0.01f)
			r = 0.0f;

		lights[selectedLight - 1].ambient.r = r;
		lights[selectedLight - 1].specular.r = r;
		lights[selectedLight - 1].diffuse.r = r;
		cout << "R: " << lights[selectedLight - 1].ambient.r << " G: " << lights[selectedLight - 1].ambient.g << " B: " << lights[selectedLight - 1].ambient.b << endl;
	}
	else if (selectedColor == 2) {
		cout << "Wybrany kolor: Zielony" << endl;
		float g = lights[selectedLight - 1].ambient.g;
		g += amount;

		if (g > 1.0f)
			g = 1.0f;
		else if (g < 0.01f)
			g = 0.0f;

		lights[selectedLight - 1].ambient.g = g;
		lights[selectedLight - 1].specular.g = g;
		lights[selectedLight - 1].diffuse.g = g;
		cout << "R: " << lights[selectedLight - 1].ambient.r << " G: " << lights[selectedLight - 1].ambient.g << " B: " << lights[selectedLight - 1].ambient.b << endl;
	}
	else if (selectedColor == 3) {
		cout << "Wybrany kolor: Niebieski" << endl;
		float b = lights[selectedLight - 1].ambient.b;
		b += amount;

		if (b > 1.0f)
			b = 1.0f;
		else if (b < 0.01f)
			b = 0.0f;

		lights[selectedLight - 1].ambient.b = b;
		lights[selectedLight - 1].specular.b = b;
		lights[selectedLight - 1].diffuse.b = b;
		cout << "R: " << lights[selectedLight - 1].ambient.r << " G: " << lights[selectedLight - 1].ambient.g << " B: " << lights[selectedLight - 1].ambient.b << endl;
	}

	if (selectedPosition == 1) {
		cout << "Poruszanie obiektem w osi: X" << endl;
		float x = lights[selectedLight - 1].position.x;
		x += amount;
		lights[selectedLight - 1].position.x = x;
		cout << "X: " << lights[selectedLight - 1].position.x << " Y: " << lights[selectedLight - 1].position.y << " Z: " << lights[selectedLight - 1].position.z << endl;
	}
	else if (selectedPosition == 2) {
		cout << "Poruszanie obiektem w osi: Y" << endl;
		float y = lights[selectedLight - 1].position.y;
		y += amount;
		lights[selectedLight - 1].position.y = y;
		cout << "X: " << lights[selectedLight - 1].position.x << " Y: " << lights[selectedLight - 1].position.y << " Z: " << lights[selectedLight - 1].position.z << endl;
	}
	else if (selectedPosition == 3) {
		cout << "Poruszanie obiektem w osi: Z" << endl;
		float z = lights[selectedLight - 1].position.z;
		z += amount;
		lights[selectedLight - 1].position.z = z;
		cout << "X: " << lights[selectedLight - 1].position.x << " Y: " << lights[selectedLight - 1].position.y << " Z: " << lights[selectedLight - 1].position.z << endl;
	}
}