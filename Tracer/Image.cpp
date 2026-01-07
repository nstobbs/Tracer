#include "Tracer/Image.hpp"

namespace Tracer {

/* Tracer::Channel */
Channel::Channel(i32 width, i32 height) {
    m_data.resize(height);
    for (auto& row : m_data) {
        row.resize(width);
    }
    FloodColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
};

void Channel::FloodColor(Color4 color) {
    for (auto& row : m_data) {
        for (auto& pixel : row) {
            pixel = color;
        }
    }
};

void Channel::DrawTestPatten(TestPatten type) {
    switch(type){
        case TestPatten::eNone: {
        }

        case TestPatten::eChecker: {
        }

        case TestPatten::eUVRamp: {
            auto h = m_data.size();
            for (int i = 0; i < h; i++) {
                auto& row = m_data[i]; 
                auto w = row.size();
                for (int j = 0; j < w; j++) {
                    auto color = Color4((float(j) / float(w)),
                                         (float(i) / float(h)), 0.0f, 1.0f);
                    row[j] = color;
                }
            }
        }
    }

};

i32 Channel::GetRowCount() {
    return m_data.size();
};

std::vector<Color4>& Channel::GetRow(u32 index) {
    return m_data[index];
};

/* Tracer::Image */
void Image::CreateChannel(const std::string& name) {
    m_channels.emplace(name, Channel(m_width, m_height));
    m_channelNames.push_back(name);
};

Channel* Image::GetChannel(const std::string& name) {
    if (m_channels.find(name) != m_channels.end()) {
        return &m_channels.at(name);
    }
    return nullptr;
};

std::vector<std::string> Image::GetChannelNames() {
    return m_channelNames;
};

}