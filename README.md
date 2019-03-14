# Imac Tower Defense

An OpenGL 4.4 game made with Entity-Component-System design pattern.

## Getting Started

### Prerequisites

You must install **[CMake](https://cmake.org/)** and a C++ compiler for at least **C++ 17**. More on the compiler on the next sections.

The project need your graphic card driver to be up to date, as it is using **OpenGL 4.4** core profile (released in 2013, so it should be fine).

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

### Build & Launch

The `CMakeLists.txt` file ensure that the project will work on Linux and Windows plateforms. Before launching the compiled binaries, **make sure that the working directory is set to this folder**, or the shaders files and images will not be found by the program. If you are unsure about how to do it, just copy the builded files to this folder before launching them. (by default, they will be in the `bin/Debug` folder).

You can build and launch the projects in many different ways, we covered a few of them :

#### With Bash (Linux only)

TODO

#### With Visual Studio (Windows only)

TODO

#### With VSCode

You will need the `CMakeTools` extension from vector-of-bools. If you want to change the source code, I highly recommand using the `C/C++` extension coming from Microsoft and the `CMake` extension from twxs.

![Screenshot](doc/readme-img/extensions.png?raw=true "VSCode extensions")

From this point, when you will open the repository folder with VSCode, it will prompt you to configure the projet. You must select the `Visual Studio Community 2017 X86` kit. Once the configuration is done, you can compile the project by pressing `f7`.

![Screenshot](doc/readme-img/kit-selection.png?raw=true "Kit selection on VSCode")

The .exe file will be outputed in `build/Debug` folder. You can launch them by pressing `f5`.

#### With CLion

TODO

## Documentation

The github repository have Wiki pages.

The reference for the code is built with [doxygen](http://www.doxygen.nl/).

## Dependencies

- [OpenGL](http://docs.gl/) - The graphic API used
- [GLM](https://glm.g-truc.net/0.9.9/index.html) - The opengl maths library
- [SDL2](https://wiki.libsdl.org/) - The library used to create a valid OpenGl context
- [EnTT](https://github.com/skypjack/entt/wiki/Crash-Course:-entity-component-system) - The ECS framework
- [PPM](http://netpbm.sourceforge.net/doc/ppm.html) - The file format used to store data with images
- [Rang](https://github.com/agauniyal/rang) - Add color to terminal logs

## Usefull articles

- [Awesome Opengl](https://github.com/eug/awesome-opengl) - A list of usefull tools for OpenGl

## Authors

- **Jules Fouchy** - Processing star

- **Cyrielle Lassarre** - Designer of the worlds

- **Guillaume Haerinck** - Coding maniac
