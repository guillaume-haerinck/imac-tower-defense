# Imac Tower Defense

An OpenGL 4.4 game made with Entity-Component-System design pattern.

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
  * [**Reference**](#code-reference)

**Additional info**
  * [**Dependencies**](#dependencies)
  * [**Inspired by**](#inspired-by)

## Features

### To-do list

To see full list, check this [Trello Board](https://trello.com/b/BauObFbb/imac-tower-defense)

- [x] Integrate ECS system
- [x] Load & play spritesheet
- [ ] Load, validate and interpret .itd files
- [ ] Load map as a grid
- [ ] User interface
- [x] Play audio
- [ ] Object picking
- [ ] Construction system
- [ ] Monster path following
- [ ] Attack system
- [ ] Projectiles
- [ ] Waves system
- [ ] Monster state machine
- [ ] Banking system
- [ ] Upgrade system

## Getting Started

### Prerequisites

You must install **[CMake](https://cmake.org/)** and a C++ compiler which handles **C++ 14**. More on the compiler on the next sections.

The project need your graphic card driver to be up to date, as it is using **OpenGL 4.4** core profile (released in 2013, so it should be fine).

#### Linux

The recommended compiler is g++, minimum version required is `g++ 6.1`, which is included by default on many linux distribution. You can update it with :

```bash
sudo apt-get install build-essential
```

You will also need OpenGl, SDL2 and Bullet physics packages.

```bash
sudo apt-get install libsdl2-dev
sudo apt-get install libglu1-mesa-dev mesa-common-dev
sudo apt-get install libbullet-dev
```

#### Windows

The recommended compiler is MSVC. To install it you will need [Visual Studio](https://visualstudio.microsoft.com/fr/) (which is not VSCode) and select the C++ development package during installation. The [MingW](http://www.mingw.org/) compiler will work as well if you prefer to use it.

![Screenshot](doc/readme-img/vs-studio.png?raw=true "Visual studio packages selection")

### Build

The `CMakeLists.txt` file ensure that the project will work on Linux and Windows plateforms.

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

From Visual Studio 2015, you don't need to generate a visual studio project with cmake. You can simply open visual studio, browse the toolbar to *File->Open->Cmake...* and select the `CMakeLists.txt` at the root of this directory. From this point, you have to make sure that you are using a **x86 build configuration**, and running the **imac-tower-defense target**. If everything is done, simply press `f5` or click on the green arrow at the top.

![Screenshot](doc/readme-img/vs-studio-launch.png?raw=true "VS launch")

#### `Option 3: VSCode`

You will need the `CMakeTools` extension from vector-of-bools. If you want to change the source code, I highly recommend using the `C/C++` extension from Microsoft and the `CMake` extension from twxs.

![Screenshot](doc/readme-img/vs-code-extensions.png?raw=true "VSCode extensions")

From this point, when you will open the repository folder with VSCode, it will prompt you to configure the projet. You must select the `Visual Studio Community 2017 - x86` kit. Once the configuration is done, you can compile the project by pressing `f7`. The .exe file will be outputed in `build/Debug` folder. You can launch it by pressing `f5`, but make sure to pick your operating system.

![Screenshot](doc/readme-img/vs-code-launch.png?raw=true "VSCode Launch options")

### Launch

Before launching the compiled binaries, **make sure that the working directory is set to this folder**, or the shaders files and images will not be found by the program. If you are unsure about how to do it, just copy the builded files to this folder before launching them. (by default, they will be in the `bin/Debug` folder).

## Documentation

### Philosohy

The project follows the **Entity Component System** design pattern.

### Folder structure

| Folder        | Description       |
| ------------- | ----------------- |
| **src**       | **Source code**   |
| `main.cpp`    | Entry point of the game |
| `components`  | Struct of data for entities |
| `core`        | Entry point delegation |
| `factories`   | Classes used to build entities |
| `graphics`    | Wrapper classes for OpenGL objects |
| `gui`         | User interface logic |
| `logger`      | Error and log handlers for the game |
| `systems`     | Update the game and the entities based on their components |
| **res**       | **Ressources loaded at runtime** |
| `audio`       | Musics and sound effects used accross the game |
| `fonts`       | Fonts used accross the game |
| `gui`         | XAML files describing the view for each user interface |
| `images`      | Images and spritesheets used by the game |
| `maps`        | .ITD and .PPM describing the levels of the game |
| `shaders`     | GLSL shaders used by openGL for rendering |
| **doc**       | **Documentation** |
| **lib**       | **Dependencies** |

### Code reference

The reference for the code is built with [doxygen](http://www.doxygen.nl/).

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
- [Bullet](https://pybullet.org/) - Physics library

## Useful articles

- [Awesome Opengl](https://github.com/eug/awesome-opengl) - A list of useful tools for OpenGl
- [Awesome Readme](https://github.com/matiassingers/awesome-readme) - A list of great readme files

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
