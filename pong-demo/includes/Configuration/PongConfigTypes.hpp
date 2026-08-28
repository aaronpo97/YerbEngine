#pragma once
#include <SDL.h>

struct ShapeConfig {
   float height;
   float width;
   SDL_Color color;

   ShapeConfig(float const height, float const width, SDL_Color const color)
       : height(height), width(width), color(color) {}
};

struct MatchConfig {
   int pointsToWin;
   Uint64 serveDelay;
};

struct PaddleConfig {
   float speed;
   float wallOffset;
   ShapeConfig shape;
};

struct BallConfig {
   float baseSpeed;
   float speedIncrement;
   float maxSpeed;
   float maxBounceAngle;
   ShapeConfig shape;
};

struct OpponentConfig {
   float speed;
   float deadZone;
   float reactionDistance;
};
