//
// Created by Nick Vazquez on 9/12/24.
//

#ifndef PIXELCONTROLLER_EFFECT_H
#define PIXELCONTROLLER_EFFECT_H

#include <string>
#include "../../include/json.hpp"
#include "Spatials.h"

struct Effect {
    int id;
    std::string name;
    Pair origin;
    Pair size;
    double startTimeMs;
    double durationMs;
    Translation *translation;

    virtual ~Effect() = default;

    static Effect *from_json(const nlohmann::json &j) {
        auto *effect = new Effect();
        effect->id = j["id"];
        effect->name = j["name"];
        effect->origin = Pair::from_json(j["origin"]);
        effect->size = Pair::from_json(j["size"]);
        effect->startTimeMs = j["startTimeMs"];
        effect->durationMs = j["durationMs"];

        Translation *translation = nullptr;
        if (j.contains("translation")) {
            translation = Translation::from_json(j["translation"]);
        }
        effect->translation = translation;

        return effect;
    };
};

struct RainbowEffect : public Effect {
    std::vector<std::string> colors;
    double speed;

    static RainbowEffect *from_json(const nlohmann::json &j) {
        auto *effect = new RainbowEffect();
        effect->id = j["id"];
        effect->name = j["name"];
        effect->origin = Pair::from_json(j["origin"]);
        effect->size = Pair::from_json(j["size"]);
        effect->startTimeMs = j["startTimeMs"];
        effect->durationMs = j["durationMs"];

        Translation *translation = nullptr;
        if (j.contains("translation")) {
            translation = Translation::from_json(j["translation"]);
        }
        effect->translation = translation;

        for (const auto &color: j["colors"]) {
            effect->colors.push_back(color);
        }

        effect->speed = j["speed"];
        return effect;
    }
};

struct RippleEffect : public Effect {
    Pair ripple_origin;
    int speed;

    static RippleEffect *from_json(const nlohmann::json &j) {
        auto *effect = new RippleEffect();
        effect->id = j["id"];
        effect->name = j["name"];
        effect->origin = Pair::from_json(j["origin"]);
        effect->size = Pair::from_json(j["size"]);
        effect->startTimeMs = j["startTimeMs"];
        effect->durationMs = j["durationMs"];
        effect->speed = j["speed"];

        Translation *translation = nullptr;
        if (j.contains("translation")) {
            translation = Translation::from_json(j["translation"]);
        }

        effect->translation = translation;
        effect->ripple_origin = Pair::from_json(j["ripple_origin"]);
        return effect;
    }
};

#endif //PIXELCONTROLLER_EFFECT_H
