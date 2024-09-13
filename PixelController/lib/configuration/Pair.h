//
// Created by Nick Vazquez on 9/12/24.
//

#ifndef PIXELCONTROLLER_PAIR_H
#define PIXELCONTROLLER_PAIR_H

#include "../../include/json.hpp"

class Pair {
public:
    int x;
    int y;

    Pair(int x, int y);

    static Pair from_json(const nlohmann::json &j);
};

class Translation {
public:
    Pair *end_pos;
    double durationMs;

    Translation(Pair *end_pos, double durationMs);

    static Translation *from_json(const nlohmann::json &j);
};

#endif //PIXELCONTROLLER_PAIR_H
