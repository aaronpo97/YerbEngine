#include <Configuration/AudioIds.hpp>
#include <EntityTags.hpp>
#include <MatchScene/MatchScene.hpp>
#include <MenuScene/MenuScene.hpp>
#include <ResultScene/ResultScene.hpp>
#include <algorithm>
#include <cmath>
#include <random>

using namespace YerbEngine;

namespace {
constexpr float BASE_MOVEMENT_MULTIPLIER = 50.0f;

constexpr float REFERENCE_COURT_HEIGHT = 720.0f;

constexpr SDL_Color COURT_COLOR = {.r = 70, .g = 70, .b = 70, .a = 255};
constexpr SDL_Color TEXT_COLOR = {.r = 235, .g = 235, .b = 235, .a = 255};

void clampPaddleToCourt(std::shared_ptr<Entity> const& paddle,
                        Vec2 const& windowSize) {
   auto const& cTransform = paddle->getComponent<Components::CTransform>();
   auto const& cShape = paddle->getComponent<Components::CShape>();

   Vec2& position = cTransform->topLeftCornerPos;
   auto const paddleHeight = cShape->rect.h;
   float const lowestY = windowSize.y() - paddleHeight;

   position.setY(std::clamp(position.y(), 0.0f, lowestY));
}

void movePaddle(std::shared_ptr<Entity> const& paddle, float const speed,
                float const deltaTime, Vec2 const& windowSize) {
   auto const& cTransform = paddle->getComponent<Components::CTransform>();
   auto const& cInput = paddle->getComponent<Components::CInput>();

   float direction = 0;
   if (cInput->directions[Components::CInput::Forward]) {
      direction -= 1;
   }
   if (cInput->directions[Components::CInput::Backward]) {
      direction += 1;
   }

   Vec2& position = cTransform->topLeftCornerPos;
   position.setY(position.y() +
                 direction * speed * deltaTime * BASE_MOVEMENT_MULTIPLIER);

   clampPaddleToCourt(paddle, windowSize);
}
}  // namespace

MatchScene::MatchScene(PongServices const& services, MatchMode const mode)
    : PongScene(services), m_mode(mode) {
   spawnPaddles();
   spawnBall();
   m_courtSize = m_services.video.getWindowSize();

   std::uniform_int_distribution<int> coinFlip(0, 1);
   serveBall(coinFlip(m_randomGenerator) == 0 ? -1 : 1);

   registerAction(SDLK_W, "LEFT_UP");
   registerAction(SDLK_S, "LEFT_DOWN");

   registerAction(SDLK_UP, "RIGHT_UP");
   registerAction(SDLK_DOWN, "RIGHT_DOWN");

   registerAction(SDLK_P, "PAUSE");
   registerAction(SDLK_BACKSPACE, "GO_BACK");

   bindPaddleControls();

   bindPress("PAUSE", [this](Action&) {
      m_services.audio.queueSample(PongAudio::SAMPLE_MENU_SELECT,
                                   PriorityLevel::CRITICAL);
      m_paused = !m_paused;

      if (!m_paused) {
         m_lastFrameTime = SDL_GetTicks();
      }
   });

   bindPress("GO_BACK", [this](Action&) {
      m_services.audio.queueSample(PongAudio::SAMPLE_MENU_SELECT,
                                   PriorityLevel::CRITICAL);
      m_endTriggered = true;
   });
}

void MatchScene::setPaddleDirection(
    std::shared_ptr<Entity> const& paddle,
    Components::CInput::Directions const direction, bool const pressed) {
   paddle->getComponent<Components::CInput>()->directions[direction] = pressed;
}

void MatchScene::bindPaddleControls() {
   bindHold("LEFT_UP", [this](bool const pressed) {
      setPaddleDirection(m_leftPaddle, Components::CInput::Forward, pressed);
   });
   bindHold("LEFT_DOWN", [this](bool const pressed) {
      setPaddleDirection(m_leftPaddle, Components::CInput::Backward, pressed);
   });

   // In single player the right paddle is driven by sOpponent(), so its keys
   // stay unbound.
   if (m_mode != MatchMode::TwoPlayer) {
      return;
   }

   bindHold("RIGHT_UP", [this](bool const pressed) {
      setPaddleDirection(m_rightPaddle, Components::CInput::Forward, pressed);
   });
   bindHold("RIGHT_DOWN", [this](bool const pressed) {
      setPaddleDirection(m_rightPaddle, Components::CInput::Backward, pressed);
   });
}

