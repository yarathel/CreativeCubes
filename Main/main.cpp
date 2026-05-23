#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "TextureCall.h" 

// Dimensiones de la ventana y variables de control delegadas a TextureCall
const unsigned int width = 1200;
const unsigned int height = 800;

// Prototipos de inicialización local para mantener limpio el flujo
GLFWwindow* inicializarVentana(unsigned int width, unsigned int height);
void processInput(GLFWwindow* window, Camera& camera);

int main() {
    // 1. Inicialización limpia del entorno gráfico
    GLFWwindow* window = inicializarVentana(width, height);
    if (!window) return -1;

    // 2. Instanciación de Shaders y Cámara
    Shader objectShader("default.vert", "default.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");
    Shader menuShader("menu.vert", "menu.frag");
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 5.0f));

    // Matriz de proyección ortogonal para 2D
    glm::mat4 projection2D = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

    // 3. Carga e inicialización de todos los recursos geométricos y texturas
    TextureCall::inicializarGeometriaYMenus();
    TextureCall::inicializarTexturasYMenus();
    TextureCall::cargarLosSkyboxes();

    // =========================================================================
    // LOOP DE RENDERIZADO PRINCIPAL
    // =========================================================================
    while (!glfwWindowShouldClose(window)) {
        processInput(window, camera);

        // Establecer el color de fondo dinámico dependiendo del estado
        TextureCall::establecerColorDeFondoClear();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (TextureCall::currentState() == JUEGO) {
            // --- MODO 3D ACTIVO ---
            objectShader.Activate();
            camera.Matrix(45.0f, 0.1f, 100.0f, objectShader, "camMatrix");
            objectShader.setMat4("model", glm::mat4(1.0f));

            // El Skybox se dibuja a sí mismo de manera interna
            TextureCall::renderizarSkybox(skyboxShader, camera, width, height);
        }
        else {
            // --- MODO INTERFAZ 2D ACTIVO (MENU, CONFIG o CREDITS) ---
            glDisable(GL_DEPTH_TEST);
            menuShader.Activate();

            TextureCall::renderizarEstadoActual2D(menuShader, projection2D);

            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // =========================================================================
    // LIMPIEZA DE MEMORIA TOTAL DELEGADA
    // =========================================================================
    TextureCall::liberarTodo();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// Inicialización modular de GLFW, Ventana, GLAD y Estados OpenGL
GLFWwindow* inicializarVentana(unsigned int width, unsigned int height) {
    if (!glfwInit()) {
        std::cout << "Error al inicializar GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "CreativeCubes 3D - Menu Interactivo", NULL, NULL);
    if (window == NULL) {
        std::cout << "Error al crear la ventana" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error al inicializar GLAD" << std::endl;
        return nullptr;
    }
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

    // Habilitar mezcla para transparencias PNG
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return window;
}

// Lógica de entradas del teclado y ratón adaptada a los nuevos estados globales
void processInput(GLFWwindow* window, Camera& camera) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (TextureCall::currentState() == MENU) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            mouseY = height - mouseY; // Invertir coordenadas del eje Y

            if (TextureCall::botonJugar().estaPresionado(mouseX, mouseY)) {
                TextureCall::currentState() = JUEGO;
                glfwWaitEventsTimeout(0.2);
            }
            else if (TextureCall::botonConfig().estaPresionado(mouseX, mouseY)) {
                TextureCall::currentState() = CONFIG;
                glfwWaitEventsTimeout(0.2);
            }
            else if (TextureCall::botonCredits().estaPresionado(mouseX, mouseY)) {
                TextureCall::currentState() = CREDITS;
                glfwWaitEventsTimeout(0.2);
            }
        }
    }
    else if (TextureCall::currentState() == CONFIG) {
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { TextureCall::currentSkybox() = DIA; glfwWaitEventsTimeout(0.2); }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { TextureCall::currentSkybox() = TARDE; glfwWaitEventsTimeout(0.2); }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { TextureCall::currentSkybox() = NOCHE; glfwWaitEventsTimeout(0.2); }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            TextureCall::conmutarModoOscuro();
            glfwWaitEventsTimeout(0.2);
        }
    }
    else if (TextureCall::currentState() == JUEGO) {
        camera.Inputs(window);
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { TextureCall::currentSkybox() = DIA; }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { TextureCall::currentSkybox() = TARDE; }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { TextureCall::currentSkybox() = NOCHE; }
    }

    if (TextureCall::currentState() != MENU && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        TextureCall::currentState() = MENU;
        glfwWaitEventsTimeout(0.2);
    }
}
