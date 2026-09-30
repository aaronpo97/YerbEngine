#pragma once

#include <SDL3/SDL.h>

#include <Configuration/PongConfigAdapter.hpp>
#include <PongScene.hpp>
#include <YerbEngine.hpp>

enum class MatchMode { SinglePlayer, TwoPlayer };

class MatchScene final : public PongScene {
   using Entity = YerbEngine::Entity;
   using Vec2 = YerbEngine::Vec2;

   MatchMode m_mode;

   std::shared_ptr<Entity> m_leftPaddle;
   std::shared_ptr<Entity> m_rightPaddle;
   std::shared_ptr<Entity> m_ball;

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
   void bindPaddleControls();
   void setPaddleDirection(std::shared_ptr<Entity> const& paddle,
                           YerbEngine::Components::CInput::Directions direction,
                           bool pressed);
   void bounceOffPaddle(std::shared_ptr<Entity> const& paddle,
                        bool isLeftPaddle);
   void scorePoint(bool leftPlayerScored);

   void renderCourt() const;
   void renderScores() const;
   void renderBanner();

  public:
   MatchScene(PongServices const& services, MatchMode mode);

   void update() override;
   void onEnd() override;
   void sRender() override;
   void onSceneWindowResize() override;

   void sOpponent();
   void sMovement();
   void sCollision();
};
