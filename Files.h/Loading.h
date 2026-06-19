#ifndef LOADING_H
#define LOADING_H

#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Texture.h"

namespace Loading {
    void Render(GLFWwindow* window, Shader& cubeShader, unsigned int cubeVAO, Texture& texCuboMundo,
        Shader& menuShader, unsigned int fondoVAO, Texture& texLoadingText, float duration);
}

#endif
