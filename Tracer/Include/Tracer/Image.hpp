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

class Channel {
public:
    Channel(i32 width, i32 height);
    ~Channel() = default;

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

    void CreateChannel(const std::string& name);
    Channel* GetChannel(const std::string& name);
    std::vector<std::string> GetChannelNames();

protected:
    i32 m_width;
    i32 m_height;
    std::unordered_map<std::string, Channel> m_channels;
    std::vector<std::string> m_channelNames;
};

}