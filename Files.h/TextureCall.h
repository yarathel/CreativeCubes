#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp> // Añadido para glm::value_ptr en el Skybox
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "Shader.h"
#include "Texture.h"
#include "Camera.h" // Añadido para el renderizado del Skybox con la cámara

// 1. Enumeraciones de Estado
enum AppState { MENU, CONFIG, CREDITS, JUEGO };
enum SkyboxType { DIA, TARDE, NOCHE };

// 2. Estructura del Botón
struct Boton2D {
    float x, y;
    float ancho, alto;

    bool estaPresionado(double mouseX, double mouseY) {
        return (mouseX >= x && mouseX <= x + ancho && mouseY >= y && mouseY <= y + alto);
    }
};

// 3. Contenedor del Sistema de Texturas (Header-Only puro sin 'inline')
struct TextureCall {

    // --- TRUCO C++: Funciones que devuelven una referencia a la variable ---
    static AppState& currentState() {
        static AppState state = MENU;
        return state;
    }

    static SkyboxType& currentSkybox() {
        static SkyboxType skybox = DIA;
        return skybox;
    }

    static unsigned int* cubemapTextures() {
        static unsigned int textures[3] = { 0, 0, 0 };
        return textures;
    }

    static Boton2D& botonJugar() {
        static Boton2D boton = { 475.0f, 450.0f, 250.0f, 60.0f };
        return boton;
    }

    static Boton2D& botonConfig() {
        static Boton2D boton = { 475.0f, 350.0f, 250.0f, 60.0f };
        return boton;
    }

    static Boton2D& botonCredits() {
        static Boton2D boton = { 475.0f, 250.0f, 250.0f, 60.0f };
        return boton;
    }

    // Punteros para las texturas internas gestionados como variables estáticas locales
    static Texture*& texFondoMenu() { static Texture* t = nullptr; return t; }
    static Texture*& texBotonIniciar() { static Texture* t = nullptr; return t; }
    static Texture*& texBotonReglas() { static Texture* t = nullptr; return t; }
    static Texture*& texBotonCreditos() { static Texture* t = nullptr; return t; }

    // --- VARIABLES DE GEOMETRÍA Y CONFIGURACIÓN INTEGRADAS ---
    static unsigned int& menuQuadVAO() { static unsigned int vao = 0; return vao; }
    static unsigned int& menuQuadVBO() { static unsigned int vbo = 0; return vbo; }
    static unsigned int& skyboxVAO() { static unsigned int vao = 0; return vao; }
    static unsigned int& skyboxVBO() { static unsigned int vbo = 0; return vbo; }

    static float* colorFondo3D() {
        static float color[3] = { 0.1f, 0.14f, 0.18f };
        return color;
    }

    static bool& modoOscuro() { static bool oscuro = true; return oscuro; }