float MatchScene::courtScale() const {
   return m_services.video.getWindowSize().y() / REFERENCE_COURT_HEIGHT;
}

void MatchScene::spawnPaddles() {
   PaddleConfig const paddleConfig = m_pong.config.getPaddleConfig();
   Vec2 const windowSize = m_services.video.getWindowSize();

   float const scale = courtScale();
   float const paddleWidth = paddleConfig.shape.width * scale;
   float const paddleHeight = paddleConfig.shape.height * scale;

   auto const spawnPaddle = [&](float const x) -> std::shared_ptr<Entity> {
      SDL_FRect const paddleRect{
          .x = 0,
          .y = 0,
          .w = paddleWidth,
          .h = paddleHeight,
      };

      Vec2 const position{x, (windowSize.y() - paddleHeight) / 2.0f};
      constexpr Vec2 velocity{0, 0};

      std::shared_ptr<Entity> const paddle =
          m_entities.addEntity(PongTags::Paddle);
      paddle->setComponent(
          std::make_shared<Components::CTransform>(position, velocity));
      paddle->setComponent(std::make_shared<Components::CShape>(
          paddleRect, paddleConfig.shape.color));
      paddle->setComponent(std::make_shared<Components::CInput>());

      return paddle;
   };

   m_leftPaddle = spawnPaddle(paddleConfig.wallOffset * scale);
   m_rightPaddle = spawnPaddle(windowSize.x() -
                               paddleConfig.wallOffset * scale - paddleWidth);

   m_entities.update();
}

void MatchScene::spawnBall() {
   BallConfig const ballConfig = m_pong.config.getBallConfig();
   float const scale = courtScale();

   SDL_FRect const ballRect{
       .x = 0,
       .y = 0,
       .w = ballConfig.shape.width * scale,
       .h = ballConfig.shape.height * scale,
   };

   constexpr Vec2 position{0, 0};
   constexpr Vec2 velocity{0, 0};

   m_ball = m_entities.addEntity(PongTags::Ball);
   m_ball->setComponent(
       std::make_shared<Components::CTransform>(position, velocity));
   m_ball->setComponent(
       std::make_shared<Components::CShape>(ballRect, ballConfig.shape.color));

   m_entities.update();
}

void MatchScene::serveBall(int const direction) {
   BallConfig const ballConfig = m_pong.config.getBallConfig();
   MatchConfig const matchConfig = m_pong.config.getMatchConfig();
   Vec2 const windowSize = m_services.video.getWindowSize();

   auto const& cTransform = m_ball->getComponent<Components::CTransform>();
   auto const& cShape = m_ball->getComponent<Components::CShape>();

   cTransform->topLeftCornerPos =
       windowSize / 2 - Vec2{cShape->rect.w / 2.0f, cShape->rect.h / 2.0f};

   std::uniform_real_distribution<float> angleDistribution(
       -ballConfig.maxBounceAngle / 2.0f, ballConfig.maxBounceAngle / 2.0f);
   float const angle = angleDistribution(m_randomGenerator);

   cTransform->velocity =
       Vec2{std::cos(angle) * static_cast<float>(direction), std::sin(angle)};

   m_ballSpeed = ballConfig.baseSpeed;
   m_serveDirection = direction;
   m_serveTime = SDL_GetTicks() + matchConfig.serveDelay;
}

void MatchScene::update() {
   tick();

   if (!m_paused && !m_matchOver) {
      sOpponent();
      sMovement();
      sCollision();
   }

   sAudio();
   sRender();

   if (m_endTriggered) {
      onEnd();
   }
}

