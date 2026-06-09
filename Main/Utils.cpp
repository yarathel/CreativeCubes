#include "Utils.h"

#include <glm/gtc/matrix_transform.hpp>


void renderBoton(Shader& shader, unsigned int quadVAO, const Boton2D& boton, const glm::mat4& projection2D) {
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
