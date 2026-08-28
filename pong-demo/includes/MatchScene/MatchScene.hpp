#pragma once

#include <SDL.h>

#include <Configuration/PongConfigAdapter.hpp>
#include <YerbEngine.hpp>
using namespace YerbEngine;
#include <random>

enum class MatchMode { SinglePlayer, TwoPlayer };

class MatchScene final : public Scene {
   EntityManager m_entities;
   PongConfigAdapter m_config;
   MatchMode m_mode;

   std::shared_ptr<Entity> m_leftPaddle;
   std::shared_ptr<Entity> m_rightPaddle;
   std::shared_ptr<Entity> m_ball;

   std::random_device m_rd;
   std::mt19937 m_randomGenerator = std::mt19937(m_rd());

   Vec2 m_courtSize{0, 0};

   int m_leftScore = 0;
   int m_rightScore = 0;
   float m_ballSpeed = 0;
   Uint64 m_serveTime = 0;
   int m_serveDirection = -1;
   bool m_matchOver = false;

   float courtScale() const;

   void spawnPaddles();
   void spawnBall();
   void serveBall(int direction);
   void bounceOffPaddle(std::shared_ptr<Entity> const& paddle,
                        bool isLeftPaddle);
   void scorePoint(bool leftPlayerScored);

   void renderCourt() const;
   void renderScores() const;
   void renderBanner();

  public:
   MatchScene(GameEngine* gameEngine, MatchMode mode);

   void update() override;
   void onEnd() override;
   void sRender() override;
   void sDoAction(Action& action) override;
   void sAudio() override;
   void onSceneWindowResize() override;

   void sOpponent();
   void sMovement();
   void sCollision();
};