void MatchScene::sOpponent() {
   if (m_mode != MatchMode::SinglePlayer) {
      return;
   }

   OpponentConfig const opponentConfig = m_pong.config.getOpponentConfig();
   Vec2 const windowSize = m_services.video.getWindowSize();

   auto const& cInput = m_rightPaddle->getComponent<Components::CInput>();
   cInput->directions.reset();

   if (SDL_GetTicks() < m_serveTime) {
      return;
   }

   Vec2 const ballCenter = m_ball->getCenterPos();
   Vec2 const paddleCenter = m_rightPaddle->getCenterPos();
   Vec2 const ballVelocity =
       m_ball->getComponent<Components::CTransform>()->velocity;

   bool const ballApproaching = ballVelocity.x() > 0;
   bool const withinReach =
       ballCenter.x() >
       windowSize.x() * (1.0f - opponentConfig.reactionDistance);

   float const targetY =
       ballApproaching && withinReach ? ballCenter.y() : windowSize.y() / 2.0f;
   float const distance = targetY - paddleCenter.y();

   if (std::abs(distance) < opponentConfig.deadZone * courtScale()) {
      return;
   }

   cInput->directions[distance < 0 ? Components::CInput::Forward
                                   : Components::CInput::Backward] = true;
}

void MatchScene::sMovement() {
   PaddleConfig const paddleConfig = m_pong.config.getPaddleConfig();
   OpponentConfig const opponentConfig = m_pong.config.getOpponentConfig();
   Vec2 const windowSize = m_services.video.getWindowSize();
   float const scale = courtScale();

   movePaddle(m_leftPaddle, paddleConfig.speed * scale, m_deltaTime,
              windowSize);

   float const rightPaddleSpeed = m_mode == MatchMode::SinglePlayer
                                      ? opponentConfig.speed
                                      : paddleConfig.speed;
   movePaddle(m_rightPaddle, rightPaddleSpeed * scale, m_deltaTime, windowSize);

   if (SDL_GetTicks() < m_serveTime) {
      return;
   }

   auto const& cTransform = m_ball->getComponent<Components::CTransform>();
   cTransform->topLeftCornerPos +=
       cTransform->velocity *
       (m_ballSpeed * scale * m_deltaTime * BASE_MOVEMENT_MULTIPLIER);
}

void MatchScene::sCollision() {
   Vec2 const windowSize = m_services.video.getWindowSize();
   AudioSampleBuffer& audioSampleBuffer = m_services.audio;

   auto const& cTransform = m_ball->getComponent<Components::CTransform>();
   auto const& cShape = m_ball->getComponent<Components::CShape>();

   Vec2& position = cTransform->topLeftCornerPos;
   Vec2& velocity = cTransform->velocity;
   auto const ballWidth = cShape->rect.w;
   auto const ballHeight = cShape->rect.h;

   bool const hitTop = position.y() <= 0 && velocity.y() < 0;
   bool const hitBottom =
       position.y() + ballHeight >= windowSize.y() && velocity.y() > 0;

   if (hitTop || hitBottom) {
      position.setY(hitTop ? 0.0f : windowSize.y() - ballHeight);
      velocity.setY(-velocity.y());
      audioSampleBuffer.queueSample(PongAudio::SAMPLE_WALL_BOUNCE,
                                    PriorityLevel::STANDARD);
   }

   for (auto const& paddle : {m_leftPaddle, m_rightPaddle}) {
      if (!CollisionHelpers::calculateCollisionBetweenEntities(m_ball,
                                                               paddle)) {
         continue;
      }

      bool const isLeftPaddle = paddle == m_leftPaddle;
      bool const movingIntoPaddle =
          isLeftPaddle ? velocity.x() < 0 : velocity.x() > 0;

      if (!movingIntoPaddle) {
         continue;
      }

      bounceOffPaddle(paddle, isLeftPaddle);
   }

   if (position.x() + ballWidth < 0) {
      scorePoint(false);
   } else if (position.x() > windowSize.x()) {
      scorePoint(true);
   }
}

