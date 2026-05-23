#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

// Sampler especial para texturas tridimensionales cúbicas (6 caras de imágenes)
uniform samplerCube skybox;

void main() {    
    FragColor = texture(skybox, TexCoords);
}
