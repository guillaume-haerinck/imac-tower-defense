# Imac Tower Defense Changelog

## 27.04.19

Service locator pattern.
Map zoom.
Construct graph from image.

## 26.04.19

Event dispatcher.

## 25.04.19

Show map on screen.

## 22.04.19

Factories are not to be used as singleton anymore.
Separation between factories for entities and factories for special components.
Start using services.

## 29.03.19

Debug drawing for physics.

## 24.03.19

Switch Bullet for Box2D, better fit for a 2D game.

## 23.03.19

Add FMOD lib to play sounds and Bullet lib for physics.

## 20.03.19

Fix several bugs related to sprite array rendering.
Switch back to ENTT in C++ 14 to get tag supports.

## 17.03.19

Change ECS library ENTT for EntityX. Now the project is compatible from C++ 11.
Use x86 architecture on Windows for larger audience.

## 16.03.19

ImGUI, NeosisGUI and logging system implementation.
Game class to init renderer content (the main.ccp will change a lot, so by moving these things away we prevent from breaking them).

## 09.03.19

Add OpenGL wrapper classes and start project structure.
Implement ECS library ENTT.
