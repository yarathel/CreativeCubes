#include <glad/glad.h>
#include "Loading.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace Loading {
    void Render(GLFWwindow* window, Shader& cubeShader, unsigned int cubeVAO, Texture& texCuboMundo,
        Shader& menuShader, unsigned int fondoVAO, Texture& texLoadingText, float duration) {
        float tiempoInicio = (float)glfwGetTime();

        while ((float)glfwGetTime() - tiempoInicio < duration && !glfwWindowShouldClose(window)) {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            glViewport(0, 0, width, height);

            // Fondo oscuro verdoso de referencia
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float tiempoActual = (float)glfwGetTime();
            float progreso = (tiempoActual - tiempoInicio) / duration;
            if (progreso > 1.0f) progreso = 1.0f;

            // =========================================================
            // 1. RENDERIZAR CUBO 3D (GIRA, FLOTA Y PULSA)
            // =========================================================
            glEnable(GL_DEPTH_TEST);
            cubeShader.Activate();

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            cubeShader.setMat4("camMatrix", projection * view);

            float rotacionX = tiempoActual * 1.2f;
            float rotacionY = tiempoActual * 1.6f;
            float flotarY = std::sin(tiempoActual * 3.0f) * 0.12f;
            float escalaPulsante = 1.0f + std::cos(tiempoActual * 2.5f) * 0.05f;

            glm::mat4 modelCubo = glm::mat4(1.0f);
            modelCubo = glm::translate(modelCubo, glm::vec3(0.0f, flotarY + 0.2f, 0.0f));
            modelCubo = glm::rotate(modelCubo, rotacionX, glm::vec3(1.0f, 0.0f, 0.0f));
            modelCubo = glm::rotate(modelCubo, rotacionY, glm::vec3(0.0f, 1.0f, 0.0f));
            modelCubo = glm::scale(modelCubo, glm::vec3(escalaPulsante));
            cubeShader.setMat4("model", modelCubo);

            glActiveTexture(GL_TEXTURE0);
            texCuboMundo.Bind();
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // =========================================================
            // 2. RENDERIZAR INTERFAZ 2D (TEXTO LOADING + BARRA EN MOVIMIENTO)
            // =========================================================
            glDisable(GL_DEPTH_TEST);
            menuShader.Activate();
            glBindVertexArray(fondoVAO);

            // Texto LOADING
            texLoadingText.Bind();
            glm::mat4 modelTexto = glm::mat4(1.0f);
            modelTexto = glm::translate(modelTexto, glm::vec3(0.0f, -0.4f, 0.0f));
            modelTexto = glm::scale(modelTexto, glm::vec3(0.30f, 0.2f, 1.0f));
            menuShader.setMat4("model2D", modelTexto);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // Contenedor estático de la barra de carga
            texCuboMundo.Bind();
            glm::mat4 modelFondoBarra = glm::mat4(1.0f);
            modelFondoBarra = glm::translate(modelFondoBarra, glm::vec3(0.0f, -0.6f, 0.0f));
            modelFondoBarra = glm::scale(modelFondoBarra, glm::vec3(0.5f, 0.008f, 1.0f));
            menuShader.setMat4("model2D", modelFondoBarra);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // Barra de progreso animada (se mueve y llena hacia la derecha)
            glm::mat4 modelBarraProgreso = glm::mat4(1.0f);
            modelBarraProgreso = glm::translate(modelBarraProgreso, glm::vec3(-0.5f * (1.0f - progreso), -0.6f, 0.0f));
            modelBarraProgreso = glm::scale(modelBarraProgreso, glm::vec3(0.5f * progreso, 0.012f, 1.0f));
            menuShader.setMat4("model2D", modelBarraProgreso);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
}