void MatchScene::bounceOffPaddle(std::shared_ptr<Entity> const& paddle,
                                 bool const isLeftPaddle) {
   BallConfig const ballConfig = m_pong.config.getBallConfig();

   auto const& ballTransform = m_ball->getComponent<Components::CTransform>();
   auto const& ballShape = m_ball->getComponent<Components::CShape>();
   auto const& paddleShape = paddle->getComponent<Components::CShape>();
   auto const& paddleTransform = paddle->getComponent<Components::CTransform>();

   float const paddleHalfHeight = paddleShape->rect.h / 2.0f;
   float const hitOffset =
       std::clamp((m_ball->getCenterPos().y() - paddle->getCenterPos().y()) /
                      paddleHalfHeight,
                  -1.0f, 1.0f);
   float const angle = hitOffset * ballConfig.maxBounceAngle;
   float const xDirection = isLeftPaddle ? 1.0f : -1.0f;

   ballTransform->velocity =
       Vec2{std::cos(angle) * xDirection, std::sin(angle)};

   m_ballSpeed =
       std::min(m_ballSpeed + ballConfig.speedIncrement, ballConfig.maxSpeed);

   Vec2 const& paddlePosition = paddleTransform->topLeftCornerPos;
   ballTransform->topLeftCornerPos.setX(
       isLeftPaddle ? paddlePosition.x() + paddleShape->rect.w
                    : paddlePosition.x() - ballShape->rect.w);

   m_services.audio.queueSample(PongAudio::SAMPLE_PADDLE_HIT,
                                PriorityLevel::IMPORTANT);
}

void MatchScene::scorePoint(bool const leftPlayerScored) {
   MatchConfig const matchConfig = m_pong.config.getMatchConfig();

   if (leftPlayerScored) {
      m_leftScore += 1;
   } else {
      m_rightScore += 1;
   }

   m_services.audio.queueSample(PongAudio::SAMPLE_SCORE,
                                PriorityLevel::CRITICAL);

   if (std::max(m_leftScore, m_rightScore) >= matchConfig.pointsToWin) {
      m_matchOver = true;
      m_endTriggered = true;
      return;
   }

   serveBall(leftPlayerScored ? 1 : -1);
}

void MatchScene::renderCourt() const {
   SDL_Renderer* renderer = m_services.video.getRenderer();
   Vec2 const windowSize = m_services.video.getWindowSize();

   float const scale = courtScale();
   float const SEGMENT_HEIGHT = 22.0f * scale;
   float const SEGMENT_GAP = 18.0f * scale;
   float const SEGMENT_WIDTH = 6.0f * scale;

   SDL_SetRenderDrawColor(renderer, COURT_COLOR.r, COURT_COLOR.g, COURT_COLOR.b,
                          COURT_COLOR.a);

   float const centerX = (windowSize.x() - SEGMENT_WIDTH) / 2.0f;

   for (float y = SEGMENT_GAP; y < windowSize.y();
        y += SEGMENT_HEIGHT + SEGMENT_GAP) {
      SDL_FRect const segment{
          .x = centerX,
          .y = y,
          .w = SEGMENT_WIDTH,
          .h = SEGMENT_HEIGHT,
      };
      SDL_RenderFillRect(renderer, &segment);
   }
}

void MatchScene::renderScores() const {
   SDL_Renderer* renderer = m_services.video.getRenderer();
   TTF_Font* fontLg = m_services.fonts.getFontLg();
   Vec2 const windowSize = m_services.video.getWindowSize();

   float const SCORE_Y = 30.0f * courtScale();

   TextHelpers::renderLineOfText(
       renderer, fontLg, std::to_string(m_leftScore), TEXT_COLOR,
       Vec2{windowSize.x() * 0.25f, SCORE_Y}, TextHelpers::TextAlign::Center);

   TextHelpers::renderLineOfText(
       renderer, fontLg, std::to_string(m_rightScore), TEXT_COLOR,
       Vec2{windowSize.x() * 0.75f, SCORE_Y}, TextHelpers::TextAlign::Center);
}

