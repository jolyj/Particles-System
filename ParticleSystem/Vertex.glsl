#version 330 core

layout(location = 0) in vec4 col;
layout(location = 1) in vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
out vec4 fragmentColor;

void main(){

	gl_Position = projectionMatrix * viewMatrix * vec4(position,1.0f);
	fragmentColor = col;
}
