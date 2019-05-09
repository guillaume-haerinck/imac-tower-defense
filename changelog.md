# Imac Tower Defense Changelog

## 09.05.19

Fix deepth buffer.

## 08.05.19

Input events are handled separatly for clear separation between game screens.
Mirror factory and limit rotation of tower by a few axes.

## 07.05.19

Towers are shooting lasers. Lasers can be reflected by mirrors.

## 05.05.19

Handles event unsubcription (deal with connection invalid constructor by making a copy of instanciated object in memory).
Bank.

## 04.05.19

Trigger collision detection.
Change map at runtime.
Game class as a basic state machine (circular inclusion problem encountered, and resolved via events and dependency injection)

## 01.05.19

Health bar.

## 28.04.19

Pathfinding.
Explosions on enemy destroy.

## 27.04.19

Service locator pattern.
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
