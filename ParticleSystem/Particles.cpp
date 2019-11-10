#include "Particles.h"

Particles::Particles(GLFWwindow* w): window(w) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	srand(time(NULL));
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	programID = LoadShaders("Vertex.glsl", "Fragment.glsl");

	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer), color_buffer, GL_STATIC_DRAW);

	glGenBuffers(1, &positionbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position_buffer), position_buffer, GL_STATIC_DRAW);

	ProjMatrixID = glGetUniformLocation(programID, "projectionMatrix");
	ViewMatrixID = glGetUniformLocation(programID, "viewMatrix");
	createParticles();

	g1.force = 30.0f;
	g1.pos = glm::vec3(0, 0, 0);
	g1.acceleration = 3;
	g1.velocity = glm::vec3(randomFloat(-5, 5), randomFloat(-5, 5), 0);
}

Particles::~Particles() {
	glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &positionbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);
}

void Particles::start() {
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0) {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		simulateParticles();

		// g1 follow mouse position
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		g1.pos.x = (xpos - 960) * WIDTH * -1 / 960.0f;
		g1.pos.y = (ypos - 540) * HEIGHT * -1 / 540.0f;


		// Use our vertex
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, positionbuffer);
		glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_PARTICLES * sizeof(GLfloat) * 3, position_buffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_PARTICLES * sizeof(GLfloat) * 4, color_buffer);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Compute projection + view matrices
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		// Reinitialize particles if F1 is pressed
		if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
			this->createParticles();
		}

		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0);

		glPointSize(2);
		glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);

		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
}

void Particles::simulateParticles() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		 
		Particle& p = ParticlesContainer[i];
		
		// Keep the particles in the area
		if (p.pos.x <= -10 || p.pos.x >= WIDTH)
			p.velocity.x *= -1;
		if (p.pos.y <= -10 || p.pos.y >= HEIGHT)
			p.velocity.y *= -1;

		// Compute the force due to the point g1
		float dist = (g1.pos.x - p.pos.x) * (g1.pos.x - p.pos.x) + (g1.pos.y - p.pos.y) * (g1.pos.y - p.pos.y);
		p.acceleration = g1.force / (dist * 8);

		// Deceleration
		if (p.acceleration > 0)
			p.acceleration -= 0.001f * 1000;

		p.pos += (p.velocity * p.acceleration) * 0.01f;

		// Position update
		position_buffer[3 * i + 0] = p.pos.x;
		position_buffer[3 * i + 1] = p.pos.y;

		color_buffer[4 * i + 0] = p.color.x;
		color_buffer[4 * i + 1] = p.color.y;
		color_buffer[4 * i + 2] = p.color.z;
		color_buffer[4 * i + 3] = 0.5f;
	}
}

float Particles::randomFloat(float a, float b) {
	// Generate random float between a and b
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void Particles::createParticles() {
	// Create the particles
	for (int i = 0; i < MAX_PARTICLES; i++) {
		ParticlesContainer[i].pos = glm::vec3(randomFloat(HEIGHT * -1, HEIGHT), randomFloat(WIDTH * -1, WIDTH), 0);
		ParticlesContainer[i].velocity = glm::vec3(randomFloat(-5, 5), randomFloat(-5, 5), 0);
		ParticlesContainer[i].acceleration = 0;
		ParticlesContainer[i].color.x = 1.0f;
		ParticlesContainer[i].color.y = 137 / 255.0f;
		ParticlesContainer[i].color.z = 0.34f;
		ParticlesContainer[i].color.w = 1.0f;
	}
}