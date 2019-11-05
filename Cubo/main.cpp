#include <Windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <vector>

#include "Headers/Includes.h"
#include "Headers/Mesh.h"
#include "Headers/Material.h"
#include "Headers/Obj3D.h"
#include "Headers/Shader.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void processInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
void processInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale, int idObjeto);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
vector<Obj3D*> listaDeObjs;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//camera
float camSpeed = 0.01f;
float dirSpeed = 0.01f;

bool firtMouse = true;

float pitchAngle = -30.f;
float yawAngle = -90.f;
glm::vec3 camDirection = glm::vec3(
	glm::normalize(glm::vec3(
		cos(glm::radians(pitchAngle)) * cos(glm::radians(yawAngle)),
		sin(glm::radians(pitchAngle)),
		cos(glm::radians(pitchAngle)) * sin(glm::radians(yawAngle))
	))
);

glm::vec3 camPosition(0.0f, 1.0f, 3.0f);
glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
glm::vec3 camFront = glm::normalize(glm::cross(camDirection, worldUp));

glm::mat4 ViewMatrix = glm::lookAt(camPosition, camPosition + camDirection, worldUp);

int selecionado = 1;

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Computacao Grafica - GA", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//mouse
	glfwSetCursorPosCallback(window, mouse_callback);

	glewExperimental = GL_TRUE;
	glewInit();

	int framebufferWidth = 0;
	int framebufferHeight = 0;

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//read shaders
	GLuint shaderProgram = LoadShader("Shaders/Core/core.vert", "Shaders/Core/core.frag");
	glUseProgram(shaderProgram);

	float fov = 90.f;
	float nearPlane = 0.1f;
	float farPlane = 1000.f;
	glm::mat4 ProjectionMatrix(1.f);

	ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);

	glm::mat4 ModelMatrix(1.f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	Obj3D* obj3d = new Obj3D();
	obj3d->Inicializar();
	obj3d->position.x = 5.0f;
	Mesh* mesh = obj3d->processObj("mesa01.obj");
	obj3d->mesh = mesh;
	vector<Material*> materials = obj3d->getMat();
	obj3d->materials = materials;
	listaDeObjs.push_back(obj3d);

	Obj3D* obj3d2 = new Obj3D();
	obj3d2->Inicializar();
	Mesh* mesh2 = obj3d2->processObj("mesa01.obj");
	obj3d2->mesh = mesh2;
	vector<Material*> materials2 = obj3d2->getMat();
	obj3d2->materials = materials2;
	listaDeObjs.push_back(obj3d2);

	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);

	while (!glfwWindowShouldClose(window))
	{

		processInput(window);
		processInput(window, listaDeObjs[selecionado - 1]->position, listaDeObjs[selecionado - 1]->rotation, listaDeObjs[selecionado - 1]->scale, selecionado);

		int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glm::vec3 camFrontCalc;
		camFrontCalc.x = cos(glm::radians(pitchAngle)) * cos(glm::radians(yawAngle));
		camFrontCalc.y = sin(glm::radians(pitchAngle));
		camFrontCalc.z = cos(glm::radians(pitchAngle)) * sin(glm::radians(yawAngle));
		camDirection = glm::normalize(camFrontCalc);

		ViewMatrix = glm::lookAt(camPosition, camPosition + camDirection, worldUp);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));

		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

		ProjectionMatrix = glm::mat4(1.f);
		ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

		for (int i = 0; i < listaDeObjs.size(); i++)
		{
			listaDeObjs[i]->transform();
			GLuint texture;
			for (Group* g : listaDeObjs[i]->mesh->groups) {
				for (Material* m : listaDeObjs[i]->materials) {
					texture = m->texture;
					glUniform3f(glGetUniformLocation(shaderProgram, "Ka"), m->ka->r, m->ka->g, m->ka->b);
					glUniform3f(glGetUniformLocation(shaderProgram, "Kd"), m->kd->r, m->kd->g, m->kd->b);
					glUniform3f(glGetUniformLocation(shaderProgram, "Ks"), m->ks->r, m->ks->g, m->ks->b);
					glUniform1f(glGetUniformLocation(shaderProgram, "Ns"), m->ns);
				}

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glUniform1i(glGetUniformLocation(shaderProgram, "texture"), 0);

				glBindVertexArray(g->vao);

				glUniform1i((glGetUniformLocation(shaderProgram, "selecionado")), selecionado == i+1);
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(listaDeObjs[i]->ModelMatrix));

				glDrawArrays(GL_TRIANGLES, 0, g->faces.size() * 3);
			}
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	int pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	if (pressed == 1) {
		cout << "mouse pressionado";
	}
	else {

	}
}

void processInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale, int idObjeto)
{
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		selecionado = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		selecionado = 2;
	}

	if (selecionado == idObjeto)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			position.y += 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			position.y += 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			position.x -= 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			position.y -= 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			position.x += 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			rotation.y -= 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			rotation.y += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			rotation.x -= 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
			rotation.x += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
			scale += 0.001f;
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			scale -= 0.001f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		camPosition += camSpeed * camDirection;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		camPosition -= camSpeed * camDirection;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		yawAngle -= dirSpeed;
		camFront = cross(camDirection, worldUp);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		yawAngle += dirSpeed;
		camFront = cross(camDirection, worldUp);
	}

}