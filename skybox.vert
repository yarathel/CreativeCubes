#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    TexCoords = aPos;
    // Forzar coordenadas tridimensionales estáticas eliminando la traslación de la cámara
    vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0f);
    // El truco en el componente Z asegura que el cielo siempre se renderice al fondo del Z-Buffer
    gl_Position = pos.xyww; 
}