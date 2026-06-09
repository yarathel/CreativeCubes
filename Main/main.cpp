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

#ifdef _WIN32
#include <windows.h>
#endif

const unsigned int width = 1920;
const unsigned int height = 1080;


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

}

int main()
{
#ifdef _WIN32
    SetProcessDPIAware();
#endif

    // Inicialización GLFW
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

    Shader menuShader("menu.vert", "menu.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");
    Shader cubeShader("default.vert", "default.frag");

    Camera camera(width, height, glm::vec3(0.0f, 1.8f, 5.0f));

    unsigned int menuQuadVAO = setupMenuQuad();
    unsigned int skyboxVAO = setupSkyboxCube();
    unsigned int cubeVAO = setupObjetoCubo();

    Texture texFondoMenu("climasoleado.png", "2D", 0);
    Texture texBotonIniciar("buttoninicio.png", "2D", 0);
    Texture texBotonReglas("rules.png", "2D", 0);
    Texture texBotonCreditos("credit.png", "2D", 0);
    Texture texBotonBack("Back.png", "2D", 0);
    Texture texPantallaConfig("pantallaReglas.png", "2D", 0);
    Texture texPantallaCreditos("pantallaCreditos.png", "2D", 0);
    Texture texCuboMundo("ladrillo.jpg", "2D", 0);

    TextureCall::inicializarSkyboxes();

    cubeShader.Activate();
    glUniform1i(
        glGetUniformLocation(cubeShader.ID, "texture_diffuse1"),
        0
    );

    glm::mat4 projection2D = glm::ortho(
        0.0f,
        (float)width,
        0.0f,
        (float)height,
        -1.0f,
        1.0f
    );

    bool mostrarCreditos = true;

    while (!glfwWindowShouldClose(window))
    {
        {
            processInput(window, camera);

            int currentWidth, currentHeight;
            glfwGetFramebufferSize(window, &currentWidth, &currentHeight);


            camera.width = currentWidth;
            camera.height = currentHeight;

            glm::mat4 projection2D = glm::ortho(
                0.0f,
                (float)currentWidth,
                0.0f,
                (float)currentHeight,
                -1.0f,
                1.0f
            );
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // ==================================================
            // MODO JUEGO
            // ==================================================
            if (currentState == JUEGO)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);

                skyboxShader.Activate();

                glm::mat4 viewSkybox = glm::mat4(glm::mat3(camera.GetViewMatrix()));
                glm::mat4 projMundo = glm::perspective(
                    glm::radians(45.0f),
                    (float)currentWidth / (float)currentHeight,
                    0.1f,
                    100.0f
                );

                skyboxShader.setMat4("view", viewSkybox);
                skyboxShader.setMat4("projection", projMundo);

                glBindVertexArray(skyboxVAO);
                TextureCall::vincularSkyboxActual();

                glDrawArrays(GL_TRIANGLES, 0, 36);
                glDepthFunc(GL_LESS);

                cubeShader.Activate();

                glm::mat4 projection = glm::perspective(
                    glm::radians(45.0f),
                    (float)currentWidth / (float)currentHeight,
                    0.1f,
                    100.0f
                );

                glm::mat4 view = camera.GetViewMatrix();
                glm::mat4 camMatrix = projection * view;

                cubeShader.setMat4("camMatrix", camMatrix);

                glActiveTexture(GL_TEXTURE0);
                texCuboMundo.Bind();
                glBindVertexArray(cubeVAO);

                for (size_t i = 0; i < posicionesCubos.size(); i++)
                {
                    glm::mat4 modelCubo =
                        glm::translate(glm::mat4(1.0f), posicionesCubos[i]);

                    cubeShader.setMat4("model", modelCubo);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }

                glDisable(GL_DEPTH_TEST);

                menuShader.Activate();
                texBotonBack.Bind();

                renderBoton(
                    menuShader,
                    menuQuadVAO,
                    botonBack,
                    projection2D
                );

                glEnable(GL_DEPTH_TEST);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);

                menuShader.Activate();

                glm::mat4 projectionMenu = glm::ortho(
                    0.0f,
                    960.0f,
                    0.0f,
                    640.0f,
                    -1.0f,
                    1.0f
                );

                if (currentState == MENU)
                {
                    texFondoMenu.Bind();

                    glm::mat4 modelFondo = glm::translate(
                        glm::mat4(1.0f),
                        glm::vec3(480.0f, 320.0f, 0.0f)
                    );

                    modelFondo = glm::scale(
                        modelFondo,
                        glm::vec3(480.0f, 320.0f, 1.0f)
                    );

                    menuShader.setMat4(
                        "model2D",
                        projectionMenu * modelFondo
                    );

                    glBindVertexArray(menuQuadVAO);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                    texBotonIniciar.Bind();
                    renderBoton(menuShader, menuQuadVAO, botonJugar, projectionMenu);

                    texBotonReglas.Bind();
                    renderBoton(menuShader, menuQuadVAO, botonConfig, projectionMenu);

                    texBotonCreditos.Bind();
                    renderBoton(menuShader, menuQuadVAO, botonCredits, projectionMenu);
                }
                else if (currentState == CONFIG)
                {
                    texPantallaConfig.Bind();

                    glm::mat4 modelConfig = glm::translate(
                        glm::mat4(1.0f),
                        glm::vec3(480.0f, 320.0f, 0.0f)
                    );

                    modelConfig = glm::scale(
                        modelConfig,
                        glm::vec3(480.0f, 320.0f, 0.0f)
                    );

                    menuShader.setMat4(
                        "model2D",
                        projectionMenu * modelConfig
                    );

                    glBindVertexArray(menuQuadVAO);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                    texBotonBack.Bind();
                    renderBoton(menuShader, menuQuadVAO, botonBack, projectionMenu);
                }
                else if (currentState == CREDITS)
                {
                    texPantallaCreditos.Bind();

                    glm::mat4 modelCredits = glm::translate(
                        glm::mat4(1.0f),
                        glm::vec3(480.0f, 320.0f, 0.0f)
                    );

                    modelCredits = glm::scale(
                        modelCredits,
                        glm::vec3(480.0f, 320.0f, 0.0f)
                    );

                    menuShader.setMat4(
                        "model2D",
                        projectionMenu * modelCredits
                    );

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

                        mostrarCreditos = false;
                    }

                    texBotonBack.Bind();
                    renderBoton(menuShader, menuQuadVAO, botonBack, projectionMenu);
                }

                glEnable(GL_DEPTH_TEST);
            }

            glfwSwapBuffers(window);
            glfwPollEvents();

            if (currentState == MENU)
                mostrarCreditos = true;
        }
    }

    TextureCall::eliminarSkyboxes();

    glfwTerminate();
    return 0;
}
