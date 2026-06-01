#include "Camera.h"
#include <vector>   
#include <cmath>    
#include <algorithm>

// CORRECCIÓN: Declaramos externamente el vector global para que Camera.cpp sepa que existe en main.cpp
extern std::vector<glm::vec3> posicionesCubos;

// Constructor de la Cámara
Camera::Camera(int width, int height, glm::vec3 position)
{
    this->width = width;
    this->height = height;
    Position = position;
}

// Retorna la matriz de vista
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Orientation, Up);
}

// Maneja la gravedad, saltos, movimientos horizontales y resolución AABB contra bloques
void Camera::Inputs(GLFWwindow* window)
{
    // 1. OBTENCIÓN DINÁMICA DE TIEMPOS (Delta Time)
    static float lastFrame = 0.0f;
    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    if (deltaTime > 0.1f) deltaTime = 0.1f; // Filtro para evitar saltos en tirones de FPS

    float currentSpeed = speed * deltaTime;
    glm::vec3 nextPosition = Position;

    // 2. OBTENER DIRECCIONES SÓLO EN EL PLANO HORIZONTALES (XZ)
    glm::vec3 forwardHorizontal = glm::normalize(glm::vec3(Orientation.x, 0.0f, Orientation.z));
    glm::vec3 rightHorizontal = glm::normalize(glm::cross(forwardHorizontal, Up));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        nextPosition += currentSpeed * forwardHorizontal;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        nextPosition -= currentSpeed * forwardHorizontal;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        nextPosition -= currentSpeed * rightHorizontal;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        nextPosition += currentSpeed * rightHorizontal;

    // 3. CÁLCULO DE GRAVEDAD Y ACCIÓN DE SALTO
    if (!isGrounded)
    {
        velocityY += GRAVITY * deltaTime;
    }
    else
    {
        if (velocityY < 0.0f) velocityY = 0.0f;

        // Saltar pulsando la barra espaciadora
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            velocityY = JUMP_FORCE;
            isGrounded = false;
        }
    }

    nextPosition.y += velocityY * deltaTime;

    // 4. LÍMITE DE SUELO POR DEFECTO (Y = 1.8f para ojos del jugador al nivel cero)
    if (nextPosition.y <= 1.8f)
    {
        nextPosition.y = 1.8f;
        velocityY = 0.0f;
        isGrounded = true;
    }
    else
    {
        isGrounded = false;
    }

    // 5. EVALUACIÓN DE COLISIONES AABB
    float cubeHalfSize = 0.5f;
    float camHalfWidth = cameraSize.x / 2.0f;
    float camHalfDepth = cameraSize.z / 2.0f;

    // Evaluar colisiones con cada cubo del escenario
    for (size_t i = 0; i < posicionesCubos.size(); i++)
    {
        glm::vec3 cuboPos = posicionesCubos[i];

        // Caja del cubo actual
        float minCuboX = cuboPos.x - cubeHalfSize;
        float maxCuboX = cuboPos.x + cubeHalfSize;
        float minCuboY = cuboPos.y - cubeHalfSize;
        float maxCuboY = cuboPos.y + cubeHalfSize;
        float minCuboZ = cuboPos.z - cubeHalfSize;
        float maxCuboZ = cuboPos.z + cubeHalfSize;

        // Caja de la cámara
        float minCamX = nextPosition.x - camHalfWidth;
        float maxCamX = nextPosition.x + camHalfWidth;
        float minCamY = nextPosition.y - cameraSize.y; // Base de los pies (Suelo)
        float maxCamY = nextPosition.y;               // Altura de los ojos (Techo)

        // CORRECCIÓN: Definición de límites Z de la cámara faltantes
        float minCamZ = nextPosition.z - camHalfDepth;
        float maxCamZ = nextPosition.z + camHalfDepth;

        // Verificar intersección tridimensional
        bool colisionX = maxCamX > minCuboX && minCamX < maxCuboX;
        bool colisionY = maxCamY > minCuboY && minCamY < maxCamY;
        bool colisionZ = maxCamZ > minCuboZ && minCamZ < maxCuboZ;

        if (colisionX && colisionY && colisionZ)
        {
            // Medir profundidad de colisión en cada eje
            float overlapX = std::min(maxCamX - minCuboX, maxCuboX - minCamX);
            float overlapY = std::min(maxCamY - minCuboY, maxCuboY - minCamY);
            float overlapZ = std::min(maxCamZ - minCuboZ, maxCuboZ - minCamZ);

            // Resolver colisión en el eje de menor penetración
            if (overlapY < overlapX && overlapY < overlapZ)
            {
                if (Position.y - cameraSize.y >= maxCuboY - 0.05f) // Caída sobre el bloque
                {
                    nextPosition.y = maxCuboY + cameraSize.y;
                    velocityY = 0.0f;
                    isGrounded = true;
                }
                else if (Position.y <= minCuboY + 0.05f) // Colisión desde abajo hacia el bloque
                {
                    nextPosition.y = minCuboY;
                    if (velocityY > 0.0f) velocityY = 0.0f;
                }
            }
            else if (overlapX < overlapZ)
            {
                // Colisión lateral en eje X
                if (Position.x < cuboPos.x) nextPosition.x -= overlapX;
                else nextPosition.x += overlapX;
            }
            else
            {
                // Colisión lateral en eje Z
                if (Position.z < cuboPos.z) nextPosition.z -= overlapZ;
                else nextPosition.z += overlapZ;
            }
        }
    }

    // Guardar posición física aprobada
    Position = nextPosition;

    // 6. RENDERIZACIÓN Y ORIENTACIÓN MEDIANTE EL RATÓN
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (firstClick)
        {
            glfwSetCursorPos(window, (width / 2), (height / 2));
            firstClick = false;
        }

        double mouseX;
        double mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
        float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

        if (std::abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
        {
            Orientation = newOrientation;
        }

        Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
        glfwSetCursorPos(window, (width / 2), (height / 2));
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
    }
}
