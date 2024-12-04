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

class Layout {
public:
    int id;
    int pixels;
    LayoutType shape;

    Layout(int id, int pixels, LayoutType shape) {
        this->id = id;
        this->pixels = pixels;
        this->shape = shape;
    }

    virtual ~Layout() = default;
    // added these vvv
    virtual bool isGridLayout() const { return false; }
    virtual int getWidth() const { throw std::runtime_error("Not a GridLayout"); }
    virtual int getHeight() const { throw std::runtime_error("Not a GridLayout"); }

    static Layout *from_json(const nlohmann::json &j) {
        auto *layout = new Layout(
                j.value("id", 0),
                j.value("pixels", 0),
                j.value("shape", LayoutType::CUSTOM)
        );
        return layout;
    }
};

class GridLayout : public Layout {
public:
    int width;
    int height;

    inline GridLayout(int id, int width, int height) :
            Layout(id, width * height, LayoutType::GRID) {
        this->width = width;
        this->height = height;
    }

    static GridLayout *from_json(const nlohmann::json &j) {
        auto id = j.value("id", 1);
        auto width = std::max(1, j["width"].get<int>());
        auto height = std::max(1, j["height"].get<int>());

        return new GridLayout(id, width, height);
    }

    // i know these look nonsensical (maybe not the getters lol) but trust -- i need em
    bool isGridLayout() const override { return true; }
    int getWidth() const override { return width; }
    int getHeight() const override { return height; }
};

#endif // PIXELCONTROLLER_LAYOUT_H