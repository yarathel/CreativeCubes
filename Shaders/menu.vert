#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

// Matriz ortogonal combinada con la posición y escala del botón
uniform mat4 model2D; 

void main() {
    gl_Position = model2D * vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
}
