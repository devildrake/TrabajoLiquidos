#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <cassert>
#include "GL_framework.h"
#include <iostream>
#include <time.h>

namespace Box {
	extern void setupCube();
	extern void cleanupCube();
	extern void drawCube();
}

namespace ClothMesh {
	extern void updateClothMesh(float* array_data);
	extern const int numCols;
	extern const int numRows;
}

namespace Sphere {
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
}

bool show_test_window = false;
bool started = false;
bool restart = false;

glm::vec3 spherePos;
glm::vec3 sphereVelocity;
glm::vec3 sphereAcceleration;



glm::vec3 dist = glm::vec3(0.58598, 0, 0.7669);
glm::vec3* arrayPos = new glm::vec3[ClothMesh::numCols*ClothMesh::numRows];
glm::vec3* arrayPosOrg = new glm::vec3[ClothMesh::numCols*ClothMesh::numRows];

glm::vec3 gravity = glm::vec3(0,-9.8,0);
glm::vec3 vectorOlaX = glm::vec3(0.6f, 0, 0);
glm::vec3 vectorOlaZ = glm::vec3(0, 0, 0.9);
glm::vec3 vectorOlaNazi = glm::vec3(0.6, 0, 0.6);


float densidad = 10; //EN REALIDAD SERIA MIl

float amplitude = 0.3f;
float frequency = 3; //50Hz
float frequency2 = 2; //50Hz
float frequency3 = 5; //50Hz
float t;

void Start() {
	srand(time(NULL));

	if (!started) {
		spherePos = glm::vec3(0, 8.7, 0);
		spherePos.x = (float)rand() / RAND_MAX * 8 - 4;
		spherePos.z = (float)rand() / RAND_MAX * 8 - 4;
		sphereVelocity = glm::vec3(0);
		sphereAcceleration = glm::vec3(0);
		Sphere::updateSphere(spherePos);

		started = true;
		for (int i = 0; i < ClothMesh::numCols; i++) {
			for (int j = 0; j < ClothMesh::numRows; j++) {
				arrayPosOrg[j * 14 + i] = (glm::vec3(j*dist.x - 5.f, 3.5, i*dist.z - 5.f));
				arrayPos[j * 14 + i] = (glm::vec3(j*dist.x - 5.f, 3.5, i*dist.z - 5.f));
			}
		}
	}

	//NO FUNCIONA POR MAGIA
	if (restart) {
		srand(time(NULL));
		sphereVelocity = glm::vec3(0);
		sphereAcceleration = glm::vec3(0);

		spherePos.x = (float)rand() / RAND_MAX * 8 - 4;
		spherePos.y = (float)rand() / RAND_MAX * 2+7;
		spherePos.z = (float)rand() / RAND_MAX * 8 - 4;
		Sphere::updateSphere(spherePos);
		t = 0;
		restart = false;
		for (int i = 0; i < ClothMesh::numCols; i++) {
			for (int j = 0; j < ClothMesh::numRows; j++) {
				arrayPos[j * 14 + i] = (glm::vec3(j*dist.x - 5.f, 3.5, i*dist.z - 5.f));
			}
		}
	}
}


class BoxObj {
public:
	//Constantes
	glm::mat3 iBody;
	float mass;

	//Variables
	glm::vec3 pos;
	glm::mat3 rotation;
	glm::vec3 linealMom;
	glm::vec3 angMom;
	int size;
};


void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Button("Pointless Button (Beware, pointless)");
		restart = ImGui::Button("Restart");
		//TODO
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);

	}
}

void PhysicsInit() {
	//TODO
}

float counter = 0;

//CONTADOR DE TIEMPO

void UpdateSpherePos(float dt,glm::vec3 b) {
	sphereAcceleration += gravity + b;
	sphereVelocity += sphereAcceleration*dt;
	spherePos += sphereVelocity*dt;
}

