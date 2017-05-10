#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <cassert>
#include "GL_framework.h"
#include <iostream>

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

bool show_test_window = false;
bool started = false;
bool restart = false;

glm::vec3 dist = glm::vec3(0.58598, 0, 0.7669);
glm::vec3* arrayPos = new glm::vec3[ClothMesh::numCols*ClothMesh::numRows];
glm::vec3* arrayPosOrg = new glm::vec3[ClothMesh::numCols*ClothMesh::numRows];

glm::vec3 gravity = glm::vec3(0,-9.8,0);

glm::vec3 vectorOlaX = glm::vec3(0.5, 0, 0);
glm::vec3 vectorOlaZ = glm::vec3(0, 0, 0.5);
float amplitude = 0.5f;
float frequency = 3; //50Hz

void Start() {
	if (!started) {
		started = true;
		for (int i = 0; i < ClothMesh::numCols; i++) {
			for (int j = 0; j < ClothMesh::numRows; j++) {
				arrayPosOrg[j * 14 + i] = (glm::vec3(j*dist.x - 5.f, 3.5, i*dist.z - 5.f));
			}
		}
			arrayPos = arrayPosOrg;
	}

	//NO FUNCIONA POR MAGIA
	if (restart) {
		restart = false;
			arrayPos = arrayPosOrg;
	
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
float t;

void PhysicsUpdate(float dt) {
	//void updateClothMesh(float* array_data);

	t += dt;

		Start();

		for (int i = 0; i < ClothMesh::numCols*ClothMesh::numRows;i++) {
			glm::vec2 orgX = glm::vec2(arrayPosOrg[i].x, arrayPosOrg[i].z);
			glm::vec2 currentX;
			currentX = orgX - glm::normalize(glm::vec2(vectorOlaX.x,vectorOlaX.z)) * amplitude * glm::sin(glm::dot(glm::vec2(vectorOlaX.x,vectorOlaX.z),orgX)- frequency * dt);
			float currentY = amplitude * glm::cos(glm::dot(glm::vec2(vectorOlaX.x, vectorOlaX.z), orgX) - frequency * dt);
			arrayPos[i] = glm::vec3(currentX.x,currentY,currentX.y);
		}





		ClothMesh::updateClothMesh((float*)&arrayPos[0].x);

	
	//TODO
}
void PhysicsCleanup() {
	//TODO
}