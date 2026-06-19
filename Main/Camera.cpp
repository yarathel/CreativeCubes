#include "Camera.h"
#include <vector>   
#include <cmath>    
#include <algorithm>

// Declaramos externamente el vector global para que Camera.cpp sepa que existe en main.cpp
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

    // 2. OBTENER DIRECCIONES HORIZONTALES (XZ)
    glm::vec3 forwardHorizontal = glm::normalize(glm::vec3(Orientation.x, 0.0f, Orientation.z));
    glm::vec3 rightHorizontal = glm::normalize(glm::cross(forwardHorizontal, Up));

    glm::vec3 movimientoHorizontal(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movimientoHorizontal += forwardHorizontal;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movimientoHorizontal -= forwardHorizontal;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movimientoHorizontal -= rightHorizontal;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movimientoHorizontal += rightHorizontal;

    if (glm::length(movimientoHorizontal) > 0.0f)
    {
        movimientoHorizontal = glm::normalize(movimientoHorizontal) * currentSpeed;
    }

    // 3. CÁLCULO DE GRAVEDAD Y SALTO (Usa el MAX_FALL_SPEED que corregiste en el .h)
    if (!isGrounded)
    {
        velocityY += GRAVITY * deltaTime;

        // Ahora que MAX_FALL_SPEED es -12.0f, te dejará caer con aceleración real
        if (velocityY < MAX_FALL_SPEED)
        {
            velocityY = MAX_FALL_SPEED;
        }
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

    // Usamos una variable temporal limpia para recalcular este frame de manera controlada
    glm::vec3 nextPosition = Position;

    // Dimensiones de cajas AABB
    float cubeHalfSize = 0.5f;
    float camHalfWidth = cameraSize.x / 2.0f;
    float camHalfDepth = cameraSize.z / 2.0f;

    // ============================================================================
    // PASO A: APLICAR Y RESOLVER MOVIMIENTO VERTICAL (EJE Y)
    // ============================================================================
    nextPosition.y += velocityY * deltaTime;
    isGrounded = false;

    // Suelo base del mapa (Nivel cero)
    if (nextPosition.y <= 1.8f)
    {
        nextPosition.y = 1.8f;
        velocityY = 0.0f;
        isGrounded = true;
    }

    // Colisiones en Y contra bloques
    for (size_t i = 0; i < posicionesCubos.size(); i++)
    {
        glm::vec3 cuboPos = posicionesCubos[i];

        float minCuboX = cuboPos.x - cubeHalfSize;
        float maxCuboX = cuboPos.x + cubeHalfSize;
        float minCuboY = cuboPos.y - cubeHalfSize;
        float maxCuboY = cuboPos.y + cubeHalfSize;
        float minCuboZ = cuboPos.z - cubeHalfSize;
        float maxCuboZ = cuboPos.z + cubeHalfSize;

        float minCamX = nextPosition.x - camHalfWidth;
        float maxCamX = nextPosition.x + camHalfWidth;
        float minCamY = nextPosition.y - cameraSize.y; // Pies del jugador
        float maxCamY = nextPosition.y;               // Ojos/Cabeza del jugador
        float minCamZ = nextPosition.z - camHalfDepth;
        float maxCamZ = nextPosition.z + camHalfDepth;

        if (maxCamX > minCuboX && minCamX < maxCuboX &&
            maxCamY > minCuboY && minCamY < maxCamY &&
            maxCamZ > minCuboZ && minCamZ < maxCuboZ)
        {
            // Caso 1: Vienes cayendo o estás apoyado en la mitad superior del cubo
            if (velocityY <= 0.0f && minCamY >= minCuboY)
            {
                nextPosition.y = maxCuboY + cameraSize.y;
                velocityY = 0.0f;
                isGrounded = true;
            }
            // Caso 2: Vas subiendo con fuerza positiva y tu cabeza golpea la base inferior del cubo
            else if (velocityY > 0.0f && maxCamY < (maxCuboY - 0.1f))
            {
                nextPosition.y = minCuboY;
                velocityY = 0.0f;
            }
        }
    }

    // ============================================================================
    // PASO B: APLICAR Y RESOLVER MOVIMIENTO HORIZONTAL (EJES X / Z)
    // ============================================================================
    nextPosition.x += movimientoHorizontal.x;
    nextPosition.z += movimientoHorizontal.z;

    for (size_t i = 0; i < posicionesCubos.size(); i++)
    {
        glm::vec3 cuboPos = posicionesCubos[i];

        float minCuboX = cuboPos.x - cubeHalfSize;
        float maxCuboX = cuboPos.x + cubeHalfSize;
        float minCuboY = cuboPos.y - cubeHalfSize;
        float maxCuboY = cuboPos.y + cubeHalfSize;
        float minCuboZ = cuboPos.z - cubeHalfSize;
        float maxCuboZ = cuboPos.z + cubeHalfSize;

        float minCamX = nextPosition.x - camHalfWidth;
        float maxCamX = nextPosition.x + camHalfWidth;
        float minCamY = nextPosition.y - cameraSize.y;
        float maxCamY = nextPosition.y;
        float minCamZ = nextPosition.z - camHalfDepth;
        float maxCamZ = nextPosition.z + camHalfDepth;

        if (maxCamX > minCuboX && minCamX < maxCuboX &&
            maxCamY > minCuboY && minCamY < maxCamY &&
            maxCamZ > minCuboZ && minCamZ < maxCuboZ)
        {
            // Tolerancia de escalón
            if (minCamY >= (maxCuboY - 0.1f))
            {
                continue;
            }

            float overlapX = std::min(maxCamX - minCuboX, maxCuboX - minCamX);
            float overlapZ = std::min(maxCamZ - minCuboZ, maxCuboZ - minCamZ);

            if (overlapX < overlapZ)
            {
                if (nextPosition.x < cuboPos.x) nextPosition.x -= overlapX;
                else nextPosition.x += overlapX;
            }
            else
            {
                if (nextPosition.z < cuboPos.z) nextPosition.z -= overlapZ;
                else nextPosition.z += overlapZ;
            }
        }
    }

    // Guardamos la posición corregida final en la variable del objeto
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

        double mouseX, mouseY;
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
