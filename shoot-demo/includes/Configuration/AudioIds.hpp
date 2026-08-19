#pragma once

#include <string_view>

namespace DemoAudio {
    inline constexpr std::string_view TRACK_MAIN_MENU = "track_main_menu";
    inline constexpr std::string_view TRACK_PLAY      = "track_play";

    inline constexpr std::string_view SAMPLE_ITEM_ACQUIRED   = "sample_item_acquired";
    inline constexpr std::string_view SAMPLE_ENEMY_COLLISION = "sample_enemy_collision";
    inline constexpr std::string_view SAMPLE_SPEED_BOOST     = "sample_speed_boost";
    inline constexpr std::string_view SAMPLE_SLOWNESS_DEBUFF = "sample_slowness_debuff";
    inline constexpr std::string_view SAMPLE_MENU_MOVE       = "sample_menu_move";
    inline constexpr std::string_view SAMPLE_MENU_SELECT     = "sample_menu_select";
    inline constexpr std::string_view SAMPLE_SHOOT           = "sample_shoot";
    inline constexpr std::string_view SAMPLE_BULLET_HIT_01   = "sample_bullet_hit_01";
    inline constexpr std::string_view SAMPLE_BULLET_HIT_02   = "sample_bullet_hit_02";
} // namespace DemoAudio
