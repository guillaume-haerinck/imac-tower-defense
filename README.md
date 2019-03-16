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
- [ ] Construction system
- [ ] Monster path following
- [ ] Attack system
- [ ] Waves system
- [ ] Monster state machine
- [ ] Banking system
- [ ] Upgrade system

## Getting Started

### Prerequisites

You must install **[CMake](https://cmake.org/)** and a C++ compiler for at least **C++ 17**. More on the compiler on the next sections.

The project need your graphic card driver to be up to date, as it is using **OpenGL 4.4** core profile (released in 2013, so it should be fine). The dependencies are targeted for **x64 processor**, so make sure yours is (99% chance it is) or you won't be able to compile the project. If you really want to use a x86 architecture, download SDL2 and Noesis Gui libs for x86 and alter the CMakeLists.txt to link against them.

#### Linux

The recommended compiler is g++, which is included by default on many linux distribution. You can update it with :

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

The `CMakeLists.txt` file ensure that the project will work on Linux and Windows plateforms.

You can build and launch the project in many different ways, we covered a few of them :

#### `Option 1: Bash (Linux only)`

TODO

#### `Option 2: Visual Studio (Windows only)`

TODO

#### `Option 3: VSCode`

You will need the `CMakeTools` extension from vector-of-bools. If you want to change the source code, I highly recommend using the `C/C++` extension from Microsoft and the `CMake` extension from twxs.

![Screenshot](doc/readme-img/extensions.png?raw=true "VSCode extensions")

From this point, when you will open the repository folder with VSCode, it will prompt you to configure the projet. You must select the `Visual Studio Community 2017 - amd64` kit. Once the configuration is done, you can compile the project by pressing `f7`. The .exe file will be outputed in `build/Debug` folder. You can launch them by pressing `f5`.

#### `Option 4: CLion`

TODO

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
| `gui`         | User interface for end-users |
| `logger` |    | Error and log handler for the game |
| `systems`     | Update the game and the entities based on their components |
| **data**      | **Game levels description** |
| **doc**       | **Documentation** |
| **images**    | **Non-UI images used by the game** |
| **shaders**   | **GLSL shaders used by openGL for rendering** |

### Code reference

The reference for the code is built with [doxygen](http://www.doxygen.nl/).

## Dependencies

- [OpenGL](http://docs.gl/) - The graphic API used
- [GLM](https://glm.g-truc.net/0.9.9/index.html) - The opengl maths library
- [SDL2](https://wiki.libsdl.org/) - The library used to create a valid OpenGl context
- [EnTT](https://github.com/skypjack/entt/wiki/Crash-Course:-entity-component-system) - The ECS framework
- [SPDLog](https://github.com/gabime/spdlog) - Logging lib
- [NeosisGUI](https://www.noesisengine.com/docs/Gui.Core.Index.html) - GUI WPF lib for end users
- [Imgui](https://github.com/ocornut/imgui) - Immediate mode GUI lib for debugging
- [Debugbreak](https://github.com/scottt/debugbreak) - Cross-platform code breakpoint header
- [StbImage](https://github.com/nothings/stb) - Load images

## Useful articles

- [Awesome Opengl](https://github.com/eug/awesome-opengl) - A list of useful tools for OpenGl
- [Awesome Readme](https://github.com/matiassingers/awesome-readme) - A list of great readme files

## Inspired by

- [tddod](https://github.com/Daivuk/tddod) - Data oriented tower defense

## Authors

- **Jules Fouchy** - Processing star

- **Cyrielle Lassarre** - Designer of the worlds

- **Guillaume Haerinck** - Coding maniac
