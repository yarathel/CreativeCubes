#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Globals.h"

void renderBoton(
    Shader& shader,
    unsigned int quadVAO,
    const Boton2D& boton,
    const glm::mat4& projection2D
);

unsigned int setupMenuQuad();
unsigned int setupSkyboxCube();
unsigned int setupObjetoCubo();
