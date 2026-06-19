#include "Input.h"
#include "Globals.h"
#include "TextureCall.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <cmath>

static bool configuracionVieneDePausa = false;

void processInput(GLFWwindow* window, Camera& camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        TextureCall::cambiarClima(DIA);
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        TextureCall::cambiarClima(TARDE);
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        TextureCall::cambiarClima(NOCHE);
    }

    // --- 1. DETECCIÓN DE LA TECLA TAB (CONMUTAR PAUSA Y SALIR DE CONFIG) ---
    static bool tabPressedLastFrame = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if (!tabPressedLastFrame)
        {
            tabPressedLastFrame = true;

            if (currentState == CONFIG)
            {
                if (configuracionVieneDePausa)
                {
                    currentState = PAUSA;
                }
                else
                {
                    currentState = MENU;
                }
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else if (currentState == JUEGO)
            {
                currentState = PAUSA;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else if (currentState == PAUSA)
            {
                currentState = JUEGO;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                camera.firstClick = true;
            }
        }
    }
    else
    {
        tabPressedLastFrame = false;
    }

    // El juego SOLO procesa cámara y bloques si está en estado JUEGO
    if (currentState == JUEGO)
    {
        camera.Inputs(window);

        static bool lPressedLastFrame = false;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            if (!lPressedLastFrame)
            {
                lPressedLastFrame = true;
                glm::vec3 deFrente = camera.Position + (camera.Orientation * 2.5f);
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

        static bool kPressedLastFrame = false;
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        {
            if (!kPressedLastFrame)
            {
                kPressedLastFrame = true;
                glm::vec3 deFrente = camera.Position + (camera.Orientation * 2.5f);
                glm::vec3 posicionObjetivo = glm::vec3(
                    std::round(deFrente.x),
                    std::round(deFrente.y),
                    std::round(deFrente.z)
                );

                for (auto it = posicionesCubos.begin(); it != posicionesCubos.end(); ++it)
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

    // --- GESTIÓN DE CLICS DEL MOUSE ---
    static bool mousePressedLastFrame = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (!mousePressedLastFrame)
        {
            mousePressedLastFrame = true;
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            if (currentState == JUEGO)
            {
                mouseY = camera.height - mouseY;
            }
            else
            {
                float virtualX = (float)mouseX * (960.0f / (float)camera.width);
                float virtualY = ((float)camera.height - (float)mouseY) * (640.0f / (float)camera.height);
                mouseX = virtualX;
                mouseY = virtualY;
            }

            if (currentState == MENU)
            {
                if (botonJugar.estaPresionado(mouseX, mouseY))
                    currentState = JUEGO;
                else if (botonConfig.estaPresionado(mouseX, mouseY))
                {
                    configuracionVieneDePausa = false;
                    currentState = CONFIG;
                }
                else if (botonCredits.estaPresionado(mouseX, mouseY))
                    currentState = CREDITS;
                else if (botonSalir.estaPresionado(mouseX, mouseY))
                    glfwSetWindowShouldClose(window, true);
            }
            else if (currentState == PAUSA)
            {
                float virtualX = (float)mouseX;
                float virtualY = (float)mouseY;

                if (botonResume.estaPresionado(virtualX, virtualY))
                {
                    currentState = JUEGO;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    camera.firstClick = true;
                }
                else if (botonRules.estaPresionado(virtualX, virtualY))
                {
                    configuracionVieneDePausa = true;
                    currentState = CONFIG;
                }
                else if (botonMainMenu.estaPresionado(virtualX, virtualY))
                {
#ifdef _WIN32
                    int respuesta = MessageBoxA(
                        NULL,
                        "All your progress and created cubes will be erased.",
                        "Warning - Creative Cubes 3D",
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
            // --- LÓGICA DE BOTÓN BACK RESTAURADA PARA CONFIG Y CREDITS ---
            else if (currentState == CONFIG || currentState == CREDITS)
            {
                if (botonBack.estaPresionado(mouseX, mouseY))
                {
                    if (currentState == CONFIG)
                    {
                        if (configuracionVieneDePausa)
                        {
                            currentState = PAUSA; // Si venías de la pausa, vuelve a la pausa
                        }
                        else
                        {
                            currentState = MENU;  // Si venías del menú principal, vuelve al inicio
                        }
                    }
                    else if (currentState == CREDITS)
                    {
                        currentState = MENU;
                    }
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    camera.firstClick = true;
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
            " All your progress and created cubes will be erased.",
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
