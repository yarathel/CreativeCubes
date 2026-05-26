#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

class Camera
{
public:
    // Posición fija en el centro
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Dimensiones ventana
    int width;
    int height;

    // Control de mouse (por si acaso)
    bool firstClick = true;

    // Velocidad de rotación con las teclas
    float rotationSpeed = 0.2f;
    float sensitivity = 0.5f;

    // Ángulos de rotación iniciales
    float yaw = -90.0f;
    float pitch = 0.0f;

    // Constructor
    Camera(int width, int height, glm::vec3 position)
    {
        this->width = width;
        this->height = height;
        this->Position = position;
    }

    // Matriz de vista
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Orientation, Up);
    }

    // ======================================================
    // CONTROL DE ROTACIÓN POR TECLADO EXCLUSIVO
    // ======================================================
    void Inputs(GLFWwindow* window)
    {
        // Tecla W: Mirar hacia ARRIBA
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            pitch += rotationSpeed;
        }
        // Tecla S: Mirar hacia ABAJO
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            pitch -= rotationSpeed;
        }
        // Tecla A: Girar hacia la IZQUIERDA
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            yaw -= rotationSpeed;
        }
        // Tecla D: Girar hacia la DERECHA
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            yaw += rotationSpeed;
        }

        // Limitar el ángulo vertical para no romper la cámara (no dar la vuelta completa)
        if (pitch > 89.0f)  pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        // recalcular el vector de orientación (hacia dónde miramos) basado en las rotaciones
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        Orientation = glm::normalize(direction);
    }
};

#endif
