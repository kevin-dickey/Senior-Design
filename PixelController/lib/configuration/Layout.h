//
// Created by Nick Vazquez on 9/12/24.
//

#ifndef PIXELCONTROLLER_LAYOUT_H
#define PIXELCONTROLLER_LAYOUT_H

#include <string>
#include "../../include/json.hpp"

enum LayoutType {
    GRID = 1,
    CIRCLE = 2,
    LINE = 3,
    CUSTOM = 4
};

struct Layout {
    LayoutType shape;

    virtual ~Layout() = default;

    static Layout *from_json(const nlohmann::json& j) {
        Layout *layout = new Layout();
        layout->shape = j.value("shape", LayoutType::CUSTOM);
        return layout;
    }
};

struct GridLayout : public Layout {
    int width{};
    int height{};

    static GridLayout *from_json(const nlohmann::json& j) {
        auto* gridLayout = new GridLayout();
        gridLayout->shape = LayoutType::GRID;
        gridLayout->width = std::max(1, j.value("width", 1));
        gridLayout->height = std::max(1, j.value("height", 1));
        return gridLayout;
    }
};

#endif //PIXELCONTROLLER_LAYOUT_H
