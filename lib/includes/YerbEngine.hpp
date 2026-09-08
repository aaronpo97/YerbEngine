#pragma once

/**
 * @file YerbEngine.hpp
 * @brief Master include file for the YerbEngine game engine
 */

#include <AssetManagement/AudioSampleBuffer.hpp>
#include <AssetManagement/FontManager.hpp>
#include <AssetManagement/TextureManager.hpp>
#include <Configuration/ConfigAdapter.hpp>
#include <Configuration/ConfigDictionary.hpp>
#include <Configuration/ConfigStore.hpp>
#include <EntityManagement/Components.hpp>
#include <EntityManagement/Entity.hpp>
#include <EntityManagement/EntityManager.hpp>
#include <GameEngine/Action.hpp>
#include <GameEngine/GameEngine.hpp>
#include <GameScenes/Scene.hpp>
#include <Helpers/CollisionHelpers.hpp>
#include <Helpers/EntityHelpers.hpp>
#include <Helpers/MathHelpers.hpp>
#include <Helpers/SpawnHelpers.hpp>
#include <Helpers/TextHelpers.hpp>
#include <Helpers/Vec2.hpp>
#include <SystemManagement/AudioManager.hpp>
#include <SystemManagement/VideoManager.hpp>
#include <Systems/RenderSystem.hpp>
