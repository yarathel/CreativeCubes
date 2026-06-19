#include "Globals.h"

AppState currentState = MENU;

// --- BOTONES DEL MENÚ PRINCIPAL ---
Boton2D botonJugar =
{
    355.0f,
    300.0f,
    250.0f,
    150.0f
};

Boton2D botonConfig =
{
    355.0f,
    180.0f,
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

Boton2D botonSalir =
{
    340.0f,
    -35.0f,  
    280.0f,
    180.0f  
};

Boton2D botonBack =
{
    30.0f,
    50.0f,
    200.0f,
    100.0f
};

// --- NUEVOS BOTONES DEL MENÚ DE PAUSA ---
Boton2D botonResume =
{
    344.0f,
    350.0f, 
    290.0f,
    260.0f
};

Boton2D botonRules =
{
    345.0f,
    180.0f, 
    290.0f,
    260.0f
};

Boton2D botonMainMenu =
{
    350.0f,
    10.0f, 
    280.0f,
    260.0f
};

Boton2D tituloPausa =
{
    350.0f,
    460.0f,
    280.0f,
    260.0f
};
std::vector<glm::vec3> posicionesCubos;

// --- DETECCIÓN DE CLICS EN LOS BOTONES ---
bool Boton2D::estaPresionado(double mouseX, double mouseY)
{
    return (
        mouseX >= x &&
        mouseX <= x + ancho &&
        mouseY >= y &&
        mouseY <= y + alto
        );
}