    // --- FUNCIONES DE CARGA Y LOGICA ---
    static unsigned int loadCubemapFromHorizontalCross(const std::string& path) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        stbi_set_flip_vertically_on_load(false);
        int widthImg, heightImg, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &widthImg, &heightImg, &nrChannels, 0);

        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            int faceSize = 0;
            bool isHorizontal = false;

            int estimatedFaceWidthH = widthImg / 4;
            int estimatedFaceHeightH = heightImg / 3;
            int estimatedFaceWidthV = widthImg / 3;
            int estimatedFaceHeightV = heightImg / 4;

            if (std::abs(estimatedFaceWidthH - estimatedFaceHeightH) <= 2) {
                isHorizontal = true;
                faceSize = estimatedFaceWidthH;
            }
            else if (std::abs(estimatedFaceWidthV - estimatedFaceHeightV) <= 2) {
                isHorizontal = false;
                faceSize = estimatedFaceWidthV;
            }
            else {
                std::cerr << "Formato de cubemap invalido en: " << path << " (" << widthImg << "x" << heightImg << ")\n";
                stbi_image_free(data);
                glDeleteTextures(1, &textureID);
                return 0;
            }

            int faceOffsets[6][2];
            if (isHorizontal) {
                faceOffsets[0][0] = 2; faceOffsets[0][1] = 1; // +X
                faceOffsets[1][0] = 0; faceOffsets[1][1] = 1; // -X
                faceOffsets[2][0] = 1; faceOffsets[2][1] = 0; // +Y
                faceOffsets[3][0] = 1; faceOffsets[3][1] = 2; // -Y
                faceOffsets[4][0] = 1; faceOffsets[4][1] = 1; // +Z
                faceOffsets[5][0] = 3; faceOffsets[5][1] = 1; // -Z
            }
            else {
                faceOffsets[0][0] = 2; faceOffsets[0][1] = 1; // +X
                faceOffsets[1][0] = 0; faceOffsets[1][1] = 1; // -X
                faceOffsets[2][0] = 1; faceOffsets[2][1] = 0; // +Y
                faceOffsets[3][0] = 1; faceOffsets[3][1] = 2; // -Y
                faceOffsets[4][0] = 1; faceOffsets[4][1] = 1; // +Z
                faceOffsets[5][0] = 1; faceOffsets[5][1] = 3; // -Z
            }

            std::vector<unsigned char> faceData(faceSize * faceSize * nrChannels);

            for (unsigned int i = 0; i < 6; i++) {
                int offsetX = faceOffsets[i][0] * faceSize;
                int offsetY = faceOffsets[i][1] * faceSize;

                std::fill(faceData.begin(), faceData.end(), 0);

                for (int y = 0; y < faceSize; y++) {
                    int srcY = offsetY + y;
                    int srcX = offsetX;

                    if (srcY >= heightImg) continue;

                    unsigned char* dstPtr = faceData.data() + (y * faceSize) * nrChannels;
                    int rowStart = (srcY * widthImg + srcX) * nrChannels;

                    int pixelsToCopy = faceSize;
                    if (srcX + pixelsToCopy > widthImg) {
                        pixelsToCopy = widthImg - srcX;
                    }

                    if (pixelsToCopy > 0) {
                        int rowBytes = pixelsToCopy * nrChannels;
                        memcpy(dstPtr, data + rowStart, rowBytes);
                    }
                }
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, faceData.data());
            }
            stbi_image_free(data);
        }
        else {
            std::cout << "Error: No se pudo cargar el archivo " << path << std::endl;
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    static void inicializarTexturasYMenus() {
        texFondoMenu() = new Texture("climasoleado.png", "2D", 0);
        texBotonIniciar() = new Texture("buttoninicio.png", "2D", 0);
        texBotonReglas() = new Texture("regla.png", "2D", 0);
        texBotonCreditos() = new Texture("creditos.png", "2D", 0);
    }

    static void cargarLosSkyboxes() {
        cubemapTextures()[DIA] = loadCubemapFromHorizontalCross("TEXTURAS/cubemap/Cubemap_Sky_02-512x512.png");
        cubemapTextures()[TARDE] = loadCubemapFromHorizontalCross("TEXTURAS/cubemap/Cubemap_Sky_01-512x512.png");
        cubemapTextures()[NOCHE] = loadCubemapFromHorizontalCross("TEXTURAS/cubemap/Cubemap_Sky_15-512x512.png");
    }

    // Genera buffers geométricos del menú 2D y del Skybox 3D
    static void inicializarGeometriaYMenus() {
        // --- Quad 2D para interfaz ---
        float quadVertices[] = {
            -1.0f,  1.0f,   0.0f, 1.0f,
            -1.0f, -1.0f,   0.0f, 0.0f,
             1.0f,  1.0f,   1.0f, 1.0f,
             1.0f, -1.0f,   1.0f, 0.0f,
        };
        glGenVertexArrays(1, &menuQuadVAO());
        glGenBuffers(1, &menuQuadVBO());
        glBindVertexArray(menuQuadVAO());
        glBindBuffer(GL_ARRAY_BUFFER, menuQuadVBO());
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        // --- Cubo 3D para Skybox ---
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
        glGenVertexArrays(1, &skyboxVAO());
        glGenBuffers(1, &skyboxVBO());
        glBindVertexArray(skyboxVAO());
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO());
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }

    // Intercambiar color claro/oscuro del fondo 3D
    static void conmutarModoOscuro() {
        modoOscuro() = !modoOscuro();
        if (modoOscuro()) {
            colorFondo3D()[0] = 0.1f; colorFondo3D()[1] = 0.14f; colorFondo3D()[2] = 0.18f;
        }
        else {
            colorFondo3D()[0] = 0.4f; colorFondo3D()[1] = 0.55f; colorFondo3D()[2] = 0.7f;
        }
    }

    // Gestiona el color dinámico de glClearColor
    static void establecerColorDeFondoClear() {
        if (currentState() == MENU)        glClearColor(0.15f, 0.15f, 0.25f, 1.0f);
        else if (currentState() == CONFIG)  glClearColor(0.15f, 0.25f, 0.15f, 1.0f);
        else if (currentState() == CREDITS) glClearColor(0.25f, 0.15f, 0.15f, 1.0f);
        else if (currentState() == JUEGO)   glClearColor(colorFondo3D()[0], colorFondo3D()[1], colorFondo3D()[2], 1.0f);
    }

    static void liberarTexturasYMenus() {
        if (texFondoMenu()) { texFondoMenu()->Delete(); delete texFondoMenu(); }
        if (texBotonIniciar()) { texBotonIniciar()->Delete(); delete texBotonIniciar(); }
        if (texBotonReglas()) { texBotonReglas()->Delete(); delete texBotonReglas(); }
        if (texBotonCreditos()) { texBotonCreditos()->Delete(); delete texBotonCreditos(); }
        glDeleteTextures(3, cubemapTextures());
    }

    // Centraliza la destrucción total al cerrar la app
    static void liberarTodo() {
        glDeleteVertexArrays(1, &menuQuadVAO());
        glDeleteBuffers(1, &menuQuadVBO());
        glDeleteVertexArrays(1, &skyboxVAO());
        glDeleteBuffers(1, &skyboxVBO());
        liberarTexturasYMenus();
    }

    static void dibujarBotonInterno(Shader& shader, unsigned int quadVAO, Boton2D& boton, const glm::mat4& projection2D) {
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

    static void renderizarMenuPrincipal(Shader& menuShader, unsigned int menuQuadVAO, const glm::mat4& projection2D) {
        glm::mat4 modelFondo = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(600.0f, 400.0f, 0.0f)), glm::vec3(600.0f, 400.0f, 1.0f));

        if (texFondoMenu()) texFondoMenu()->Bind();
        menuShader.setMat4("model2D", projection2D * modelFondo);
        glBindVertexArray(menuQuadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        if (texBotonIniciar()) texBotonIniciar()->Bind();
        dibujarBotonInterno(menuShader, menuQuadVAO, botonJugar(), projection2D);

        if (texBotonReglas()) texBotonReglas()->Bind();
        dibujarBotonInterno(menuShader, menuQuadVAO, botonConfig(), projection2D);

        if (texBotonCreditos()) texBotonCreditos()->Bind();
        dibujarBotonInterno(menuShader, menuQuadVAO, botonCredits(), projection2D);
    }

    static void renderizarPantallaConfig(Shader& menuShader, unsigned int menuQuadVAO, const glm::mat4& projection2D) {
        glm::mat4 modelFondo = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(600.0f, 400.0f, 0.0f)), glm::vec3(600.0f, 400.0f, 1.0f));
        if (texFondoMenu()) texFondoMenu()->Bind();
        menuShader.setMat4("model2D", projection2D * modelFondo);
        glBindVertexArray(menuQuadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    static void renderizarPantallaCreditos(Shader& menuShader, unsigned int menuQuadVAO, const glm::mat4& projection2D) {
        glm::mat4 modelFondo = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(600.0f, 400.0f, 0.0f)), glm::vec3(600.0f, 400.0f, 1.0f));
        if (texFondoMenu()) texFondoMenu()->Bind();
        menuShader.setMat4("model2D", projection2D * modelFondo);
        glBindVertexArray(menuQuadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // Distribuidor automático de pantallas 2D basado en el estado actual
    static void renderizarEstadoActual2D(Shader& shader, const glm::mat4& projection2D) {
        if (currentState() == MENU) {
            renderizarMenuPrincipal(shader, menuQuadVAO(), projection2D);
        }
        else if (currentState() == CONFIG) {
            renderizarPantallaConfig(shader, menuQuadVAO(), projection2D);
        }
        else if (currentState() == CREDITS) {
            renderizarPantallaCreditos(shader, menuQuadVAO(), projection2D);
        }
    }

    // Encapsula el dibujado matemático del Skybox en el entorno 3D
    static void renderizarSkybox(Shader& skyboxShader, Camera& camera, float w, float h) {
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Activate();

        glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), w / h, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(skyboxVAO());
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextures()[currentSkybox()]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LESS);
    }
};
