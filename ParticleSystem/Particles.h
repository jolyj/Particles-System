#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <memory>
#include <time.h>
#include "Shader.h"
#include "Control.h"
#include <iostream>

#define HEIGHT 10
#define WIDTH 10
#define MAX_PARTICLES 300000

struct Particle {
	glm::vec3 pos;
	glm::vec4 color;
	glm::vec3 velocity;
	float acceleration;
};

struct AttractionPoint {
	glm::vec3 pos;
	float force;
	glm::vec3 velocity;
	float acceleration;
};

class Particles {
public:
	Particles(GLFWwindow* window);
	~Particles();
	void start();

private:
	GLFWwindow* window;
	GLuint colorbuffer;
	GLuint positionbuffer;
	GLuint programID;
	GLuint VertexArrayID;

	GLfloat color_buffer[MAX_PARTICLES * 4];
	GLfloat position_buffer[MAX_PARTICLES * 3];

	GLuint ViewMatrixID;
	GLuint ProjMatrixID;

	Particle ParticlesContainer[MAX_PARTICLES];
	void createParticles();
	void simulateParticles();
	float randomFloat(float, float);

	AttractionPoint g1;
};