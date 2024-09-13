//
// Created by Nick Vazquez on 9/12/24.
//

#ifndef PIXELCONTROLLER_SPATIALS_H
#define PIXELCONTROLLER_SPATIALS_H

#include "../../include/json.hpp"

struct Pair {
    int x;
    int y;

    static Pair from_json(const nlohmann::json &j) {
        return {j["x"], j["y"]};
    }
};

struct Translation {
    Pair end_pos;
    double durationMs;

    static Translation *from_json(const nlohmann::json &j) {
        Translation *translation = new Translation();
        translation->end_pos = Pair::from_json(j["end_pos"]);
        translation->durationMs = j["durationMs"];
        return translation;
    }
};

#endif //PIXELCONTROLLER_SPATIALS_H
