#include "Input.h"
#include "Globals.h"
#include "TextureCall.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <cmath>

void processInput(GLFWwindow* window, Camera& camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Tecla 1 = Día
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        TextureCall::cambiarClima(DIA);
    }

    // Tecla 2 = Tarde
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        TextureCall::cambiarClima(TARDE);
    }

    // Tecla 3 = Noche
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        TextureCall::cambiarClima(NOCHE);
    }

    if (currentState == JUEGO)
    {
        // Movimiento WASD + cámara con mouse
        camera.Inputs(window);

        // ==========================================
        // CÓDIGO EXISTENTE: COLOCAR BLOQUES (TECLA L)
        // ==========================================
        static bool lPressedLastFrame = false;

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            if (!lPressedLastFrame)
            {
                lPressedLastFrame = true;

                glm::vec3 deFrente =
                    camera.Position + (camera.Orientation * 2.5f);

                glm::vec3 posicionBloque = glm::vec3(
                    std::round(deFrente.x),
                    std::round(deFrente.y),
                    std::round(deFrente.z)
                );

                posicionesCubos.push_back(posicionBloque);
            }
        }
        else
        {
            lPressedLastFrame = false;
        }

        // ==========================================
        // BORRAR BLOQUES (TECLA K)
        // ==========================================
        static bool kPressedLastFrame = false;

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        {
            if (!kPressedLastFrame)
            {
                kPressedLastFrame = true;

                glm::vec3 deFrente =
                    camera.Position + (camera.Orientation * 2.5f);

                glm::vec3 posicionObjetivo = glm::vec3(
                    std::round(deFrente.x),
                    std::round(deFrente.y),
                    std::round(deFrente.z)
                );

                for (auto it = posicionesCubos.begin();
                    it != posicionesCubos.end();
                    ++it)
                {
                    if (it->x == posicionObjetivo.x &&
                        it->y == posicionObjetivo.y &&
                        it->z == posicionObjetivo.z)
                    {
                        posicionesCubos.erase(it);
                        break;
                    }
                }
            }
        }
        else
        {
            kPressedLastFrame = false;
        }
    }

    static bool mousePressedLastFrame = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (!mousePressedLastFrame)
        {
            mousePressedLastFrame = true;
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            mouseY = camera.height - mouseY;

            if (currentState == MENU)
            {
                if (botonJugar.estaPresionado(mouseX, mouseY))
                    currentState = JUEGO;
                else if (botonConfig.estaPresionado(mouseX, mouseY))
                    currentState = CONFIG;
                else if (botonCredits.estaPresionado(mouseX, mouseY))
                    currentState = CREDITS;
            }
            else if (currentState == CONFIG || currentState == CREDITS || currentState == JUEGO)
            {
                if (botonBack.estaPresionado(mouseX, mouseY))
                {
                    if (currentState == JUEGO)
                    {
#ifdef _WIN32
                        int respuesta = MessageBoxA(
                            NULL,
                            "Todo tu progreso y cubos creados se borraran.",
                            "Advertencia - Creative Cubes 3D",
                            MB_YESNO | MB_ICONWARNING | MB_TOPMOST
                        );

                        if (respuesta == IDYES)
                        {
                            posicionesCubos.clear();
                            currentState = MENU;
                            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                            camera.firstClick = true;
                        }
#else
                        posicionesCubos.clear();
                        currentState = MENU;
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        camera.firstClick = true;
#endif
                    }
                    else
                    {
                        currentState = MENU;
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        camera.firstClick = true;
                    }
                }
            }
        }
    }
    else
    {
        mousePressedLastFrame = false;
    }

    if (currentState == JUEGO && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
#ifdef _WIN32
        int respuesta = MessageBoxA(
            NULL,
            " Todo tu progreso y cubos creados se borraran.",
            "Advertencia - Creative Cubes 3D",
            MB_YESNO | MB_ICONWARNING | MB_TOPMOST
        );

        if (respuesta == IDYES)
        {
            posicionesCubos.clear();
            currentState = MENU;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            camera.firstClick = true;
        }
#else
        posicionesCubos.clear();
        currentState = MENU;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        camera.firstClick = true;
#endif
    }
}
