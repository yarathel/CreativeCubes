#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "Shader.h"

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    bool firstClick = true;

    int width;
    int height;

    float speed = 4.0f;
    float sensitivity = 100.0f;

    float velocityY = 0.0f;
    const float GRAVITY = -9.81f;
    const float JUMP_FORCE = 4.2f;
    bool isGrounded = false;
    glm::vec3 cameraSize = glm::vec3(0.5f, 1.8f, 0.5f);

    Camera(int width, int height, glm::vec3 position);
    glm::mat4 GetViewMatrix();
    void Inputs(GLFWwindow* window);
};

#endif