void bajarArray(float* distancias, int* indices,int a) {
	for (int i = 3; i > a; i--) {
		distancias[i] = distancias[i-1];
		indices[i] = indices[i-1];
	}

}

void PhysicsUpdate(float dt) {
	//void updateClothMesh(float* array_data);

	t += dt;

	Start();

	for (int i = 0; i < ClothMesh::numCols*ClothMesh::numRows; i++) {

		glm::vec2 orgX = glm::vec2(arrayPosOrg[i].x, arrayPosOrg[i].z);
		glm::vec2 currentX;
		currentX = orgX - (glm::normalize(glm::vec2(vectorOlaX.x, vectorOlaX.z)) * amplitude * glm::sin(glm::dot(glm::vec2(vectorOlaX.x, vectorOlaX.z), orgX) - frequency * t)
			+ glm::normalize(glm::vec2(vectorOlaZ.x, vectorOlaZ.z)) * amplitude * glm::sin(glm::dot(glm::vec2(vectorOlaZ.x, vectorOlaZ.z), orgX) - frequency2 * t)
			+ glm::normalize(glm::vec2(vectorOlaNazi.x, vectorOlaNazi.z)) * amplitude * glm::sin(glm::dot(glm::vec2(vectorOlaNazi.x, vectorOlaNazi.z), orgX) - frequency3 * t)
			);

		float currentY = 5 + (amplitude * glm::cos(glm::dot(glm::vec2(vectorOlaX.x, vectorOlaX.z), orgX) - frequency * t) +
			amplitude * glm::cos(glm::dot(glm::vec2(vectorOlaZ.x, vectorOlaZ.z), orgX) - frequency2 * t)/* + otras olas*/ +
			amplitude * glm::cos(glm::dot(glm::vec2(vectorOlaNazi.x, vectorOlaNazi.z), orgX) - frequency3 * t)
			);

		arrayPos[i] = glm::vec3(currentX.x, currentY, currentX.y);

	}

	//Inicializamos los indices a -1
	int closestVertexIndex[4] = { -1, -1, -1, -1 };
	float closestDistances[4] = { 20,20,20,20 };

	for (int i = 0; i < ClothMesh::numCols*ClothMesh::numRows; i++) {

		float distanciaActual = glm::distance(arrayPos[i], spherePos) - 1;

		if (distanciaActual < (closestDistances[0])) {

			bajarArray(closestDistances, closestVertexIndex, 0);
			closestDistances[0] = distanciaActual;
			closestVertexIndex[0] = i;

		}
		else if ((distanciaActual < (closestDistances[1]))) {

			bajarArray(closestDistances, closestVertexIndex, 1);
			closestDistances[1] = distanciaActual;
			closestVertexIndex[1] = i;
		}

		else if (distanciaActual < (closestDistances[2])) {

			bajarArray(closestDistances, closestVertexIndex, 2);
			closestDistances[2] = distanciaActual;
			closestVertexIndex[2] = i;
		}

		else if (distanciaActual < (closestDistances[2])) {
			closestDistances[3] = distanciaActual;
			closestVertexIndex[3] = i;
		}
	
	}
	
	glm::vec3 closestVertex[4];

	glm::vec3 buoyancy = glm::vec3(0,0,0);

	float alturaSumergida = 0;
	if (spherePos.y+1 < arrayPos[closestVertexIndex[0]].y) {
		 alturaSumergida = glm::abs(spherePos.y + 1 - 5);
		 float volumenSumergido = ((alturaSumergida * alturaSumergida *3.1416f) / 3) * (3 - alturaSumergida);
		 buoyancy = densidad * gravity * volumenSumergido * glm::vec3(0, 1, 0);

	}




UpdateSpherePos(dt,buoyancy);

		ClothMesh::updateClothMesh((float*)&arrayPos[0].x);
		Sphere::updateSphere(spherePos);

		//TODO
	
}

void PhysicsCleanup() {
	//TODO
}