void MatchScene::renderBanner() {
   SDL_Renderer* renderer = m_services.video.getRenderer();
   TTF_Font* fontMd = m_services.fonts.getFontMd();
   TTF_Font* fontSm = m_services.fonts.getFontSm();
   Vec2 const windowSize = m_services.video.getWindowSize();

   if (m_paused) {
      TextHelpers::renderLineOfText(
          renderer, fontMd, "PAUSED", TEXT_COLOR,
          Vec2{windowSize.x() / 2.0f, windowSize.y() * 0.4f},
          TextHelpers::TextAlign::Center);
   } else if (SDL_GetTicks() < m_serveTime) {
      std::string const serveText =
          m_serveDirection < 0 ? "SERVING LEFT" : "SERVING RIGHT";
      TextHelpers::renderLineOfText(
          renderer, fontMd, serveText, TEXT_COLOR,
          Vec2{windowSize.x() / 2.0f, windowSize.y() * 0.4f},
          TextHelpers::TextAlign::Center);
   }

   MatchConfig const matchConfig = m_pong.config.getMatchConfig();
   std::string const footerText = "First to " +
                                  std::to_string(matchConfig.pointsToWin) +
                                  "   |   P to pause, Backspace for menu";

   TextHelpers::renderLineOfText(
       renderer, fontSm, footerText, COURT_COLOR,
       Vec2{windowSize.x() / 2.0f, windowSize.y() - 40.0f * courtScale()},
       TextHelpers::TextAlign::Center);
}

void MatchScene::sRender() {
   VideoManager& videoManager = m_services.video;

   RenderSystem::clear(videoManager);
   renderCourt();
   RenderSystem::renderEntities(m_entities, videoManager, m_services.textures);
   renderScores();
   renderBanner();
   RenderSystem::present(videoManager);
}

void MatchScene::onSceneWindowResize() {
   PaddleConfig const paddleConfig = m_pong.config.getPaddleConfig();
   BallConfig const ballConfig = m_pong.config.getBallConfig();
   Vec2 const windowSize = m_services.video.getWindowSize();
   float const scale = courtScale();

   auto const resizeEntity = [](std::shared_ptr<Entity> const& entity,
                                float const width, float const height) {
      SDL_FRect& rect = entity->getComponent<Components::CShape>()->rect;
      rect.w = width;
      rect.h = height;
   };

   bool const hasPreviousCourt = m_courtSize.y() > 0;
   float const leftFraction =
       hasPreviousCourt ? m_leftPaddle->getCenterPos().y() / m_courtSize.y()
                        : 0.5f;
   float const rightFraction =
       hasPreviousCourt ? m_rightPaddle->getCenterPos().y() / m_courtSize.y()
                        : 0.5f;
   Vec2 const ballFraction =
       hasPreviousCourt ? Vec2{m_ball->getCenterPos().x() / m_courtSize.x(),
                               m_ball->getCenterPos().y() / m_courtSize.y()}
                        : Vec2{0.5f, 0.5f};

   float const paddleWidth = paddleConfig.shape.width * scale;
   float const paddleHeight = paddleConfig.shape.height * scale;
   resizeEntity(m_leftPaddle, paddleWidth, paddleHeight);
   resizeEntity(m_rightPaddle, paddleWidth, paddleHeight);

   float const ballWidth = ballConfig.shape.width * scale;
   float const ballHeight = ballConfig.shape.height * scale;
   resizeEntity(m_ball, ballWidth, ballHeight);

   m_leftPaddle->getComponent<Components::CTransform>()->topLeftCornerPos =
       Vec2{paddleConfig.wallOffset * scale,
            leftFraction * windowSize.y() - paddleHeight / 2.0f};
   m_rightPaddle->getComponent<Components::CTransform>()->topLeftCornerPos =
       Vec2{windowSize.x() - paddleConfig.wallOffset * scale - paddleWidth,
            rightFraction * windowSize.y() - paddleHeight / 2.0f};

   clampPaddleToCourt(m_leftPaddle, windowSize);
   clampPaddleToCourt(m_rightPaddle, windowSize);

   m_ball->getComponent<Components::CTransform>()->topLeftCornerPos =
       Vec2{ballFraction.x() * windowSize.x() - ballWidth / 2.0f,
            ballFraction.y() * windowSize.y() - ballHeight / 2.0f};

   m_courtSize = windowSize;

   if (SDL_GetTicks() < m_serveTime) {
      serveBall(m_serveDirection);
   }
}

void MatchScene::onEnd() {
   if (!m_matchOver) {
      m_services.router.LoadScene("Menu", std::make_shared<MenuScene>(m_pong));
      return;
   }

   m_services.router.LoadScene(
       "Result", std::make_shared<ResultScene>(m_pong, m_mode, m_leftScore,
                                               m_rightScore));
}
