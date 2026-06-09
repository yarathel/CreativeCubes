#ifndef TEXTURE_H
#define TEXTURE_H

#include <stb_image.h>
#include <glad/glad.h>
#include <iostream>
#include <string>

class Texture
{
public:
    unsigned int ID;
    std::string type;
    unsigned int unit;

    Texture(const char* imagePath, const char* texType, unsigned int slot)
    {
        type = texType;
        unit = slot;

        glGenTextures(1, &ID);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int widthImg, heightImg, numColCh;

        stbi_set_flip_vertically_on_load(true);

        unsigned char* bytes =
            stbi_load(imagePath, &widthImg, &heightImg, &numColCh, 0);

        if (bytes)
        {
            GLenum format = GL_RGB;

            if (numColCh == 1)
                format = GL_RED;
            else if (numColCh == 3)
                format = GL_RGB;
            else if (numColCh == 4)
                format = GL_RGBA;

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                format,
                widthImg,
                heightImg,
                0,
                format,
                GL_UNSIGNED_BYTE,
                bytes
            );

            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(bytes);
        }
        else
        {
            std::cout
                << "AVISO::TEXTURE::No se encontro archivo: "
                << imagePath
                << std::endl;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Bind()
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    void Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Delete()
    {
        glDeleteTextures(1, &ID);
    }
};

#endif
