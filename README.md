# Groundbreaker

Groundbreaker is a bomberbam-style game for the terminal. It is written in C and uses the SDL2 library.

## Installation
You will need to have SDL2 installed on your system. On Debian-based systems, you can install it with `sudo apt install libsdl2-dev`.

To compile the game, run `cmake .` and then `make`. This will create an executable called `groundbreaker`.


### Dependencies

- SDL2
- SDL2\_image
- SDL2\_ttf
- SDL2\_mixer
- SDL2\_gfx
- SDL2\_sound

To build the game you need theses packages:

make cmake gcc git build-essential libsdl2-dev libsdl2-2.0-0 libsdl2-gfx-dev libsdl2-gfx-1.0-0 libsdl2-image-dev libsdl2-image-2.0-0 libsdl2-ttf-dev libsdl2-ttf-2.0-0

A Unix-like system is required to build the game.
Windows is not supported yet.

### Usage

To run the game, simply run `./groundbreaker` in the directory where you compiled the game.


### Nextly

- [ ] Procedural generation
- [ ] Add a pause menu
- [ ] Inventory
- [ ] Better UI
- [ ] Object interaction
- [ ] Add enemies
- [ ] Add items
- [ ] Multiplayer
- [ ] Animation
- [ ] Music and sounds




