//
// Created by Nick Vazquez on 9/11/24.
//

#include "Configuration.h"
#include "Pair.h"

Sensor Sensor::from_json(const nlohmann::json &j) {
    int id = j["id"];
    int pin = j["pin"];
    SensorType type = j["type"];
    Pair location = Pair::from_json(j["location"]);
    return Sensor{id, pin, type, &location};
}

Effect * Effect::from_json(const nlohmann::json &j) {
    int id = j["id"];
    std::string name = j["name"];
    Pair origin = Pair::from_json(j["origin"]);
    Pair size = Pair::from_json(j["size"]);
    double startTimeMs = j["startTimeMs"];
    double durationMs = j["durationMs"];
    Translation *translation = nullptr;
    if (j.contains("translation")) {
        translation = Translation::from_json(j["translation"]);
    }
    return new Effect(id, &name, &origin, &size, startTimeMs, durationMs, translation);
}
