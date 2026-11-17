#pragma once

#include <SDL2/SDL.h>
#include <algorithm>
#include <limits>

class Timer {
private:
    Uint32 lastTime;
    Uint32 duration;
    float accumulated;
    float multiplier;
    bool paused;

    float currentAccumulated() const {
        if (paused) return accumulated;
        Uint32 now = SDL_GetTicks();
        float total = accumulated + static_cast<float>(now - lastTime) * multiplier;
        return std::min(total, static_cast<float>(duration));
    }

public:
    Timer(Uint32 d = 0, bool startImmediately = true)
        : lastTime(SDL_GetTicks()),
          duration(d),
          accumulated(startImmediately ? 0.0f : static_cast<float>(d)),
          multiplier(1.0f),
          paused(false) {}

    void setMultiplier(float newMultiplier) {
        accumulated = currentAccumulated();
        lastTime = SDL_GetTicks();
        multiplier = std::max(0.0f, newMultiplier);
    }

    float getMultiplier() const {
        return multiplier;
    }

    void setDuration(Uint32 newDuration) {
        accumulated = currentAccumulated();
        lastTime = SDL_GetTicks();
        duration = newDuration;
        accumulated = std::min(accumulated, static_cast<float>(duration));
    }

    Uint32 getDuration() const {
        return duration;
    }

    Uint32 elapsedTime() const {
        return static_cast<Uint32>(currentAccumulated());
    }

    Uint32 remainingBaseTime() const {
        return duration - elapsedTime();
    }

    Uint32 remainingTime() const {
        float rem = static_cast<float>(duration) - currentAccumulated();
        if (rem <= 0.0f) return 0;
        if (paused || multiplier <= 0.0f) return std::numeric_limits<Uint32>::max();
        return static_cast<Uint32>(rem / multiplier);
    }

    float progress() const {
        if (duration == 0) return 1.0f;
        return currentAccumulated() / static_cast<float>(duration);
    }

    bool isRunning() const {
        return currentAccumulated() < static_cast<float>(duration);
    }

    bool isPaused() const {
        return paused;
    }

    void pause() {
        if (!paused) {
            accumulated = currentAccumulated();
            paused = true;
        }
    }

    void resume() {
        if (paused) {
            lastTime = SDL_GetTicks();
            paused = false;
        }
    }

    void reset() {
        lastTime = SDL_GetTicks();
        accumulated = 0.0f;
        paused = false;
    }

    void reset(Uint32 newDuration) {
        duration = newDuration;
        reset();
    }

    bool ifReset() {
        if (isRunning()) return false;
        reset();
        return true;
    }

    void finish() {
        accumulated = static_cast<float>(duration);
        lastTime = SDL_GetTicks();
    }
};