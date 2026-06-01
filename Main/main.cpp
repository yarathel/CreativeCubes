#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>

#include "shaderClass.h"
#include "Camera.h"
#include "Texture.h"
#include "TextureCall.h" // Controlador de climas

#ifdef _WIN32
#include <windows.h>
#endif

// Ventana
const unsigned int width = 960;
const unsigned int height = 640;

// Estados de la aplicación
enum AppState
{
    MENU,
    CONFIG,
    CREDITS,
    JUEGO
};

AppState currentState = MENU;

// Estructura de Botones
struct Boton2D
{
    float x, y;
    float ancho, alto;

    bool estaPresionado(double mouseX, double mouseY)
    {
        return (
            mouseX >= x &&
            mouseX <= x + ancho &&
            mouseY >= y &&
            mouseY <= y + alto
            );
    }
};

// Ubicación de Botones ORIGINALES
Boton2D botonJugar = { 355.0f, 450.0f, 250.0f, 60.0f };
Boton2D botonConfig = { 355.0f, 350.0f, 250.0f, 60.0f };
Boton2D botonCredits = { 355.0f, 250.0f, 250.0f, 60.0f };
Boton2D botonBack = { 30.0f, 50.0f, 200.0f, 100.0f };

// Prototipos
unsigned int setupMenuQuad();
unsigned int setupSkyboxCube();
unsigned int setupObjetoCubo();
void processInput(GLFWwindow* window, Camera& camera);
void renderBoton(Shader& shader, unsigned int quadVAO, Boton2D& boton, glm::mat4& projection2D);

// Vector dinámico global para almacenar los cubos colocados por el usuario
std::vector<glm::vec3> posicionesCubos;

