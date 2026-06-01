#include <glad/glad.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // Añadido para glm::value_ptr en el Skybox
#include <iostream>
#pragma once
#ifndef TEXTURE_CALL_H
#define TEXTURE_CALL_H

#include <stb/stb_image.h> // <--- CRUCIAL: Añadido para stbi_load y corregir errores

#include <string>
#include <vector>
#include <cmath>

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

// NOTA: Quitamos AppState, SkyboxType y Boton2D de aquí porque YA existen en tu main.cpp
// Solo dejamos las declaraciones que usará la estructura.
enum SkyboxType { DIA, TARDE, NOCHE };

struct TextureCall {

    // --- Variables de Estado Internas ---
    static int& currentState() {
        static int state = 0; // Guardado como entero para evitar conflictos de enums
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

    // Punteros para las texturas internas gestionados como variables estáticas locales
    static Texture*& texFondoMenu() { static Texture* t = nullptr; return t; }
    static Texture*& texBotonIniciar() { static Texture* t = nullptr; return t; }
    static Texture*& texBotonReglas() { static Texture* t = nullptr; return t; }
    static Texture*& texBotonCreditos() { static Texture* t = nullptr; return t; }

    // VARIABLES DE GEOMETRÍA INTEGRADAS
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

    // Inicializa texturas del mapa
    static void inicializarSkyboxes() {
        cubemapTextures()[DIA] = loadCubemapFromHorizontalCross("Cubemap_Sky_02-512x512.png");
        cubemapTextures()[TARDE] = loadCubemapFromHorizontalCross("Cubemap_Sky_01-512x512.png");
        cubemapTextures()[NOCHE] = loadCubemapFromHorizontalCross("Cubemap_Sky_15-512x512.png");
    }

    // Vincula el cubemap activo para que sea dibujado por tu main
    static void vincularSkyboxActual() {
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextures()[currentSkybox()]);
    }

    // Cambia dinámicamente el clima actual
    static void cambiarClima(SkyboxType tipo) {
        currentSkybox() = tipo;
    }

    // Elimina las texturas de la GPU de forma limpia
    static void eliminarSkyboxes() {
        glDeleteTextures(3, cubemapTextures());
    }
};

#endif
