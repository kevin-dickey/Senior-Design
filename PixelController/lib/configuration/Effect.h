#ifndef PIXELCONTROLLER_EFFECT_H
#define PIXELCONTROLLER_EFFECT_H

#include <string>
#include <utility>
#include <vector>
#include "../../include/json.hpp"
#include "Spatials.h"

class Effect {
public:
    int id;
    std::string name;
    Pair *origin;
    Pair *size;
    double startTimeMs;
    double durationMs;
    Translation *translation;

    Effect(int id, std::string name,
           Pair *origin, Pair *size,
           double startTimeMs, double durationMs,
           Translation *translation
    ) {
        this->id = id;
        this->name = std::move(name);
        this->origin = origin;
        this->size = size;
        this->startTimeMs = startTimeMs;
        this->durationMs = durationMs;
        this->translation = translation;
    }

    virtual ~Effect() {
        delete translation;
    }

    static Effect *from_json(const nlohmann::json &j) {
        Translation *translation = nullptr;
        if (j.contains("translation")) {
            translation = Translation::from_json(j["translation"]);
        }

        return new Effect(
                j["id"],
                j["name"],
                Pair::from_json(j["origin"]),
                Pair::from_json(j["size"]),
                j["startTimeMs"],
                j["durationMs"],
                translation
        );
    }
};

class RainbowEffect : public Effect {
public:
    std::vector<std::string> *colors;
    double speed;

    RainbowEffect(
            int id, std::string name,
            Pair *origin, Pair *size,
            double startTimeMs, double durationMs,
            Translation *translation,
            std::vector<std::string> *colors,
            double speed
    ) : Effect(
            id, std::move(name), origin, size, startTimeMs, durationMs, translation
    ) {
        this->colors = colors;
        this->speed = speed;
    }

    static RainbowEffect *from_json(const nlohmann::json &j) {
        Translation *translation = nullptr;
        if (j.contains("translation")) {
            translation = Translation::from_json(j["translation"]);
        }

        auto *colors = new std::vector<std::string>();
        for (const auto &color : j["colors"]) {
            colors->push_back(color);
        }

        return new RainbowEffect(
                j["id"],
                j["name"],
                Pair::from_json(j["origin"]),
                Pair::from_json(j["size"]),
                j["startTimeMs"],
                j["durationMs"],
                translation,
                colors,
                j["speed"]
        );
    }
};

class RippleEffect : public Effect {
public:
    Pair *ripple_origin;
    int speed;

    RippleEffect(
            int id, std::string name,
            Pair *origin, Pair *size,
            double startTimeMs, double durationMs,
            Translation *translation,
            Pair *ripple_origin,
            int speed
    ) : Effect(
            id, std::move(name), origin, size, startTimeMs, durationMs, translation
    ) {
        this->ripple_origin = ripple_origin;
        this->speed = speed;
    }

    static RippleEffect *from_json(const nlohmann::json &j) {
        Translation *translation = nullptr;
        if (j.contains("translation")) {
            translation = Translation::from_json(j["translation"]);
        }

        return new RippleEffect(
                j["id"],
                j["name"],
                Pair::from_json(j["origin"]),
                Pair::from_json(j["size"]),
                j["startTimeMs"],
                j["durationMs"],
                translation,
                Pair::from_json(j["ripple_origin"]),
                j["speed"]
        );
    }
};

#endif // PIXELCONTROLLER_EFFECT_H