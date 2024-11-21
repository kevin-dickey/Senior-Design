#ifndef PIXELCONTROLLER_EFFECT_H
#define PIXELCONTROLLER_EFFECT_H

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
    Pair_t origin;
    Pair_t size;
    double startTimeMs;
    double durationMs;
    std::unique_ptr<Translation_t> translation;

    inline Effect(int id, std::string name,
                  Pair_t origin, Pair_t size,
                  double startTimeMs, double durationMs,
                  std::unique_ptr<Translation_t> translation)
    {
        this->id = id;
        this->name = std::move(name);
        this->origin = origin;
        this->size = size;
        this->startTimeMs = startTimeMs;
        this->durationMs = durationMs;
        this->translation = std::move(translation);
    }

    Effect(const Effect &) = delete;
    Effect &operator=(const Effect &) = delete;
    virtual ~Effect() = default;

    static Effect *from_json(const nlohmann::json &j)
    {
        std::unique_ptr<Translation_t> translation = nullptr;
        if (j.contains("translation"))
        {
            translation.reset(Translation_t::from_json(j["translation"]));
        }

        // Check if all required fields are present
        if (!j.contains("id") || !j.contains("name") || !j.contains("origin") || !j.contains("size") || !j.contains("startTimeMs") || !j.contains("durationMs"))
        {
            throw std::invalid_argument("Effect JSON missing required fields");
        }

        return new Effect(
            j["id"].get<int>(),
            j["name"].get<std::string>(),
            Pair_t::from_json(j["origin"]),
            Pair_t::from_json(j["size"]),
            j["startTimeMs"].get<double>(),
            j["durationMs"].get<double>(),
            std::move(translation));
    }
};

class RainbowEffect : public Effect
{
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
    ~RainbowEffect() = default;

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
        int id, std::string name,
        Pair_t origin, Pair_t size,
        double startTimeMs, double durationMs,
        std::unique_ptr<Translation_t> translation,
        Pair_t ripple_origin,
        int speed) : Effect(id, std::move(name), origin, size, startTimeMs, durationMs, std::move(translation)),
                     ripple_origin(ripple_origin), speed(speed) {}

    RippleEffect(const RippleEffect &) = delete;
    RippleEffect &operator=(const RippleEffect &) = delete;
    ~RippleEffect() = default;

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