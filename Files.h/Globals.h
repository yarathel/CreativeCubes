#pragma once
#include <vector>
#include <glm/glm.hpp>

enum AppState
{
    MENU,
    CONFIG,
    CREDITS,
    JUEGO,
    PAUSA       
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
extern Boton2D botonSalir;


extern Boton2D botonResume;
extern Boton2D botonRules;
extern Boton2D botonMainMenu;
extern Boton2D tituloPausa;
extern std::vector<glm::vec3> posicionesCubos;
