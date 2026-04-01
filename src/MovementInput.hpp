#pragma once

//MovementDirection
enum class MovementDirection : int {
	BACKWARD = -1,
	STILL = 0,
	FORWARD = 1
};

inline MovementDirection& operator+=(MovementDirection& md, int delta) {
    int v = static_cast<int>(md) + delta;

    if (v < static_cast<int>(MovementDirection::BACKWARD))
		v = static_cast<int>(MovementDirection::BACKWARD);
    if (v > static_cast<int>(MovementDirection::FORWARD)) 
		v = static_cast<int>(MovementDirection::FORWARD);

    md = static_cast<MovementDirection>(v);
    return md;
}

inline MovementDirection& operator-=(MovementDirection& md, int delta) {
	return md += -delta;
}

//TurnDirection
enum class TurnDirection : int {
	RIGHT = -1,
	STRAIGHT = 0,
	LEFT = 1
};

inline TurnDirection& operator+=(TurnDirection& td, int delta) {
    int v = static_cast<int>(td) + delta;

    if (v < static_cast<int>(TurnDirection::RIGHT))
		v = static_cast<int>(TurnDirection::RIGHT);
    if (v > static_cast<int>(TurnDirection::LEFT))
		v = static_cast<int>(TurnDirection::LEFT);

    td = static_cast<TurnDirection>(v);
    return td;
}

inline TurnDirection& operator-=(TurnDirection& td, int delta) {
	return td += -delta;
}