// MAIN
int main()
{
#ifdef _WIN32
    SetProcessDPIAware();
#endif

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        width,
        height,
        "Creative Cubes 3D",
        NULL,
        NULL
    );

    if (window == NULL)
    {
        std::cout << "Error ventana" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error GLAD" << std::endl;
        return -1;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Inicialización de Shaders
    Shader menuShader("menu.vert", "menu.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");
    Shader cubeShader("default.vert", "default.frag");

    // Instancia de cámara en el origen
    Camera camera(width, height, glm::vec3(0.0f, 1.8f, 5.0f));

    // Configuración de VAOs
    unsigned int menuQuadVAO = setupMenuQuad();
    unsigned int skyboxVAO = setupSkyboxCube();
    unsigned int cubeVAO = setupObjetoCubo();

    // Carga de Texturas
    Texture texFondoMenu("climasoleado.png", "2D", 0);
    Texture texBotonIniciar("buttoninicio.png", "2D", 0);
    Texture texBotonReglas("regla.png", "2D", 0);
    Texture texBotonCreditos("creditos.png", "2D", 0);
    Texture texBotonBack("Back.png", "2D", 0);
    Texture texPantallaConfig("pantallaReglas.png", "2D", 0);
    Texture texPantallaCreditos("pantallaCreditos.png", "2D", 0);
    Texture texCuboMundo("buttoninicio.png", "2D", 0);

    // Inicializamos Skyboxes del controlador climático
    TextureCall::inicializarSkyboxes();
    cubeShader.Activate();
    glUniform1i(glGetUniformLocation(cubeShader.ID, "texture_diffuse1"), 0);

    glm::mat4 projection2D = glm::ortho(
        0.0f, (float)width,
        0.0f, (float)height,
        -1.0f, 1.0f
    );

    bool mostrarCreditos = true;

    // LOOP PRINCIPAL
    while (!glfwWindowShouldClose(window))
    {
        processInput(window, camera);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ==================================================
        // MODO JUEGO
        // ==================================================
        if (currentState == JUEGO)
        {
            // --- 1. RENDERIZADO DEL SKYBOX ---
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);

            skyboxShader.Activate();

            glm::mat4 viewSkybox = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            glm::mat4 projMundo = glm::perspective(
                glm::radians(45.0f),
                (float)width / (float)height,
                0.1f,
                100.0f
            );

            skyboxShader.setMat4("view", viewSkybox);
            skyboxShader.setMat4("projection", projMundo);

            glBindVertexArray(skyboxVAO);
            TextureCall::vincularSkyboxActual();

            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);

            // --- 2. RENDERIZADO DE LOS CUBOS 3D ---
            cubeShader.Activate();

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 camMatrix = projection * view;

            cubeShader.setMat4("camMatrix", camMatrix);

            glActiveTexture(GL_TEXTURE0);
            texCuboMundo.Bind();
            glBindVertexArray(cubeVAO);

            for (size_t i = 0; i < posicionesCubos.size(); i++)
            {
                glm::mat4 modelCubo = glm::translate(glm::mat4(1.0f), posicionesCubos[i]);
                cubeShader.setMat4("model", modelCubo);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // --- 3. BOTÓN BACK SOBRE EL JUEGO (2D) ---
            glDisable(GL_DEPTH_TEST);
            menuShader.Activate();
            texBotonBack.Bind();
            renderBoton(menuShader, menuQuadVAO, botonBack, projection2D);
            glEnable(GL_DEPTH_TEST);
        }
        // ==================================================
        // INTERFACES 2D
        // ==================================================
        else
        {
            glDisable(GL_DEPTH_TEST);
            menuShader.Activate();

            if (currentState == MENU)
            {
                texFondoMenu.Bind();

                glm::mat4 modelFondo = glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3((float)width / 2.0f, (float)height / 2.0f, 0.0f)
                );
                modelFondo = glm::scale(
                    modelFondo,
                    glm::vec3((float)width / 2.0f, (float)height / 2.0f, 1.0f)
                );

                menuShader.setMat4("model2D", projection2D * modelFondo);

                glBindVertexArray(menuQuadVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                texBotonIniciar.Bind();
                renderBoton(menuShader, menuQuadVAO, botonJugar, projection2D);

                texBotonReglas.Bind();
                renderBoton(menuShader, menuQuadVAO, botonConfig, projection2D);

                texBotonCreditos.Bind();
                renderBoton(menuShader, menuQuadVAO, botonCredits, projection2D);
            }
            else if (currentState == CONFIG)
            {
                texPantallaConfig.Bind();

                glm::mat4 modelConfig = glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3((float)width / 2.0f, (float)height / 2.0f, 0.0f)
                );
                modelConfig = glm::scale(
                    modelConfig,
                    glm::vec3((float)width / 2.0f, (float)height / 2.0f, 1.0f)
                );

                menuShader.setMat4("model2D", projection2D * modelConfig);

                glBindVertexArray(menuQuadVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                texBotonBack.Bind();
                renderBoton(menuShader, menuQuadVAO, botonBack, projection2D);
            }
            else if (currentState == CREDITS)
            {
                texPantallaCreditos.Bind();

                glm::mat4 modelCredits = glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3((float)width / 2.0f, (float)height / 2.0f, 0.0f)
                );
                modelCredits = glm::scale(
                    modelCredits,
                    glm::vec3((float)width / 2.0f, (float)height / 2.0f, 1.0f)
                );

                menuShader.setMat4("model2D", projection2D * modelCredits);

                glBindVertexArray(menuQuadVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                if (mostrarCreditos)
                {
#ifdef _WIN32
                    system("cls");
#else
                    system("clear");
#endif
                    std::cout << "\n=====================================\n";
                    std::cout << "         CREATIVE CUBES 3D\n";
                    std::cout << "=====================================\n\n";
                    std::cout << "Proyecto realizado por:\n\n";
                    std::cout << "Baltodano Solis Ashley Nayara\n";
                    std::cout << "Sergio Amir Morales Vallejos\n";
                    std::cout << "Yader Jose Rodriguez Benavidez\n\n";
                    std::cout << "Ingenieria en Computacion\n\n";
                    std::cout << "Asignatura:\nProgramacion Grafica\n\nAño: 2026\n\n";
                    std::cout << "=====================================\n";
                    std::cout << "Boton ATRAS o tecla M\n";
                    std::cout << "=====================================\n";
                    mostrarCreditos = false;
                }

                texBotonBack.Bind();
                renderBoton(menuShader, menuQuadVAO, botonBack, projection2D);
            }
            glEnable(GL_DEPTH_TEST);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (currentState == MENU) mostrarCreditos = true;
    }

    TextureCall::eliminarSkyboxes();
    glfwTerminate();
    return 0;
}

