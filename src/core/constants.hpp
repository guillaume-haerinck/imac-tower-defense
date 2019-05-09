#pragma once

static const int WIN_WIDTH = 1920 / 2;
static const int WIN_HEIGHT = 1080 / 2;
static const float WIN_RATIO = (float) WIN_WIDTH / WIN_HEIGHT;
static const float PROJ_WIDTH = 100.0f;
static const float PROJ_WIDTH_RAT = PROJ_WIDTH * WIN_RATIO;
static const float PROJ_HEIGHT = 100.0f;
static const float TILE_SIZE = 11.0f;

static const int ENNEMY_MONEY_VALUE = 2;
static const float ENNEMY_HEALTH = 5.0f;

static const float TOWER_HEALTH = 10;
static const float TOWER_HITBOX_RADIUS = 4.0f;
static const float TOWER_ATTACK_RANGE = 25.0f;
static const int TOWER_COST = 10;
static const int MIRROR_COST = 10;
static const float MIRROR_RADIUS = TILE_SIZE / 2;

static const float LASER_DAMAGE_PER_SECOND = 2.0f;
static const float LASER_WIDTH = 15;
static const float PROJECTILE_HITBOX_RADIUS = 1.0f;

static const float TARGET_DELTA_MS = 1000.0f / 60.0f; // 60 FPS
static const float MAX_DELTA_MS = 1000.0f; // one second

extern int zIndexEnemy;
extern int zIndexTower;
extern int zIndexMap;
extern int zIndexExplosion;
extern int zIndexDebugDraw;
extern int zIndexHud;