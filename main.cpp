#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

// Dimensiones de la ventana
const unsigned int width = 1200;
const unsigned int height = 800;

// Máquina de estados del juego
enum AppState { MENU, CONFIG, CREDITS, JUEGO };
AppState currentState = MENU;

// Variables de configuración de fondo para el modo 3D
float colorFondo[3] = { 0.1f, 0.14f, 0.18f };
bool modoOscuro = true;

// =========================================================================
// ESTRUCTURA Y LOGICA DE BOTONES INTERACTIVOS 2D
// =========================================================================
struct Boton2D {
    float x, y;          // Posición en píxeles (Esquina inferior izquierda)
    float ancho, alto;   // Tamaño en píxeles

    // Detecta si el cursor colisiona con el área rectangular del botón
    bool estaPresionado(double mouseX, double mouseY) {
        return (mouseX >= x && mouseX <= x + ancho && mouseY >= y && mouseY <= y + alto);
    }
};

// Declaración de botones globales del Menú Principal
Boton2D botonJugar = { 475.0f, 450.0f, 250.0f, 60.0f }; // Botón Superior (Iniciar)
Boton2D botonConfig = { 475.0f, 350.0f, 250.0f, 60.0f }; // Botón Central (Reglas)
Boton2D botonCredits = { 475.0f, 250.0f, 250.0f, 60.0f }; // Botón Inferior (Créditos)

// Prototipos de funciones
unsigned int setupMenuQuad();
unsigned int loadSkyboxVAO();
void processInput(GLFWwindow* window, Camera& camera);
void renderBoton(Shader& shader, unsigned int quadVAO, Boton2D& boton, glm::mat4& projection2D);

