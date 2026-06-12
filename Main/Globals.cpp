#include "Globals.h"

AppState currentState = MENU;

Boton2D botonJugar =
{
    355.0f,
    330.0f,
    250.0f,
    150.0f
};

Boton2D botonConfig =
{
    355.0f,
    200.0f,
    250.0f,
    160.0f
};

Boton2D botonCredits =
{
    355.0f,
    80.0f,
    250.0f,
    150.0f
};

Boton2D botonBack =
{
    30.0f,
    50.0f,
    200.0f,
    100.0f
};

std::vector<glm::vec3> posicionesCubos;

bool Boton2D::estaPresionado(double mouseX, double mouseY)
{
    return (
        mouseX >= x &&
        mouseX <= x + ancho &&
        mouseY >= y &&
        mouseY <= y + alto
    );
}
