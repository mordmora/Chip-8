# Emulador CHIP-8

Este proyecto es un interprete del sistema CHIP-8 escrito en C. Utiliza la biblioteca SDL2 para la salida grafica y la entrada de teclado, y emplea la API de Windows (Win32) para ejecutar un hilo secundario de depuracion que muestra el estado de la CPU en la consola.

## Caracteristicas

- Implementacion del conjunto de instrucciones CHIP-8.
- Renderizado grafico mediante SDL2.
- Monitor de depuracion en tiempo real (PC, Stack, Registros) ejecutandose en un hilo separado.
- Soporte para carga de ROMs desde linea de comandos.

## Requisitos

- CMake (version 3.14 o superior)
- Compilador de C compatible con C11 (se recomienda MSVC en Windows)
- Git (para descargar SDL2 automaticamente)

## Compilacion

El proyecto utiliza CMake y FetchContent para gestionar la dependencia de SDL2, por lo que no es necesario instalarla manualmente.

1. Clone el repositorio o descargue el codigo fuente.
2. Abra una terminal en la raiz del proyecto.
3. Ejecute los siguientes comandos:

```bash
mkdir build
cd build
cmake ..
cmake --build .