// PROCESAR ENTRADAS
void processInput(GLFWwindow* window, Camera& camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (currentState == JUEGO)
    {
        camera.Inputs(window);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) TextureCall::cambiarClima(DIA);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) TextureCall::cambiarClima(TARDE);
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) TextureCall::cambiarClima(NOCHE);

        // Colocar bloques dinámicamente en los tres ejes
        // ==========================================
// CÓDIGO EXISTENTE: COLOCAR BLOQUES (TECLA L)
// ==========================================
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

        // ==========================================
        // NUEVO CÓDIGO: BORRAR BLOQUES (TECLA K)
        // ==========================================
        static bool kPressedLastFrame = false;
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        {
            if (!kPressedLastFrame)
            {
                kPressedLastFrame = true;

                // Calculamos el punto al que está mirando la cámara
                glm::vec3 deFrente = camera.Position + (camera.Orientation * 2.5f);
                glm::vec3 posicionObjetivo = glm::vec3(
                    std::round(deFrente.x),
                    std::round(deFrente.y),
                    std::round(deFrente.z)
                );

                // Buscamos si existe un cubo en esa posición exacta para eliminarlo
                for (auto it = posicionesCubos.begin(); it != posicionesCubos.end(); ++it)
                {
                    if (it->x == posicionObjetivo.x &&
                        it->y == posicionObjetivo.y &&
                        it->z == posicionObjetivo.z)
                    {
                        posicionesCubos.erase(it); // Elimina el cubo del vector
                        break; // Salimos del bucle tras borrarlo
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
            mouseY = height - mouseY;

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
                            "¿Seguro que quieres volver al menu principal? Todo tu progreso y cubos creados se borraran.",
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
            "¿Seguro que quieres volver al menu principal? Todo tu progreso y cubos creados se borraran.",
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

// RENDER BOTÓN
void renderBoton(Shader& shader, unsigned int quadVAO, Boton2D& boton, glm::mat4& projection2D) {
    float centroX = boton.x + (boton.ancho / 2.0f);
    float centroY = boton.y + (boton.alto / 2.0f);
    float escalaX = boton.ancho / 2.0f;
    float escalaY = boton.alto / 2.0f;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(centroX, centroY, 0.0f));
    model = glm::scale(model, glm::vec3(escalaX, escalaY, 1.0f));

    shader.setMat4("model2D", projection2D * model);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// GENERAR QUAD DE MENÚ
unsigned int setupMenuQuad() {
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    return quadVAO;
}

// GENERAR CUBO DE SKYBOX
unsigned int setupSkyboxCube() {
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,    1.0f,  1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,   -1.0f, -1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,    1.0f, -1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,    1.0f,  1.0f, -1.0f,    1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,    1.0f, -1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,    1.0f,  1.0f, -1.0f,    1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f,    1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,   -1.0f, -1.0f, -1.0f
    };
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    return skyboxVAO;
}

// GENERAR CUBO TRIDIMENSIONAL
unsigned int setupObjetoCubo() {
    float vertices[] = {
        // POSICIÓN (XYZ)      // NORMALES (XYZ)      // UV (ST)
        // Cara Trasera
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,    0.0f, 0.0f,

        // Cara Frontal
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,    0.0f, 0.0f,

        // Cara Izquierda
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

        // Cara Derecha
         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

         // Cara Inferior
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,    0.0f, 1.0f,

        // Cara Superior
       -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,    0.0f, 1.0f
    };

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLsizei stride = 8 * sizeof(float);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    return cubeVAO;
}
