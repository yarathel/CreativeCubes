#ifndef LOADING_H
#define LOADING_H

#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Texture.h"

namespace Loading {
    // Renderiza el cubo animado, el texto LOADING y la barra de progreso en movimiento
    void Render(GLFWwindow* window, Shader& cubeShader, unsigned int cubeVAO, Texture& texCuboMundo,
        Shader& menuShader, unsigned int fondoVAO, Texture& texLoadingText, float duration);
}

#endif
