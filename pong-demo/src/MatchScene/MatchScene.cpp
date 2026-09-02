#include <Configuration/AudioIds.hpp>
#include <Helpers/PongTextHelpers.hpp>
#include <MatchScene/MatchScene.hpp>
#include <MenuScene/MenuScene.hpp>
#include <ResultScene/ResultScene.hpp>
#include <algorithm>
#include <cmath>

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
   auto const paddleHeight = static_cast<float>(cShape->rect.h);
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

MatchScene::MatchScene(GameEngine* gameEngine, MatchMode const mode)
    : Scene(gameEngine),
      m_config(gameEngine->GetConfig(), gameEngine->GetConfigStore("demo")),
      m_mode(mode) {
   spawnPaddles();
   spawnBall();
   m_courtSize = m_gameEngine->getVideoManager().getWindowSize();

   std::uniform_int_distribution<int> coinFlip(0, 1);
   serveBall(coinFlip(m_randomGenerator) == 0 ? -1 : 1);

   registerAction(SDLK_W, "LEFT_UP");
   registerAction(SDLK_S, "LEFT_DOWN");

   registerAction(SDLK_UP, "RIGHT_UP");
   registerAction(SDLK_DOWN, "RIGHT_DOWN");

   registerAction(SDLK_P, "PAUSE");
   registerAction(SDLK_BACKSPACE, "GO_BACK");
}

float MatchScene::courtScale() const {
   return m_gameEngine->getVideoManager().getWindowSize().y() /
          REFERENCE_COURT_HEIGHT;
}

void MatchScene::spawnPaddles() {
   PaddleConfig const paddleConfig = m_config.getPaddleConfig();
   Vec2 const windowSize = m_gameEngine->getVideoManager().getWindowSize();

   float const scale = courtScale();
   float const paddleWidth = paddleConfig.shape.width * scale;
   float const paddleHeight = paddleConfig.shape.height * scale;

   auto const spawnPaddle = [&](float const x) -> std::shared_ptr<Entity> {
      SDL_FRect const paddleRect{
          .x = 0,
          .y = 0,
          .w = static_cast<float>(paddleWidth),
          .h = static_cast<float>(paddleHeight),
      };

      Vec2 const position{x, (windowSize.y() - paddleHeight) / 2.0f};
      constexpr Vec2 velocity{0, 0};

      std::shared_ptr<Entity> const paddle =
          m_entities.addEntity(EntityTags::Player);
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
   BallConfig const ballConfig = m_config.getBallConfig();
   float const scale = courtScale();

   SDL_FRect const ballRect{
       .x = 0,
       .y = 0,
       .w = static_cast<float>(ballConfig.shape.width * scale),
       .h = static_cast<float>(ballConfig.shape.height * scale),
   };

   constexpr Vec2 position{0, 0};
   constexpr Vec2 velocity{0, 0};

   m_ball = m_entities.addEntity(EntityTags::Bullet);
   m_ball->setComponent(
       std::make_shared<Components::CTransform>(position, velocity));
   m_ball->setComponent(
       std::make_shared<Components::CShape>(ballRect, ballConfig.shape.color));

   m_entities.update();
}

void MatchScene::serveBall(int const direction) {
   BallConfig const ballConfig = m_config.getBallConfig();
   MatchConfig const matchConfig = m_config.getMatchConfig();
   Vec2 const windowSize = m_gameEngine->getVideoManager().getWindowSize();

   auto const& cTransform = m_ball->getComponent<Components::CTransform>();
   auto const& cShape = m_ball->getComponent<Components::CShape>();

   cTransform->topLeftCornerPos =
       windowSize / 2 - Vec2{static_cast<float>(cShape->rect.w) / 2.0f,
                             static_cast<float>(cShape->rect.h) / 2.0f};

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

   OpponentConfig const opponentConfig = m_config.getOpponentConfig();
   Vec2 const windowSize = m_gameEngine->getVideoManager().getWindowSize();

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
   PaddleConfig const paddleConfig = m_config.getPaddleConfig();
   OpponentConfig const opponentConfig = m_config.getOpponentConfig();
   Vec2 const windowSize = m_gameEngine->getVideoManager().getWindowSize();
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
   Vec2 const windowSize = m_gameEngine->getVideoManager().getWindowSize();
   AudioSampleBuffer& audioSampleBuffer = m_gameEngine->getAudioSampleBuffer();

   auto const& cTransform = m_ball->getComponent<Components::CTransform>();
   auto const& cShape = m_ball->getComponent<Components::CShape>();

   Vec2& position = cTransform->topLeftCornerPos;
   Vec2& velocity = cTransform->velocity;
   auto const ballWidth = static_cast<float>(cShape->rect.w);
   auto const ballHeight = static_cast<float>(cShape->rect.h);

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
   BallConfig const ballConfig = m_config.getBallConfig();

   auto const& ballTransform = m_ball->getComponent<Components::CTransform>();
   auto const& ballShape = m_ball->getComponent<Components::CShape>();
   auto const& paddleShape = paddle->getComponent<Components::CShape>();
   auto const& paddleTransform = paddle->getComponent<Components::CTransform>();

   float const paddleHalfHeight =
       static_cast<float>(paddleShape->rect.h) / 2.0f;
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
       isLeftPaddle
           ? paddlePosition.x() + static_cast<float>(paddleShape->rect.w)
           : paddlePosition.x() - static_cast<float>(ballShape->rect.w));

   m_gameEngine->getAudioSampleBuffer().queueSample(
       PongAudio::SAMPLE_PADDLE_HIT, PriorityLevel::IMPORTANT);
}

