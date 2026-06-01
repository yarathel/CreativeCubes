#ifndef TEXTURE_H
#define TEXTURE_H

#include <stb_image.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <string>

class Texture {
public:
    unsigned int ID;
    std::string type;
    unsigned int unit;

    // Constructor único y corregido
    Texture(const char* imagePath, const char* texType, unsigned int slot) {
        type = texType;
        unit = slot;

        glGenTextures(1, &ID);
        glActiveTexture(GL_TEXTURE0 + slot);

        // ============================================================
        // CASO 1: CUBEMAP (TUS 6 IMÁGENES .JPG DEL BOSQUE)
        // ============================================================
        if (type == "cubemap" || type == "CUBEMAP") {
            glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

            // Nombres exactos de tus archivos subidos
            std::vector<std::string> facesFiles = {
                "right.jpg",  // +X
                "left.jpg",   // -X
                "top.jpg",    // +Y
                "bottom.jpg", // -Y
                "front.jpg",  // +Z
                "back.jpg"    // -Z
            };

            // Para paisajes 3D NO se voltea verticalmente
            stbi_set_flip_vertically_on_load(false);

            for (unsigned int i = 0; i < 6; i++) {
                // Construye la ruta combinando el directorio base con el nombre del archivo
                std::string fullPath = std::string(imagePath) + facesFiles[i];

                int widthImg, heightImg, numColCh;
                unsigned char* bytes = stbi_load(fullPath.c_str(), &widthImg, &heightImg, &numColCh, 0);

                if (bytes) {
                    GLenum format = GL_RGB;
                    if (numColCh == 1) format = GL_RED;
                    else if (numColCh == 3) format = GL_RGB;
                    else if (numColCh == 4) format = GL_RGBA;

                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
                    stbi_image_free(bytes);
                }
                else {
                    // Si no encuentra la foto, inyecta un color plano de respaldo para que no se quede negro
                    unsigned char fallbackColor[6][3] = {
                        {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}, {255, 0, 255}, {0, 255, 255}
                    };
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, fallbackColor[i]);
                    std::cout << "AVISO::CUBEMAP::No se encontro la cara: " << fullPath << " (Se aplico color de respaldo)" << std::endl;
                }
            }

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
        // ============================================================
        // CASO 2: TEXTURAS INTERFAZ 2D
        // ============================================================
        else {
            glBindTexture(GL_TEXTURE_2D, ID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            int widthImg, heightImg, numColCh;
            stbi_set_flip_vertically_on_load(true); // Las imágenes 2D del menú sí se voltean

            unsigned char* bytes = stbi_load(imagePath, &widthImg, &heightImg, &numColCh, 0);

            if (bytes) {
                GLenum format = GL_RGB;
                if (numColCh == 1) format = GL_RED;
                else if (numColCh == 3) format = GL_RGB;
                else if (numColCh == 4) format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, format, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(bytes);
            }
            else {
                std::cout << "AVISO::TEXTURE::No se encontro archivo fisico en: " << imagePath << std::endl;
            }

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void Bind() {
        glActiveTexture(GL_TEXTURE0 + unit);
        if (type == "cubemap" || type == "CUBEMAP") {
            glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, ID);
        }
    }

    void Unbind() {
        if (type == "cubemap" || type == "CUBEMAP") {
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void Delete() {
        glDeleteTextures(1, &ID);
    }
};

#endif
