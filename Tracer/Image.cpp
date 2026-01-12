#include "Tracer/Image.hpp"

namespace Tracer {

/* Tracer::Layer */
Layer::Layer(i32 width, i32 height) {
    m_data.resize(height);
    for (auto& row : m_data) {
        row.resize(width);
    }
    FloodColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
};

void Layer::FloodColor(Color4 color) {
    for (auto& row : m_data) {
        for (auto& pixel : row) {
            pixel = color;
        }
    }
};

void Layer::DrawTestPatten(TestPatten type) {
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

i32 Layer::GetRowCount() {
    return m_data.size();
};

std::vector<Color4>& Layer::GetRow(u32 index) {
    return m_data[index];
};

/* Tracer::Image */
void Image::CreateLayer(const std::string& name) {
    m_layers.emplace(name, Layer(m_width, m_height));
    m_layerNames.push_back(name);
};

Layer* Image::GetLayer(const std::string& name) {
    if (m_layers.find(name) != m_layers.end()) {
        return &m_layers.at(name);
    }
    return nullptr;
};

std::vector<std::string> Image::GetLayerNames() {
    return m_layerNames;
};

}