void MatchScene::scorePoint(bool const leftPlayerScored) {
   MatchConfig const matchConfig = m_config.getMatchConfig();

   if (leftPlayerScored) {
      m_leftScore += 1;
   } else {
      m_rightScore += 1;
   }

   m_gameEngine->getAudioSampleBuffer().queueSample(PongAudio::SAMPLE_SCORE,
                                                    PriorityLevel::CRITICAL);

   if (std::max(m_leftScore, m_rightScore) >= matchConfig.pointsToWin) {
      m_matchOver = true;
      m_endTriggered = true;
      return;
   }

   serveBall(leftPlayerScored ? 1 : -1);
}

void MatchScene::sDoAction(Action& action) {
   ActionState const& actionState = action.getState();
   bool const actionStateStart = actionState == ActionState::START;
   std::string const& actionName = action.getName();

   auto const& leftInput = m_leftPaddle->getComponent<Components::CInput>();

   if (actionName == "LEFT_UP") {
      leftInput->directions[Components::CInput::Forward] = actionStateStart;
   }
   if (actionName == "LEFT_DOWN") {
      leftInput->directions[Components::CInput::Backward] = actionStateStart;
   }

   if (m_mode == MatchMode::TwoPlayer) {
      auto const& rightInput =
          m_rightPaddle->getComponent<Components::CInput>();

      if (actionName == "RIGHT_UP") {
         rightInput->directions[Components::CInput::Forward] = actionStateStart;
      }
      if (actionName == "RIGHT_DOWN") {
         rightInput->directions[Components::CInput::Backward] =
             actionStateStart;
      }
   }

   if (!actionStateStart) {
      return;
   }

   AudioSampleBuffer& audioSampleBuffer = m_gameEngine->getAudioSampleBuffer();

   if (actionName == "PAUSE") {
      audioSampleBuffer.queueSample(PongAudio::SAMPLE_MENU_SELECT,
                                    PriorityLevel::CRITICAL);
      m_paused = !m_paused;

      if (!m_paused) {
         m_lastFrameTime = SDL_GetTicks();
      }
   }

   if (actionName == "GO_BACK") {
      audioSampleBuffer.queueSample(PongAudio::SAMPLE_MENU_SELECT,
                                    PriorityLevel::CRITICAL);
      m_endTriggered = true;
   }
}

void MatchScene::renderCourt() const {
   SDL_Renderer* renderer = m_gameEngine->getVideoManager().getRenderer();
   Vec2 const windowSize = m_gameEngine->getVideoManager().getWindowSize();

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
          .x = static_cast<float>(centerX),
          .y = static_cast<float>(y),
          .w = static_cast<float>(SEGMENT_WIDTH),
          .h = static_cast<float>(SEGMENT_HEIGHT),
      };
      SDL_RenderFillRect(renderer, &segment);
   }
}

