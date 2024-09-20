#ifndef PIXELCONTROLLER_EFFECT_H
#define PIXELCONTROLLER_EFFECT_H

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "../../include/json.hpp"
#include "Spatials.h"

class Effect {
public:
    int id;
    std::string name;
    Pair_t origin;
    Pair_t size;
    double startTimeMs;
    double durationMs;
    std::unique_ptr<Translation_t> translation;

    inline Effect(int id, std::string name,
           Pair_t origin, Pair_t size,
           double startTimeMs, double durationMs,
           std::unique_ptr<Translation_t> translation
    ) {
        this->id = id;
        this->name = std::move(name);
        this->origin = origin;
        this->size = size;
        this->startTimeMs = startTimeMs;
        this->durationMs = durationMs;
        this->translation = std::move(translation);
    }

    virtual ~Effect() = default;

    static Effect *from_json(const nlohmann::json &j) {
        std::unique_ptr<Translation_t> translation = nullptr;

        if (j.contains("translation")) {
            std::unique_ptr<Translation_t>(new Translation_t::from_json(j["translation"]));
        }

        return new Effect(
                j["id"].get<int>(),
                j["name"].get<std::string>(),
                Pair_t::from_json(j["origin"]),
                Pair_t::from_json(j["size"]),
                j["startTimeMs"].get<double>(),
                j["durationMs"].get<double>(),
                std::move(translation)
        );
    }
};

class RainbowEffect : public Effect {
public:
    std::vector<std::string> colors;
    double speed;

    inline RainbowEffect(
            int id, std::string name,
            Pair_t origin, Pair_t size,
            double startTimeMs, double durationMs,
            std::unique_ptr<Translation_t> translation,
            std::vector<std::string> colors,
            double speed
    ) : Effect(
            id, std::move(name), origin, size, startTimeMs, durationMs, std::move(translation)
    ) {
        this->colors = colors;
        this->speed = speed;
    }

    static RainbowEffect *from_json(const nlohmann::json &j) {
        std::unique_ptr<Translation_t> translation = nullptr;
        
        if (j.contains("translation")) {
            std::unique_ptr<Translation_t> translation (new Translation_t::from_json(j["translation"]));
        }

        std::vector<std::string> colors;
        for (const auto &color : j["colors"]) {
            colors.push_back(color);
        }

        return new RainbowEffect(
                j["id"].get<int>(),
                j["name"].get<std::string>(),
                Pair_t::from_json(j["origin"]),
                Pair_t::from_json(j["size"]),
                j["startTimeMs"].get<double>(),
                j["durationMs"].get<double>(),
                std::move(translation),
                std::move(colors),
                j["speed"].get<int>()
        );
    }
};

class RippleEffect : public Effect {
public:
    Pair ripple_origin;
    int speed;

    inline RippleEffect(
            int id, std::string name,
            Pair_t origin, Pair_t size,
            double startTimeMs, double durationMs,
            std::unique_ptr<Translation_t> translation,
            Pair_t ripple_origin,
            int speed
    ) : Effect(
            id, std::move(name), origin, size, startTimeMs, durationMs, std::move(translation)
    ) {
        this->ripple_origin = ripple_origin;
        this->speed = speed;
    }

    static RippleEffect *from_json(const nlohmann::json &j) {
        std::unique_ptr<Translation_t> translation = nullptr;

        if (j.contains("translation")) {
            std::unique_ptr<Translation_t> translation (new Translation_t::from_json(j["translation"]));
        }

        return new RippleEffect(
                j["id"].get<int>(),
                j["name"].get<std::string>(),
                Pair_t::from_json(j["origin"]),
                Pair_t::from_json(j["size"]),
                j["startTimeMs"].get<double>(),
                j["durationMs"].get<double>(),
                std::move(translation),
                Pair_t::from_json(j["ripple_origin"]),
                j["speed"].get<int>()
        );
    }
};

#endif // PIXELCONTROLLER_EFFECT_H