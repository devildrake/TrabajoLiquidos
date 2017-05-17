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
#include <algorithm>

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
float sphereMass = 5;


glm::vec3 dist = glm::vec3(0.58598, 0, 0.7669);
glm::vec3* arrayPos = new glm::vec3[ClothMesh::numCols*ClothMesh::numRows];
glm::vec3* arrayPosOrg = new glm::vec3[ClothMesh::numCols*ClothMesh::numRows];

float multiplicadorOla1, multiplicadorOla2, multiplicadorOla3;

glm::vec3 gravity = glm::vec3(0,-9.8,0);
glm::vec3 vectorOlaX = glm::vec3(0.6f, 0, 0);
glm::vec3 vectorOlaZ = glm::vec3(0, 0, 0.9);
glm::vec3 vectorOlaNazi = glm::vec3(0.6, 0, 0.6);
glm::vec3 colPos;

float densidad = 10; //EN REALIDAD SERIA MIl

float amplitude = 0.3f;
float frequency = 3; //50Hz
float frequency2 = 2; //50Hz
float frequency3 = 5; //50Hz
float t;
bool pause;
void Start() {
	srand(time(NULL));

	if (!started) {
		sphereMass = 5;
		multiplicadorOla1 = multiplicadorOla2 = multiplicadorOla3 = 1;
		 amplitude = 0.3f;
		 frequency = 3; //50Hz
		 frequency2 = 2; //50Hz
		 frequency3 = 5; //50Hz
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
	//ahora ya si
	if (restart) {
		sphereMass = 5;
		 multiplicadorOla1 = multiplicadorOla2 = multiplicadorOla3 = 1;
		  amplitude = 0.3f;
		  frequency = 3; //50Hz
		  frequency2 = 2; //50Hz
		  frequency3 = 5; //50Hz
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
		ImGui::DragFloat("Masa Esferas",&sphereMass,0.1f,0.1f,45.0f);
		ImGui::DragFloat("Ola 1", &multiplicadorOla1,0.1f,0.1,5.0f);
		ImGui::DragFloat("Ola 2", &multiplicadorOla2, 0.1f, 0.1, 5.0f);
		ImGui::DragFloat("Ola 3", &multiplicadorOla3, 0.1f, 0.1, 5.0f);
		ImGui::DragFloat("Freq 1", &frequency, 0.1f, 1, 5.0f);
		ImGui::DragFloat("Freq 2", &frequency2, 0.1f, 1, 5.0f);
		ImGui::DragFloat("Freq 3", &frequency3, 0.1f, 1, 5.0f);
		ImGui::DragFloat("Amplitud", &amplitude, 0.05f, 0.05f, 0.4f);
		ImGui::Checkbox("Pause",&pause);
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

void UpdateSpherePos(float dt,glm::vec3 b, glm::vec3 drag) {
	//glm::vec3 aBuo = b / sphereMass;
	//glm::vec3 aDrag = drag / sphereMass;
	glm::vec3 fGrav = gravity * sphereMass;
	glm::vec3 sumF = fGrav + b + drag;


	sphereAcceleration = sumF/sphereMass;
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

	if (!pause) {
		//void updateClothMesh(float* array_data);

		vectorOlaX = glm::vec3(0.6f, 0, 0) * multiplicadorOla1;
		vectorOlaZ = glm::vec3(0, 0, 0.9) * multiplicadorOla2;
		vectorOlaNazi = glm::vec3(0.6, 0, 0.6) * multiplicadorOla3;



		t += dt;

		Start();

		//movimiento de las olas
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



		glm::vec3 buoyancy = glm::vec3(0, 0, 0);
		glm::vec3 fDrag = glm::vec3(0, 0, 0);

		float alturaSumergida = 0;


		//AQUI YACE EL INTENTO FALLIDO DE INTERPOLACION

		//std::sort(closestVertexIndex,closestVertexIndex+sizeof(closestVertexIndex)/sizeof(int));

		//float distanceToSphere = spherePos.x-arrayPos[closestVertexIndex[0]].x;
		//float distanceBetweenP = arrayPos[closestVertexIndex[0]].x - arrayPos[closestVertexIndex[1]].x;
		//float alpha = glm::abs(distanceToSphere / distanceBetweenP);


		//distanceToSphere = spherePos.x - arrayPos[closestVertexIndex[2]].x;
		//distanceBetweenP = arrayPos[closestVertexIndex[2]].x - arrayPos[closestVertexIndex[3]].x;
		//float beta = glm::abs(distanceToSphere / distanceBetweenP);

		//float puntoVirtual1 = glm::mix(arrayPos[closestVertexIndex[0]].y,arrayPos[closestVertexIndex[1]].y,alpha);
		//float puntoVirtual2 = glm::mix(arrayPos[closestVertexIndex[2]].y, arrayPos[closestVertexIndex[3]].y, beta);
		//
		//distanceToSphere = spherePos.z - arrayPos[closestVertexIndex[1]].z;
		//distanceBetweenP = arrayPos[closestVertexIndex[1]].z - arrayPos[closestVertexIndex[3]].z;
		//
		//alpha = glm::abs(distanceToSphere / distanceBetweenP);

		//float puntoVirtualDef = glm::mix(puntoVirtual2,puntoVirtual1, alpha);

		float alturaApprox = (arrayPos[closestVertexIndex[0]].y + arrayPos[closestVertexIndex[1]].y + arrayPos[closestVertexIndex[2]].y + arrayPos[closestVertexIndex[3]].y) / 4;

		if (spherePos.y - 1 < alturaApprox) {
			alturaSumergida = glm::clamp(glm::abs(spherePos.y - 1 - alturaApprox), 0.f, 2.f);
			float volumenSumergido = ((alturaSumergida * alturaSumergida *3.1416f) / 3) * (3 - alturaSumergida);
			buoyancy = densidad * glm::length(gravity) * glm::abs(volumenSumergido) * glm::vec3(0, 1, 0);

			//fuerza de drag
			float coefDrag = 1.f;
			float crossSectionalArea;
			alturaSumergida = glm::clamp(alturaSumergida, 0.0f, 1.0f);

			crossSectionalArea = 3.1416 * glm::pow(((alturaSumergida)), 2);

			fDrag = (densidad * coefDrag * crossSectionalArea * glm::length(sphereVelocity) * sphereVelocity);
			fDrag = fDrag * (-1.f / 2.f);
		}




		UpdateSpherePos(dt, buoyancy, fDrag);

		ClothMesh::updateClothMesh((float*)&arrayPos[0].x);
		Sphere::updateSphere(spherePos);

		//TODO

	}
}
void PhysicsCleanup() {
	//TODO
}