#ifndef AUDIO_CONTROLLER_H
#define AUDIO_CONTROLLER_H

#include <iostream>
#include <string>
#include <windows.h>
#include <mmsystem.h>

// Enlazamos la librería de sonido de Windows
#pragma comment(lib, "winmm.lib")

class AudioController {
public:
    // Constructor simple
    AudioController() : estaEncendido(true), archivoActual("") {}

    /**
     * @brief Inicia la reproducción de un archivo .wav
     * @param ruta Ruta del archivo de audio
     */
    void ReproducirPista(const std::string& ruta) {
        if (!estaEncendido) return;

        // Intentamos cargar el audio
        // SND_FILENAME: indica archivo, SND_ASYNC: no bloquea el programa, SND_LOOP: repite
        bool exito = PlaySoundA(ruta.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);

        if (exito) {
            archivoActual = ruta;
        }
        else {
            std::cerr << "Error: No se pudo reproducir el archivo: " << ruta << std::endl;
        }
    }

    /**
     * @brief Detiene cualquier sonido que se esté reproduciendo
     */
    void DetenerTodo() {
        PlaySoundA(NULL, NULL, 0);
        archivoActual = "";
    }

    /**
     * @brief Alterna el estado de mute/unmute
     */
    void AlternarSilencio() {
        estaEncendido = !estaEncendido;
        if (!estaEncendido) {
            DetenerTodo();
        }
        else if (!archivoActual.empty()) {
            ReproducirPista(archivoActual);
        }
    }

    bool EstaActivo() const { return estaEncendido; }

private:
    std::string archivoActual;
    bool estaEncendido;
};
#endif
