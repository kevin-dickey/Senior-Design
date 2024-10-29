#ifndef PIXELCONTROLLER_EFFECT_H
#define PIXELCONTROLLER_EFFECT_H

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "../../include/json.hpp"
#include "Spatials.h"

class Effect
{
public:
    int id;
    std::string name;
    Pair_t origin{};
    Pair_t size{};
    double startTimeMs;
    double durationMs;
    std::unique_ptr<Translation_t> translation;

    inline Effect(int id,
                  EffectType type,
                  std::string name,
                  Pair_t origin, Pair_t size,
                  double startTimeMs, double durationMs,
                  std::unique_ptr<Translation_t> translation)
            : id(id), effectType(type), name(std::move(name)),
              origin(origin), size(size),
              startTimeMs(startTimeMs), durationMs(durationMs),
              translation(std::move(translation)) {}

    Effect(const Effect &) = delete;

    Effect &operator=(const Effect &) = delete;

    virtual ~Effect() = default;

    static Effect *from_json(const nlohmann::json &j) {
        try {
            std::unique_ptr<Translation_t> translation = nullptr;
            if (j.contains("translation")) {
                translation.reset(Translation_t::from_json(j["translation"]));
            }

            if (!j.contains("id")) { throw std::invalid_argument("Effect ID not found"); }
            if (!j.contains("type")) { throw std::invalid_argument("Effect type not found"); }

            // Parse the effect type input value
            EffectType parsedType;
            auto effectType = j["type"].get<std::string>();
            if (effectType == "rainbow") {
                parsedType = E_RAINBOW;
            } else if (effectType == "ripple") {
                parsedType = E_RIPPLE;
            } else {
                throw std::invalid_argument("Could not parse effect type: " + effectType);
            }

            return new Effect(
                    j["id"].get<int>(),
                    parsedType,
                    j["name"].get<std::string>(),
                    Pair_t::from_json(j["origin"]),
                    Pair_t::from_json(j["size"]),
                    j["startTimeMs"].get<double>(),
                    j["durationMs"].get<double>(),
                    std::move(translation)
            );
        }
        catch (const std::exception &e) {
            std::cerr << "Error parsing effect: " << e.what() << std::endl;
            try {
                std::cerr << "JSON: " << j.dump(4) << std::endl;
            }
            catch (const std::exception &e) {
                std::cerr << "Error printing faulty JSON: " << e.what() << std::endl;
            }
            throw e;
        }
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
        double speed) : Effect(id, std::move(name), origin, size, startTimeMs, durationMs, std::move(translation)),
                        colors(std::move(colors)), speed(speed) {}

    RainbowEffect(const RainbowEffect &) = delete;

    RainbowEffect &operator=(const RainbowEffect &) = delete;

    static RainbowEffect *from_json(const nlohmann::json &j) {
        std::unique_ptr<Translation_t> translation = nullptr;

        if (j.contains("translation")) {
            translation.reset(Translation_t::from_json(j["translation"]));
        }

        std::vector<std::string> colors;
        for (const auto &color: j["colors"]) {
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
                j["speed"].get<int>());
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
        int speed) : Effect(id, std::move(name), origin, size, startTimeMs, durationMs, std::move(translation)),
                     ripple_origin(ripple_origin), speed(speed) {}

    RippleEffect(const RippleEffect &) = delete;

    RippleEffect &operator=(const RippleEffect &) = delete;

    static RippleEffect *from_json(const nlohmann::json &j) {
        std::unique_ptr<Translation_t> translation = nullptr;

        if (j.contains("translation")) {
            translation.reset(Translation_t::from_json(j["translation"]));
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
                j["speed"].get<int>());
    }
};

#endif // PIXELCONTROLLER_EFFECT_H