int main() {
    // 1. Inicializar entorno GLFW y ventana
    if (!glfwInit()) {
        std::cout << "Error al inicializar GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "CreativeCubes 3D - Menu Interactivo", NULL, NULL);
    if (window == NULL) {
        std::cout << "Error al crear la ventana" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error al inicializar GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

    // =========================================================================
    // CARGA DE SHADERS Y TEXTURAS
    // =========================================================================
    Shader objectShader("default.vert", "default.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");
    Shader menuShader("menu.vert", "menu.frag");

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 5.0f));

    // Cargar buffers geométricos planos para el HUD/Menú y cúbicos para el cielo
    unsigned int menuQuadVAO = setupMenuQuad();
    unsigned int skyboxVAO = loadSkyboxVAO();

    // Textura del fondo de pantalla completo
    Texture texFondoMenu("climasoleado.png", "2D", 0);

    // NUEVAS: 3 Texturas independientes con sus textos correspondientes dibujados
    Texture texBotonIniciar("buttoninicio.png", "2D", 0);
    Texture texBotonReglas("regla.png", "2D", 0);
    Texture texBotonCreditos("creditos.png", "2D", 0);

    // Matriz de proyección ortogonal para trabajar en 2D con píxeles exactos
    glm::mat4 projection2D = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

    // Inicialización de textura vacía simulada para el Cubemap del Skybox
    unsigned int cubemapTexture;
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Loop de renderizado principal
    while (!glfwWindowShouldClose(window)) {
        processInput(window, camera);

        // Colores de fondo de respaldo si faltan las imágenes en algún estado
        if (currentState == MENU)       glClearColor(0.15f, 0.15f, 0.25f, 1.0f);
        else if (currentState == CONFIG)  glClearColor(0.15f, 0.25f, 0.15f, 1.0f);
        else if (currentState == CREDITS) glClearColor(0.25f, 0.15f, 0.15f, 1.0f);
        else if (currentState == JUEGO)   glClearColor(colorFondo[0], colorFondo[1], colorFondo[2], 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // =====================================================================
        // RENDERIZADO SEGÚN EL ESTADO
        // =====================================================================
        if (currentState == JUEGO) {
            // --- MODO 3D ACTIVO ---
            objectShader.Activate();
            camera.Matrix(45.0f, 0.1f, 100.0f, objectShader, "camMatrix");
            glm::mat4 modelMat = glm::mat4(1.0f);
            objectShader.setMat4("model", modelMat);

            // Renderizar Fondo (Skybox)
            glDepthFunc(GL_LEQUAL);
            skyboxShader.Activate();
            glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
            glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

            glBindVertexArray(skyboxVAO);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);
        }
        else {
            // --- MODO INTERFAZ 2D ACTIVO (MENU, CONFIG o CREDITS) ---
            glDisable(GL_DEPTH_TEST);
            menuShader.Activate();

            if (currentState == MENU) {
                // 1. Dibujar el fondo completo del paisaje de montañas (climasoleado.png)
                texFondoMenu.Bind();
                glm::mat4 modelFondo = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(600.0f, 400.0f, 0.0f)), glm::vec3(600.0f, 400.0f, 1.0f));
                menuShader.setMat4("model2D", projection2D * modelFondo);
                glBindVertexArray(menuQuadVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                // 2. Dibujar el Botón Superior (Iniciar) con su propia textura
                texBotonIniciar.Bind();
                renderBoton(menuShader, menuQuadVAO, botonJugar, projection2D);

                // 3. Dibujar el Botón Central (Reglas) con su propia textura
                texBotonReglas.Bind();
                renderBoton(menuShader, menuQuadVAO, botonConfig, projection2D);

                // 4. Dibujar el Botón Inferior (Créditos) con su propia textura
                texBotonCreditos.Bind();
                renderBoton(menuShader, menuQuadVAO, botonCredits, projection2D);
            }
            else {
                // Pantallas secundarias de Reglas o Créditos (Se limpian con el color del ClearColor plano)
                // En el futuro, puedes renderizar texturas específicas para los textos explicativos aquí.
            }

            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Liberar memoria de los buffers y las nuevas texturas
    glDeleteVertexArrays(1, &menuQuadVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    texFondoMenu.Delete();
    texBotonIniciar.Delete();
    texBotonReglas.Delete();
    texBotonCreditos.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// Procesa las entradas del teclado y los clics del ratón
void processInput(GLFWwindow* window, Camera& camera) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Captura de clics únicamente cuando estamos en el Menú Principal
    if (currentState == MENU) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // Invertir el eje Y de GLFW a coordenadas matemáticas OpenGL
            mouseY = height - mouseY;

            // Evaluar la colisión exacta en píxeles del cursor con cada botón
            if (botonJugar.estaPresionado(mouseX, mouseY)) {
                currentState = JUEGO;
                glfwWaitEventsTimeout(0.2);
            }
            else if (botonConfig.estaPresionado(mouseX, mouseY)) {
                currentState = CONFIG; // Cambia al estado de las Reglas
                glfwWaitEventsTimeout(0.2);
            }
            else if (botonCredits.estaPresionado(mouseX, mouseY)) {
                currentState = CREDITS; // Cambia al estado de los Créditos
                glfwWaitEventsTimeout(0.2);
            }
        }
    }
    else if (currentState == CONFIG) {
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            modoOscuro = !modoOscuro;
            if (modoOscuro) {
                colorFondo[0] = 0.1f; colorFondo[1] = 0.14f; colorFondo[2] = 0.18f;
            }
            else {
                colorFondo[0] = 0.4f; colorFondo[1] = 0.55f; colorFondo[2] = 0.7f;
            }
            glfwWaitEventsTimeout(0.2);
        }
    }
    else if (currentState == JUEGO) {
        camera.Inputs(window);
    }

    // Regresar al Menú Principal de forma universal presionando la tecla 'M'
    if (currentState != MENU) {
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            currentState = MENU;
            glfwWaitEventsTimeout(0.2);
        }
    }
}

// Función auxiliar para renderizar y posicionar cada botón escalándolo matemáticamente
void renderBoton(Shader& shader, unsigned int quadVAO, Boton2D& boton, glm::mat4& projection2D) {
    float centroX = boton.x + (boton.ancho / 2.0f);
    float centroY = boton.y + (boton.alto / 2.0f);
    float escalaX = boton.ancho / 2.0f;
    float escalaY = boton.alto / 2.0f;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(centroX, centroY, 0.0f));
    model = glm::scale(model, glm::vec3(escalaX, escalaY, 1.0f));

    shader.setMat4("model2D", projection2D * model);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// Define un plano base centrado en el origen con coordenadas locales
unsigned int setupMenuQuad() {
    float quadVertices[] = {
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    return quadVAO;
}

unsigned int loadSkyboxVAO() {
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f
    };
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    return skyboxVAO;
}