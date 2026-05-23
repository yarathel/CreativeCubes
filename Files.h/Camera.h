#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Camera {
public:
    // Vectores de coordenadas de la cámara
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Evita saltos bruscos del cursor al enfocar por primera vez
    bool firstClick = true;

    // Dimensiones de la ventana
    int width;
    int height;

    // Velocidad y sensibilidad del control libre
    float speed = 0.05f;
    float sensitivity = 100.0f;

    // Ángulos de rotación del ratón
    float yaw = -90.0f;
    float pitch = 0.0f;

    // Constructor de la Cámara
    Camera(int width, int height, glm::vec3 position) {
        this->width = width;
        this->height = height;
        this->Position = position;
    }

    // Calcula la matriz de Vista y Proyección juntas y se las pasa al Shader
    void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform) {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        // Hacia dónde mira la cámara
        view = glm::lookAt(Position, Position + Orientation, Up);
        // Perspectiva matemática tridimensional
        projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

        // Envía el resultado combinado al shader multiplicando Proyección * Vista
        shader.setMat4(uniform, projection * view);
    }

    // Procesa las entradas del teclado y ratón para el movimiento libre
    void Inputs(GLFWwindow* window) {
        // Movimiento por teclado
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            Position += speed * Orientation;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            Position -= speed * Orientation;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            Position -= speed * glm::normalize(glm::cross(Orientation, Up));
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            Position += speed * glm::normalize(glm::cross(Orientation, Up));
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            Position += speed * Up;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            Position -= speed * Up;

        // Movimiento por ratón (Captura de coordenadas en pantalla)
       // Movimiento por ratón (Captura de coordenadas en pantalla)
// SE CORRIGIÓ: GLFW_LEFT_BUTTON por GLFW_MOUSE_BUTTON_LEFT
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            // Oculta el cursor mientras arrastramos la vista
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // ... (el resto de tu código de manejo de mouse sigue igual)

     

            if (firstClick) {
                lastX = mouseX;
                lastY = mouseY;
                firstClick = false;
            }

            float xoffset = mouseX - lastX;
            float yoffset = lastY - mouseY; // Invertido ya que las coordenadas Y van de abajo hacia arriba
            lastX = mouseX;
            lastY = mouseY;

            xoffset *= (sensitivity / 1000.0f);
            yoffset *= (sensitivity / 1000.0f);

            yaw += xoffset;
            pitch += yoffset;

            // Limitar la rotación vertical para no voltear la cámara al revés
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;

            glm::vec3 direction;
            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            Orientation = glm::normalize(direction);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstClick = true;
        }
    }

private:
    double lastX = 0.0, lastY = 0.0;
};
