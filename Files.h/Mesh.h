#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>

class CubeMesh {
public:
    unsigned int VAO, VBO, EBO;

    CubeMesh() {
        // Coordenadas de los vértices de un cubo 3D estándar (X, Y, Z, U, V para textura)
        float vertices[] = {
            // Cara Frontal
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            // Cara Trasera
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };

        // Índices para unir los triángulos y formar las caras del cubo
        unsigned int indices[] = {
            0, 1, 2,  2, 3, 0, // Frontal
            1, 5, 6,  6, 2, 1, // Derecha
            7, 6, 5,  5, 4, 7, // Trasera
            4, 0, 3,  3, 7, 4, // Izquierda
            3, 2, 6,  6, 7, 3, // Superior
            4, 5, 1,  1, 0, 4  // Inferior
        };

        // Generar los buffers en la GPU
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Cargar vértices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Cargar índices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Definir Atributo de Posición (X, Y, Z) en el layout 0
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Definir Atributo de Coordenadas de Textura (U, V) en el layout 1
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    // Método para dibujar el cubo en la pantalla
    void Draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Limpieza de memoria gráfica
    void Delete() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};
#endif
