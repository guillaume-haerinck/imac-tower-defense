# Imac Tower Defense

An OpenGL 4.4 game made with Entity-Component-System design pattern.

This is a student project for the French engineering school [IMAC](https://www.ingenieur-imac.fr/). If you're a french-speaker, you might consider to have a look at the [post-mortem](https://github.com/guillaume-haerinck/imac-tower-defense/blob/master/doc/rapport.md) (it has a lot of images) inside the `doc` folder. 

The structure of this project is being improved in my next C++ game [Roll Goal](https://github.com/guillaume-haerinck/roll-goal/tree/master/sdl2). The main changes are : improved state machine with the pushdown automaton structure, the replacement of most events by singleton components, a render queue for the renderrer and an more elaborate and maintenable structure for rendering (materials, layers, ...).

<p align="center">
<img src="https://github.com/guillaume-haerinck/imac-tower-defense/blob/master/doc/rapport-img/title.png?raw=true" alt="Logo Projet">
</p>

## Table of Contents

[**Features**](#features)

[**Getting Started**](#getting-started)
  * [**Prerequisites**](#prerequisites)
  * [**Linux**](#linux)
  * [**Windows**](#windows)
  * [**Build**](#build)
  * [**Launch**](#launch)

[**Documentation**](#documentation)
  * [**Philosophy**](#philosophy)
  * [**Folder structure**](#folder-structure)

**Additional info**
  * [**Dependencies**](#dependencies)
  * [**Inspired by**](#inspired-by)

<p align="center">
<img src="https://github.com/guillaume-haerinck/imac-tower-defense/blob/master/doc/rapport-img/v3.gif?raw=true" alt="SpriteSheet">
</p>

## Features

### To-do list

- [x] Integrate ECS system
- [x] Load & play spritesheet
- [x] Load, validate and interpret .itd files
- [x] Load map as a grid
- [x] User interface
- [x] Play audio
- [x] Object picking
- [x] Construction system
- [x] Monster path following
- [x] Attack system
- [x] Projectiles
- [x] Waves system
- [x] Banking system
- [x] Switch levels
- [x] Attack with raycasting
- [x] Mirror towers
- [ ] Level design
- [ ] Tutorials

## Getting Started

### Prerequisites

You must install **[CMake](https://cmake.org/)** and a C++ compiler which handles **C++ 14**. More on the compiler on the next sections.

The project need your graphic card driver to be up to date, as it is using **OpenGL 4.4** core profile (released in 2013, so it should be fine).

#### Linux

The recommended compiler is g++, minimum version required is `g++ 6.1`, which is included by default on many linux distribution. You can update it with :

```bash
sudo apt-get install build-essential
```

You will also need the OpenGl and SDL2 packages.

```bash
sudo apt-get install libsdl2-dev
sudo apt-get install libglu1-mesa-dev mesa-common-dev
```

#### Windows

The recommended compiler is MSVC. To install it you will need [Visual Studio](https://visualstudio.microsoft.com/fr/) (which is not VSCode) and select the C++ development package during installation. The [MingW](http://www.mingw.org/) compiler will work as well if you prefer to use it.

![Screenshot](doc/readme-img/vs-studio.png?raw=true "Visual studio packages selection")

### Build

The `CMakeLists.txt` file ensure that the project will work on Linux and Windows platforms.

You can build and launch the project in many different ways, we covered a few of them :

#### `Option 1: Bash (Linux only)`

Go to this folder with a terminal, and enter those commands.

```bash
cmake .
make
./bin/imac-tower-defense
```

It means that you are genererating a Makefile, then using it to build the project, and finally running the binaries from the current working directory (needed to find assets).

#### `Option 2: Visual Studio (Windows only)`

From Visual Studio 2017, you don't need to generate a visual studio project with cmake. You can simply open visual studio, browse the toolbar to *File->Open->Cmake...* and select the `CMakeLists.txt` at the root of this directory. From this point, you have to make sure that you are using a **x86 build configuration**, and running the **imac-tower-defense target**. If everything is done, simply press `f5` or click on the green arrow at the top.

![Screenshot](doc/readme-img/vs-studio-launch.png?raw=true "VS launch")

If you add files, you will need to re-generate the cache for a successfull build. To do so, right click on `CMakeLists.txt` and use `Generate Cache for imac-tower-defense`

![Screenshot](doc/readme-img/vs-studio-cache.png?raw=true "VS cache")

#### `Option 3: VSCode`

You will need the `CMakeTools` extension from vector-of-bools. If you want to change the source code, I highly recommend using the `C/C++` extension from Microsoft and the `CMake` extension from twxs.

![Screenshot](doc/readme-img/vs-code-extensions.png?raw=true "VSCode extensions")

From this point, when you will open the repository folder with VSCode, it will prompt you to configure the projet. You must select a x86 kit, for exemple the `Visual Studio Community 2017 - x86` kit. Once the configuration is done, you can compile the project by pressing `f7`. The .exe file will be outputed in `build/Debug` folder. You can launch it by pressing `f5`, but make sure to pick your operating system.

![Screenshot](doc/readme-img/vs-code-launch.png?raw=true "VSCode Launch options")

### Launch

Before launching the compiled binaries, **make sure that the working directory is set to this folder**, or the shaders files and images will not be found by the program. If you are unsure about how to do it, just copy the builded files to this folder before launching them. (by default, they will be in the `bin/Debug` folder).

## Documentation

### Philosophy

The project follows the **Entity Component System** design pattern. It has been made possible with the great [ENTT](https://github.com/skypjack/entt) library.

Our game is concieved as a state machine, and works like this architecture :

<p align="center">
<img src="https://github.com/guillaume-haerinck/imac-tower-defense/blob/master/doc/rapport-img/architecture-general.JPG?raw=true" alt="Event publisher">
</p>

### Folder structure

| Folder					| Description       |
| -------------				| ----------------- |
| **src**					| **Source code**   |
| `main.cpp`				| Entry point of the game |
| `components`				| Struct of data for entities |
| `component-factories`		| Creation of complex components |
| `entity-factories`		| Classes used to build entities |
| `events`					| Struct of data defining event types |
| `graphics`				| Wrapper classes for OpenGL objects |
| `gui`						| User interface logic |
| `logger`					| Error and log handlers for the game |
| `services`				| Functionalities needed anywhere, like playing a sound |
| `systems`					| Update the game and the entities based on their components (all the game logic is there) |
| **core**					| **Constants, Game state machine and map loading** |
| `game-states`				| The different states of the game (title menu, in-level, game over, ...) |
| `level`				    | Read .ITD and .PPM files, and construct a graph from them |
| **res**					| **Ressources loaded at runtime** |
| `audio`					| Musics and sound effects used accross the game |
| `fonts`					| Fonts used accross the game |
| `gui`						| XAML files describing the view for each user interface |
| `images`					| Images and spritesheets used by the game |
| `levels`					| .ITD and .PPM describing the levels of the game |
| `shaders`					| GLSL shaders used by openGL for rendering |
| **doc**					| **Documentation** |
| **lib**					| **Dependencies** |

## Dependencies

- [OpenGL](http://docs.gl/) - The graphic API used
- [GLM](https://glm.g-truc.net/) - The opengl maths library
- [SDL2](https://wiki.libsdl.org/) - The library used to create a valid OpenGl context
- [ENTT](https://github.com/skypjack/entt) - The ECS framework
- [SPDLog](https://github.com/gabime/spdlog) - Logging lib
- [NeosisGUI](https://www.noesisengine.com/) - GUI WPF lib for end users
- [Imgui](https://github.com/ocornut/imgui) - Immediate mode GUI lib for debugging
- [Debugbreak](https://github.com/scottt/debugbreak) - Cross-platform code breakpoint header
- [StbImage](https://github.com/nothings/stb) - Load images
- [FMOD](https://www.fmod.com/) - Load and play adaptative audio
- [Box2D](https://box2d.org/) - 2D Physics library

## Useful articles

- [Awesome Opengl](https://github.com/eug/awesome-opengl) - A list of useful tools for OpenGl
- [Awesome Readme](https://github.com/matiassingers/awesome-readme) - A list of great readme files
- [Pathfinding](https://www.redblobgames.com/pathfinding/tower-defense/) - Red blob games's article on pathfinding

## Inspired by

- [tddod](https://github.com/Daivuk/tddod) - Data oriented tower defense using ENTT library
- [Unity tower defense template](https://unity3d.com/learn/tutorials/s/tower-defense-template) - Tower defense game made with Unity engine
- [Unreal engine tower defense template](https://docs.unrealengine.com/en-us/Resources/SampleGames/StrategyGame) - Tower defense game made with Unreal Engine
- [RayWenderlich TD](https://www.raywenderlich.com/2709-how-to-make-a-tower-defense-game-tutorial) - Tower defense game made in Objective C
- [ENTT Pacman](https://github.com/Kerndog73/EnTT-Pacman) - Pacman game using ENTT library

## Authors

- **Jules Fouchy** - Processing star

- **Cyrielle Lassarre** - Designer of the worlds

- **Guillaume Haerinck** - Coding maniac
