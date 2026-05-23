#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

// Mapeador de la textura cargada
uniform sampler2D texture_diffuse1;

void main() {
    // Si usas una textura mapeada la pinta, si no, pinta color base blanco texturizado
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}