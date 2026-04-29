#include "wavtool/Renderer.h"
#include <vector>
#include <filesystem>
#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <fstream>



namespace wavtool {

struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};


static void setPixel(std::vector<Pixel> &pixels, size_t width, size_t height, size_t x, size_t y, Pixel color) {
    if (x >= width || y >= height) {
        return;
    }
    pixels[y * width + x] = color;
}

static size_t amplitudeToY(int16_t sample, size_t height) {
    double normalized = sample / 32768.0;
    double centerY = height / 2.0;
    double halfHeight = height / 2.0; 
    double yFloat = centerY - normalized * halfHeight;
    int yRounded = static_cast<int>(std::round(yFloat));
    if (yRounded >= static_cast<int>(height) ) {
        yRounded = static_cast<int>(height) - 1;
    } else if (yRounded < 0) {
        yRounded = 0;
    }
    return static_cast<size_t>(yRounded);
}

static Pixel computeFadeColor(size_t y, size_t centerY, size_t height, RgbColor baseColor) {
    double distance = std::abs(static_cast<double>(y) - centerY);
    double halfHeight = height / 2.0;
    double fade = 1.0 - (distance / halfHeight);
    fade = std::clamp(fade, 0.0, 1.0);
    uint8_t resultR = static_cast<uint8_t>(std::round(255 * (1 - fade) + baseColor.red * fade));
    uint8_t resultG = static_cast<uint8_t>(std::round(255 * (1 - fade) + baseColor.green * fade));
    uint8_t resultB = static_cast<uint8_t>(std::round(255 * (1 - fade) + baseColor.blue * fade));
    
    return Pixel{resultR, resultG, resultB};
}

static void writeBmp(const std::filesystem::path& outputPath, const std::vector<Pixel>& pixels, size_t width, size_t height) {
    std::ofstream file(outputPath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open specified file to write BMP: " + outputPath.string());
    }

    uint32_t  rowSize = static_cast<uint32_t>(width * 3);
    uint32_t  padding = (4 - (rowSize % 4)) % 4;
    uint32_t  dataSize = (rowSize + padding) * static_cast<uint32_t>(height);
    uint32_t  fileSize = 54 + dataSize;

    file.put('B');
    file.put('M');
    file.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
    
    uint16_t reserved = 0;
    file.write(reinterpret_cast<const char*>(&reserved), sizeof(reserved));
    file.write(reinterpret_cast<const char*>(&reserved), sizeof(reserved));
    
    uint32_t offset = 54;
    file.write(reinterpret_cast<const char*>(&offset), sizeof(offset));

    uint32_t infoHeaderSize = 40;
    uint32_t width32 = static_cast<uint32_t>(width);
    uint32_t height32 = static_cast<uint32_t>(height);
    uint16_t planes = 1;
    uint16_t bitsPerPixel = 24;
    uint32_t compression = 0;
    uint32_t xPixelsPerMeter = 0;
    uint32_t yPixelsPerMeter = 0;
    uint32_t colorsUsed = 0;
    uint32_t importantColors = 0;

    file.write(reinterpret_cast<const char*>(&infoHeaderSize), sizeof(infoHeaderSize));
    file.write(reinterpret_cast<const char*>(&width32), sizeof(width32));
    file.write(reinterpret_cast<const char*>(&height32), sizeof(height32));
    file.write(reinterpret_cast<const char*>(&planes), sizeof(planes));
    file.write(reinterpret_cast<const char*>(&bitsPerPixel), sizeof(bitsPerPixel));
    file.write(reinterpret_cast<const char*>(&compression), sizeof(compression));
    file.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
    file.write(reinterpret_cast<const char*>(&xPixelsPerMeter), sizeof(xPixelsPerMeter));
    file.write(reinterpret_cast<const char*>(&yPixelsPerMeter), sizeof(yPixelsPerMeter));
    file.write(reinterpret_cast<const char*>(&colorsUsed), sizeof(colorsUsed));
    file.write(reinterpret_cast<const char*>(&importantColors), sizeof(importantColors));

    uint8_t paddingBytes[3] = {0, 0, 0};

    for (int y = static_cast<int>(height) - 1; y >= 0; --y) {
        for (size_t x = 0; x < width; ++x) {
            const Pixel& pixel = pixels[y * width + x];

            file.put(static_cast<char>(pixel.b));
            file.put(static_cast<char>(pixel.g));
            file.put(static_cast<char>(pixel.r));
        }

        file.write(reinterpret_cast<const char*>(paddingBytes), padding);
    }
    file.close();
}

void Renderer::saveWaveformBmp(const std::vector<WaveformPoint> &waveform, size_t height, RgbColor chosenColor, 
                                const std::filesystem::path& outputPath) {
    
    if (waveform.empty()) {
        throw std::invalid_argument("Waveform has no data to draw BMP image with");
    }
    if (height < 2) {
        throw std::invalid_argument("Height too small to draw BMP image with");
    }
    if (outputPath.empty()) {
        throw std::invalid_argument("No file location specified to save BMP image to");
    }

    size_t width = waveform.size();                                
    Pixel white{255, 255, 255};
    std::vector<Pixel> pixels(width * height, white);
    Pixel black{0, 0, 0};
    size_t centerY = height / 2;

    for (size_t i = 0; i < width; i++) {
        size_t yTop = amplitudeToY(waveform[i].maxAmp, height);
        size_t yBottom = amplitudeToY(waveform[i].minAmp, height);

        if (yTop > yBottom) {
            std::swap(yTop, yBottom);
        }
        for (size_t j = yTop; j <= yBottom; j++) {
            Pixel color = computeFadeColor(j, centerY, height, chosenColor);
            setPixel(pixels, width, height, i, j, color);
        }

    }

    for (size_t i = 0; i < width; i++) {
        setPixel(pixels, width, height, i, centerY, black);
    }

    writeBmp(outputPath, pixels, width, height);
}
}