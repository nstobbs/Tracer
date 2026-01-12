#pragma once
#include "Tracer/Types.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace Tracer {

using Row = std::vector<Color4>;

enum class TestPatten {
    eNone = -1,
    eUVRamp = 0,
    eChecker = 1 };

enum class PreDefinedLayer {
    eInvalid = 0,
    eColor = 1,
    eNormals = 2,
    eDepth = 3,
    ePosition = 4 };

class Layer {
public:
    Layer(i32 width, i32 height);
    ~Layer() = default;

    Color4& at(u32 x, u32 y) { return m_data[y][x]; };

    void FloodColor(Color4 color);
    void DrawTestPatten(TestPatten type);
    i32 GetRowCount();
    std::vector<Color4>& GetRow(u32 index);

protected:
    std::vector<Row> m_data;
};

class Image {
public:
    Image(i32 width, i32 height) : m_width(width), m_height(height) { };
    ~Image() = default;

    void CreateLayer(const std::string& name);
    Layer* GetLayer(const std::string& name);
    std::vector<std::string> GetLayerNames();

    u32 GetWidth() const { return m_width; };
    u32 GetHeight() const { return m_height; };


protected:
    u32 m_width;
    u32 m_height;
    std::unordered_map<std::string, Layer> m_layers;
    std::vector<std::string> m_layerNames;
};

}