void MatchScene::renderScores() const {
   SDL_Renderer* renderer = m_gameEngine->getVideoManager().getRenderer();
   TTF_Font* fontLg = m_gameEngine->getFontManager().getFontLg();
   Vec2 const windowSize = m_gameEngine->getVideoManager().getWindowSize();

   float const SCORE_Y = 30.0f * courtScale();

   PongTextHelpers::renderCenteredText(renderer, fontLg,
                                       std::to_string(m_leftScore), TEXT_COLOR,
                                       Vec2{windowSize.x() * 0.25f, SCORE_Y});

   PongTextHelpers::renderCenteredText(renderer, fontLg,
                                       std::to_string(m_rightScore), TEXT_COLOR,
                                       Vec2{windowSize.x() * 0.75f, SCORE_Y});
}

void MatchScene::renderBanner() {
   SDL_Renderer* renderer = m_gameEngine->getVideoManager().getRenderer();
   TTF_Font* fontMd = m_gameEngine->getFontManager().getFontMd();
   TTF_Font* fontSm = m_gameEngine->getFontManager().getFontSm();
   Vec2 const windowSize = m_gameEngine->getVideoManager().getWindowSize();

   if (m_paused) {
      PongTextHelpers::renderCenteredText(
          renderer, fontMd, "PAUSED", TEXT_COLOR,
          Vec2{windowSize.x() / 2.0f, windowSize.y() * 0.4f});
   } else if (SDL_GetTicks() < m_serveTime) {
      std::string const serveText =
          m_serveDirection < 0 ? "SERVING LEFT" : "SERVING RIGHT";
      PongTextHelpers::renderCenteredText(
          renderer, fontMd, serveText, TEXT_COLOR,
          Vec2{windowSize.x() / 2.0f, windowSize.y() * 0.4f});
   }

   MatchConfig const matchConfig = m_config.getMatchConfig();
   std::string const footerText = "First to " +
                                  std::to_string(matchConfig.pointsToWin) +
                                  "   |   P to pause, Backspace for menu";

   PongTextHelpers::renderCenteredText(
       renderer, fontSm, footerText, COURT_COLOR,
       Vec2{windowSize.x() / 2.0f, windowSize.y() - 40.0f * courtScale()});
}

void MatchScene::sRender() {
   SDL_Renderer* renderer = m_gameEngine->getVideoManager().getRenderer();
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   SDL_RenderClear(renderer);

   renderCourt();

   for (auto const& entity : m_entities.getEntities()) {
      auto const& cShape = entity->getComponent<Components::CShape>();
      auto const& cTransform = entity->getComponent<Components::CTransform>();

      if (cShape == nullptr || cTransform == nullptr) {
         continue;
      }

      SDL_FRect& rect = cShape->rect;
      Vec2 const& pos = cTransform->topLeftCornerPos;

      rect.x = static_cast<float>(pos.x());
      rect.y = static_cast<float>(pos.y());

      SDL_SetRenderDrawColor(renderer, cShape->color.r, cShape->color.g,
                             cShape->color.b, cShape->color.a);
      SDL_RenderFillRect(renderer, &rect);
   }

   renderScores();
   renderBanner();

   SDL_RenderPresent(renderer);
}

void MatchScene::onSceneWindowResize() {
   PaddleConfig const paddleConfig = m_config.getPaddleConfig();
   BallConfig const ballConfig = m_config.getBallConfig();
   Vec2 const windowSize = m_gameEngine->getVideoManager().getWindowSize();
   float const scale = courtScale();

   auto const resizeEntity = [](std::shared_ptr<Entity> const& entity,
                                float const width, float const height) {
      SDL_FRect& rect = entity->getComponent<Components::CShape>()->rect;
      rect.w = static_cast<float>(width);
      rect.h = static_cast<float>(height);
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
      m_gameEngine->LoadScene("Menu",
                              std::make_shared<MenuScene>(m_gameEngine));
      return;
   }

   m_gameEngine->LoadScene(
       "Result", std::make_shared<ResultScene>(m_gameEngine, m_mode,
                                               m_leftScore, m_rightScore));
}
