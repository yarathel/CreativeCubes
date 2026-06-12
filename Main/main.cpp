#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "TextureCall.h"

#include "Globals.h"
#include "Utils.h"
#include "Input.h"
#include "Loading.h"
#include "AudioPlayer.h"

#ifdef _WIN32
#include <windows.h>
#endif

const unsigned int width = 960;
const unsigned int height = 640;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
#ifdef _WIN32
    SetProcessDPIAware();
#endif

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Creative Cubes 3D", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Error ventana" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error GLAD" << std::endl;
        return -1;
    }

    int initWidth, initHeight;
    glfwGetFramebufferSize(window, &initWidth, &initHeight);
    glViewport(0, 0, initWidth, initHeight);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader menuShader("menu.vert", "menu.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");
    Shader cubeShader("default.vert", "default.frag");

    Camera camera(initWidth, initHeight, glm::vec3(0.0f, 1.8f, 5.0f));

    float fondoVertices[] = {
        -1.0f,  1.0f,    0.0f, 1.0f,
        -1.0f, -1.0f,    0.0f, 0.0f,
         1.0f,  1.0f,    1.0f, 1.0f,
         1.0f, -1.0f,    1.0f, 0.0f,
    };
    unsigned int fondoVAO, fondoVBO;
    glGenVertexArrays(1, &fondoVAO);
    glGenBuffers(1, &fondoVBO);
    glBindVertexArray(fondoVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fondoVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fondoVertices), fondoVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    unsigned int menuQuadVAO = setupMenuQuad();
    unsigned int skyboxVAO = setupSkyboxCube();
    unsigned int cubeVAO = setupObjetoCubo();

    Texture texFondoMenu("fondo_menu.png", "2D", 0);
    Texture texBotonIniciar("buttoninicio.png", "2D", 0);
    Texture texBotonReglas("rules.png", "2D", 0);
    Texture texBotonCreditos("credit.png", "2D", 0);
    Texture texBotonBack("Back.png", "2D", 0);
    Texture PantallaConfig("pantallaReglas.png", "2D", 0);
    Texture texPantallaCreditos("pantallaCreditos.png", "2D", 0);
    Texture texCuboMundo("ladrillo.jpg", "2D", 0);

    // Textura con el letrero "LOADING"
    Texture texLoadingText("loading.png", "2D", 0);

    TextureCall::inicializarSkyboxes();

    cubeShader.Activate();
    glUniform1i(glGetUniformLocation(cubeShader.ID, "texture_diffuse1"), 0);

    // Renderiza la animación completa por 8s segundos
    Loading::Render(window, cubeShader, cubeVAO, texCuboMundo, menuShader, fondoVAO, texLoadingText, 8.0f);

    bool mostrarCreditos = true;

    AudioController musicaFondo;

    musicaFondo.ReproducirPista("C:\\Users\\USUARIO\\source\\repos\\Proyecto_PG\\Proyecto_PG\\musicadabuti.wav");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window, camera);

        int currentWidth, currentHeight;
        glfwGetFramebufferSize(window, &currentWidth, &currentHeight);

        camera.width = currentWidth;
        camera.height = currentHeight;

        glm::mat4 projection2D = glm::ortho(0.0f, (float)currentWidth, 0.0f, (float)currentHeight, -1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (currentState == JUEGO)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);

            skyboxShader.Activate();
            glm::mat4 viewSkybox = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            glm::mat4 projMundo = glm::perspective(glm::radians(45.0f), (float)currentWidth / (float)currentHeight, 0.1f, 100.0f);

            skyboxShader.setMat4("view", viewSkybox);
            skyboxShader.setMat4("projection", projMundo);

            glBindVertexArray(skyboxVAO);
            TextureCall::vincularSkyboxActual();
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);

            cubeShader.Activate();
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)currentWidth / (float)currentHeight, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            cubeShader.setMat4("camMatrix", projection * view);

            glActiveTexture(GL_TEXTURE0);
            texCuboMundo.Bind();
            glBindVertexArray(cubeVAO);

            for (size_t i = 0; i < posicionesCubos.size(); i++)
            {
                glm::mat4 modelCubo = glm::translate(glm::mat4(1.0f), posicionesCubos[i]);
                cubeShader.setMat4("model", modelCubo);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            glDisable(GL_DEPTH_TEST);
            menuShader.Activate();
            texBotonBack.Bind();
            renderBoton(menuShader, menuQuadVAO, botonBack, projection2D);
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
            menuShader.Activate();

            glm::mat4 projectionMenuVirtual = glm::ortho(0.0f, 960.0f, 0.0f, 640.0f, -1.0f, 1.0f);

            if (currentState == MENU)
            {
                texFondoMenu.Bind();
                menuShader.setMat4("model2D", glm::mat4(1.0f));
                glBindVertexArray(fondoVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                texBotonIniciar.Bind();
                renderBoton(menuShader, menuQuadVAO, botonJugar, projectionMenuVirtual);

                texBotonReglas.Bind();
                renderBoton(menuShader, menuQuadVAO, botonConfig, projectionMenuVirtual);

                texBotonCreditos.Bind();
                renderBoton(menuShader, menuQuadVAO, botonCredits, projectionMenuVirtual);
            }
            else if (currentState == CONFIG)
            {
                PantallaConfig.Bind();
                menuShader.setMat4("model2D", glm::mat4(1.0f));
                glBindVertexArray(fondoVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                texBotonBack.Bind();
                renderBoton(menuShader, menuQuadVAO, botonBack, projectionMenuVirtual);
            }
            else if (currentState == CREDITS)
            {
                texPantallaCreditos.Bind();
                menuShader.setMat4("model2D", glm::mat4(1.0f));
                glBindVertexArray(fondoVAO);
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

                    mostrarCreditos = false;
                }

                texBotonBack.Bind();
                renderBoton(menuShader, menuQuadVAO, botonBack, projectionMenuVirtual);
            }
            glEnable(GL_DEPTH_TEST);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (currentState == MENU)
            mostrarCreditos = true;
    }

    glDeleteVertexArrays(1, &fondoVAO);
    glDeleteBuffers(1, &fondoVBO);
    musicaFondo.DetenerTodo();
    TextureCall::eliminarSkyboxes();
    glfwTerminate();
    return 0;
}
