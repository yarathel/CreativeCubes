#pragma once

#include <vector>
#include <glm/glm.hpp>

enum AppState
{
    MENU,
    CONFIG,
    CREDITS,
    JUEGO
};

struct Boton2D
{
    float x, y;
    float ancho, alto;

    bool estaPresionado(double mouseX, double mouseY);
};

extern AppState currentState;

extern Boton2D botonJugar;
extern Boton2D botonConfig;
extern Boton2D botonCredits;
extern Boton2D botonBack;

extern std::vector<glm::vec3> posicionesCubos;
