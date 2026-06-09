#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Camera.h"

void processInput(
    GLFWwindow* window,
    Camera& camera
);
