#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

// Matriz de proyección combinada enviada por la clase Camera.h
uniform mat4 camMatrix; 
// Matriz de transformaciones del objeto (Posición/Escala global en el mundo)
uniform mat4 model;     

void main() {
    gl_Position = camMatrix * model * vec4(aPos, 1.0f);
    TexCoords = aTexCoords;
}