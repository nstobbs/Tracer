#include "Tracer/Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Tracer {

/* Tracer::Layer */
Layer::Layer(i32 width, i32 height) {
    m_data.resize(height);
    for (auto& row : m_data) {
        row.resize(width);
    }
    FloodColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
};

Color4& Layer::at(u32 x, u32 y) { 
    if (y < m_data.size() && y < m_data[y].size()) {
        return m_data[y][x];
    }
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
    if (index < m_data.size()) {
        return m_data[index];
    }
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

Image Image::ReadImage(const std::string& filepath, const std::string& layerName) {
    i32 width, height, channelsCount;
    auto pixels = stbi_load(filepath.c_str(), &width, &height, &channelsCount, STBI_rgb_alpha);
    if (!pixels) {
        std::printf("Failed to Read Image. Error: &s\n", stbi_failure_reason());
    }

    Image output(width, height);
    output.CreateLayer(layerName);
    Layer* RGBA = output.GetLayer(layerName);

    for (i32 y = 0; y < height; y++) {
        Row& row = RGBA->GetRow(y);
        for (i32 x = 0; x < width; x++) {
            unsigned char* pixelOffset = pixels + (x + (y * width)) * channelsCount;
            u8 r = pixelOffset[0];
            u8 g = pixelOffset[1];
            u8 b = pixelOffset[2];
            u8 a = pixelOffset[3];
            Color4 color(static_cast<f32>(r) / 255.0f,
                         static_cast<f32>(g) / 255.0f,
                         static_cast<f32>(b) / 255.0f,
                         static_cast<f32>(a) / 255.0f);
            row.at(x) = color;
        }
    }
    stbi_image_free(pixels);
    return output;
};

}