//
// Created by Nick Vazquez on 9/12/24.
//

#include "Pair.h"

Pair::Pair(int x, int y) {
    this->x = x;
    this->y = y;
}

Pair Pair::from_json(const nlohmann::json &j) {
    return Pair(j["x"], j["y"]);
}

Translation::Translation(Pair *end_pos, double durationMs) {
    this->end_pos = end_pos;
    this->durationMs = durationMs;
}

Translation *Translation::from_json(const nlohmann::json &j) {
    Pair end_pos = Pair::from_json(j["end_pos"]);
    double durationMs = j["durationMs"];
    return new Translation(&end_pos, durationMs);
}