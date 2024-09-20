//
// Created by Nick Vazquez on 9/12/24.
//

#ifndef PIXELCONTROLLER_SPATIALS_H
#define PIXELCONTROLLER_SPATIALS_H

#include "../../include/json.hpp"

typedef struct Pair
{
    int x;
    int y;

    static Pair from_json(const nlohmann::json &j)
    {
        return {
            x : j["x"],
            y : j["y"]
        };
    }
} Pair_t;

typedef struct Translation
{
    Pair_t end_pos;
    double durationMs;

    static Translation from_json(const nlohmann::json &j)
    {
        return {
            end_pos : Pair_t::from_json(j["end_pos"]),
            durationMs : j["durationMs"]
        };
    }
} Translation_t;

#endif // PIXELCONTROLLER_SPATIALS_H
