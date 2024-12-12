#ifndef PIXELCONTROLLER_EFFECT_H
#define PIXELCONTROLLER_EFFECT_H

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "../../include/json.hpp"
#include "Spatials.h"

enum EffectType : int {
    rainbow = 1,
    ripple = 2,
    pumpkinRainbow = 3,
    pumpkinRipple = 4,
    ghostRainbow = 5,
    ghostRipple = 6,
    pumpkinGhostRainbow = 7,
    pumpkinGhostRipple = 8,
    snowflake = 9,
    snowman = 10,
    christmasTree = 11,
    candyCane = 12,
    pumpkin = 13,
};

namespace std {
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&...args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}  // namespace std

class Effect
{
public:
    int id;
    EffectType effectType;
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
            if (effectType == "rainbow" || effectType == "RainbowEffect") {
                parsedType = rainbow;
            } else if (effectType == "ripple" || effectType == "RippleEffect") {
                parsedType = ripple;
            } else if (effectType == "pumpkinRainbow") {
                parsedType = pumpkinRainbow;
            } else if (effectType == "pumpkinRipple") {
                parsedType = pumpkinRipple;
            } else if (effectType == "ghostRainbow") {
                parsedType = ghostRainbow;
            } else if (effectType == "ghostRipple") {
                parsedType = ghostRipple;
            } else if (effectType == "pumpkinGhostRainbow") {
                parsedType = pumpkinGhostRainbow;
            } else if (effectType == "pumpkinGhostRipple") {
                parsedType = pumpkinGhostRipple;
            } else if (effectType == "snowflake") {
                parsedType = snowflake;
            } else if (effectType == "snowman") {
                parsedType = snowman;
            } else if (effectType == "christmasTree") {
                parsedType = christmasTree;
            } else if (effectType == "candyCane") {
                parsedType = candyCane;
            } else if (effectType == "pumpkin") {
                parsedType = pumpkin;
            } else {
                throw std::invalid_argument("Could not parse effect type: " + effectType);
            }

            return new Effect(
                    j.at("id").get<int>(),
                    parsedType,
                    j.at("name").get<std::string>(),
                    Pair_t::from_json(j["origin"]),
                    Pair_t::from_json(j["size"]),
                    j.at("startTimeMs").get<double>(),
                    j.at("durationMs").get<double>(),
                    std::move(translation)
            );
        } catch (const std::exception &e) {
            std::cerr << "Error parsing effect: " << e.what() << std::endl;
            try {
                std::cerr << "JSON: " << j.dump(4) << std::endl;
            }
            catch (const std::exception &e) {
                std::cerr << "Error printing faulty JSON: " << e.what() << std::endl;
            }
            throw e;
        }
    };
};

class RainbowEffect : public Effect
{
public:
    std::vector<std::string> colors;
    double speed;

    inline RainbowEffect(
            int id, const std::string &name,
            Pair_t origin, Pair_t size,
            double startTimeMs, double durationMs,
            std::unique_ptr<Translation_t> translation,
            std::vector<std::string> colors,
            double speed) : Effect(id, rainbow,
                                   name,
                                   origin, size,
                                   startTimeMs, durationMs,
                                   std::move(translation)),
                            colors(std::move(colors)), speed(speed) {}

    RainbowEffect(const RainbowEffect &) = delete;

    RainbowEffect &operator=(const RainbowEffect &) = delete;

    static RainbowEffect *from_json(const nlohmann::json &j)
    {
        std::unique_ptr<Translation_t> translation = nullptr;

        if (j.contains("translation"))
        {
            translation.reset(Translation_t::from_json(j["translation"]));
        }

        std::vector<std::string> colors;
        for (const auto &color : j["colors"])
        {
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

class RippleEffect : public Effect
{
public:
    Pair ripple_origin;
    int speed;

    inline RippleEffect(
            int id, const std::string &name,
            Pair_t origin, Pair_t size,
            double startTimeMs, double durationMs,
            std::unique_ptr<Translation_t> translation,
            Pair_t ripple_origin,
            int speed) : Effect(id, ripple,
                                name,
                                origin, size,
                                startTimeMs, durationMs,
                                std::move(translation)),
                         ripple_origin(ripple_origin), speed(speed) {}

    RippleEffect(const RippleEffect &) = delete;

    RippleEffect &operator=(const RippleEffect &) = delete;

    static RippleEffect *from_json(const nlohmann::json &j)
    {
        std::unique_ptr<Translation_t> translation = nullptr;

        if (j.contains("translation"))
        {
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