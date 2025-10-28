#pragma once

/**
 * @file YerbEngine.hpp
 * @brief Master include file for the YerbEngine game engine
 *
 * This file provides a convenient single-header include for all
 * core YerbEngine components. Include this file to get access to
 * the entire engine API.
 *
 * Usage:
 *   #include <YerbEngine/YerbEngine.hpp>
 */

// ============================================================================
// Core Game Engine
// ============================================================================
#include <YerbEngine/GameEngine/Action.hpp>
#include <YerbEngine/GameEngine/GameEngine.hpp>

// ============================================================================
// Scene Management
// ============================================================================
#include <YerbEngine/GameScenes/Scene.hpp>

// ============================================================================
// Entity-Component-System (ECS)
// ============================================================================
#include <YerbEngine/EntityManagement/Components.hpp>
#include <YerbEngine/EntityManagement/Entity.hpp>
#include <YerbEngine/EntityManagement/EntityManager.hpp>

// ============================================================================
// Configuration System
// ============================================================================
#include <YerbEngine/Configuration/Config.hpp>
#include <YerbEngine/Configuration/ConfigManager.hpp>

// ============================================================================
// Asset Management
// ============================================================================
#include <YerbEngine/AssetManagement/AudioSampleQueue.hpp>
#include <YerbEngine/AssetManagement/FontManager.hpp>
#include <YerbEngine/AssetManagement/TextureManager.hpp>

// ============================================================================
// System Management (SDL Abstractions)
// ============================================================================
#include <YerbEngine/SystemManagement/AudioManager.hpp>
#include <YerbEngine/SystemManagement/VideoManager.hpp>

// ============================================================================
// Helper Utilities
// ============================================================================
#include <YerbEngine/Helpers/CollisionHelpers.hpp>
#include <YerbEngine/Helpers/EntityHelpers.hpp>
#include <YerbEngine/Helpers/MathHelpers.hpp>
#include <YerbEngine/Helpers/MovementHelpers.hpp>
#include <YerbEngine/Helpers/SpawnHelpers.hpp>
#include <YerbEngine/Helpers/TextHelpers.hpp>
#include <YerbEngine/Helpers/Vec2.hpp>