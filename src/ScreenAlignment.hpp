#pragma once

#include <cstdint>

enum class ScreenAlignment : std::uint8_t {
    NONE    =      0,
    BOTTOM  = 1 << 0,
    VCENTER = 1 << 1,
    TOP     = 1 << 2,
    LEFT    = 1 << 3,
    HCENTER = 1 << 4,
    RIGHT   = 1 << 5,

    HORIZONTAL = LEFT   | HCENTER | RIGHT,
    VERTICAL   = BOTTOM | VCENTER | TOP,

    BOTTOM_LEFT   = BOTTOM  | LEFT,
    BOTTOM_CENTER = BOTTOM  | HCENTER,
    BOTTOM_RIGHT  = BOTTOM  | RIGHT,
    CENTER_LEFT   = VCENTER | LEFT,
    CENTER        = VCENTER | HCENTER,
    CENTER_RIGHT  = VCENTER | RIGHT,
    TOP_LEFT      = TOP     | LEFT,
    TOP_CENTER    = TOP     | HCENTER,
    TOP_RIGHT     = TOP     | RIGHT
};

constexpr ScreenAlignment operator|(ScreenAlignment a, ScreenAlignment b) {
    return static_cast<ScreenAlignment>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

constexpr ScreenAlignment operator&(ScreenAlignment a, ScreenAlignment b) {
    return static_cast<ScreenAlignment>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
}

constexpr ScreenAlignment operator~(ScreenAlignment a) {
    return static_cast<ScreenAlignment>(~static_cast<std::uint8_t>(a));
}