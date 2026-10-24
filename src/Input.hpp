#pragma once

#include <cstdint>

//MovementDirection
enum class MovementDirection : std::uint8_t {
	STILL    = 0,
	BACKWARD = 1 << 0,
	FORWARD  = 1 << 1,

    BRAKE = BACKWARD | FORWARD
};

constexpr MovementDirection operator|(MovementDirection a, MovementDirection b) {
    return static_cast<MovementDirection>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

constexpr MovementDirection operator&(MovementDirection a, MovementDirection b) {
    return static_cast<MovementDirection>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
}

constexpr MovementDirection operator~(MovementDirection a) {
    return static_cast<MovementDirection>(~static_cast<std::uint8_t>(a));
}

//TurnDirection
enum class TurnDirection : std::uint8_t {
	STRAIGHT = 0,
	LEFT     = 1 << 0,
	RIGHT    = 1 << 1,

    STRAIGHT = LEFT | RIGHT
};

constexpr TurnDirection operator|(TurnDirection a, TurnDirection b) {
    return static_cast<TurnDirection>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

constexpr TurnDirection operator&(TurnDirection a, TurnDirection b) {
    return static_cast<TurnDirection>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
}

constexpr TurnDirection operator~(TurnDirection a) {
    return static_cast<TurnDirection>(~static_cast<std::uint8_t>(a));
}