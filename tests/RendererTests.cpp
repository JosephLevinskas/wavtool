#include "wavtool/Renderer.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

struct BgrPixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

uint16_t readU16(std::ifstream& file) {
    uint8_t bytes[2];
    file.read(reinterpret_cast<char*>(bytes), 2);

    return static_cast<uint16_t>(
        bytes[0] |
        (bytes[1] << 8)
    );
}

uint32_t readU32(std::ifstream& file) {
    uint8_t bytes[4];
    file.read(reinterpret_cast<char*>(bytes), 4);

    return static_cast<uint32_t>(
        bytes[0] |
        (bytes[1] << 8) |
        (bytes[2] << 16) |
        (bytes[3] << 24)
    );
}

std::vector<BgrPixel> readBmpPixelsTopDown(
    const std::filesystem::path& path,
    size_t width,
    size_t height
) {
    std::ifstream file(path, std::ios::binary);
    require(static_cast<bool>(file), "Could not open BMP file for reading");

    file.seekg(54);

    size_t rowSize = width * 3;
    size_t padding = (4 - (rowSize % 4)) % 4;

    std::vector<BgrPixel> pixels(width * height);

    for (int y = static_cast<int>(height) - 1; y >= 0; --y) {
        for (size_t x = 0; x < width; ++x) {
            BgrPixel pixel{};
            file.read(reinterpret_cast<char*>(&pixel), 3);

            pixels[static_cast<size_t>(y) * width + x] = pixel;
        }

        file.ignore(static_cast<std::streamsize>(padding));
    }

    return pixels;
}

void testRendererRejectsEmptyWaveform() {
    bool threw = false;

    try {
        wavtool::Renderer::saveWaveformBmp(
            {},
            100,
            wavtool::RgbColor{0, 100, 255},
            "empty_should_fail.bmp"
        );
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    require(threw, "Renderer should reject empty waveform");
}

void testRendererRejectsTinyHeight() {
    bool threw = false;

    try {
        std::vector<wavtool::WaveformPoint> waveform{
            wavtool::WaveformPoint{0, 0}
        };

        wavtool::Renderer::saveWaveformBmp(
            waveform,
            1,
            wavtool::RgbColor{0, 100, 255},
            "tiny_height_should_fail.bmp"
        );
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    require(threw, "Renderer should reject height less than 2");
}

void testRendererWritesValidBmpHeader() {
    std::filesystem::path outputPath = "renderer_header_test.bmp";

    std::vector<wavtool::WaveformPoint> waveform{
        wavtool::WaveformPoint{0, 0},
        wavtool::WaveformPoint{-32768, 32767},
        wavtool::WaveformPoint{0, 0}
    };

    size_t width = waveform.size();
    size_t height = 100;

    wavtool::Renderer::saveWaveformBmp(
        waveform,
        height,
        wavtool::RgbColor{0, 100, 255},
        outputPath
    );

    std::ifstream file(outputPath, std::ios::binary);
    require(static_cast<bool>(file), "Could not open written BMP");

    char b = 0;
    char m = 0;

    file.get(b);
    file.get(m);

    require(b == 'B', "BMP signature first byte should be B");
    require(m == 'M', "BMP signature second byte should be M");

    uint32_t fileSize = readU32(file);

    uint16_t reserved1 = readU16(file);
    uint16_t reserved2 = readU16(file);
    uint32_t pixelOffset = readU32(file);

    uint32_t infoHeaderSize = readU32(file);
    uint32_t bmpWidth = readU32(file);
    uint32_t bmpHeight = readU32(file);
    uint16_t planes = readU16(file);
    uint16_t bitsPerPixel = readU16(file);

    require(fileSize > 54, "BMP file size should be greater than header size");
    require(reserved1 == 0, "BMP reserved1 should be zero");
    require(reserved2 == 0, "BMP reserved2 should be zero");
    require(pixelOffset == 54, "BMP pixel offset should be 54");
    require(infoHeaderSize == 40, "BMP info header size should be 40");
    require(bmpWidth == width, "BMP width incorrect");
    require(bmpHeight == height, "BMP height incorrect");
    require(planes == 1, "BMP planes should be 1");
    require(bitsPerPixel == 24, "BMP should be 24-bit");
    file.close();
    std::filesystem::remove(outputPath);
}

void testRendererDrawsBlackCenterLine() {
    std::filesystem::path outputPath = "renderer_center_line_test.bmp";

    std::vector<wavtool::WaveformPoint> waveform{
        wavtool::WaveformPoint{0, 0},
        wavtool::WaveformPoint{0, 0},
        wavtool::WaveformPoint{0, 0},
        wavtool::WaveformPoint{0, 0}
    };

    size_t width = waveform.size();
    size_t height = 100;
    size_t centerY = height / 2;

    wavtool::Renderer::saveWaveformBmp(
        waveform,
        height,
        wavtool::RgbColor{0, 100, 255},
        outputPath
    );

    auto pixels = readBmpPixelsTopDown(outputPath, width, height);

    for (size_t x = 0; x < width; ++x) {
        BgrPixel pixel = pixels[centerY * width + x];

        require(pixel.r == 0, "Center line red channel should be black");
        require(pixel.g == 0, "Center line green channel should be black");
        require(pixel.b == 0, "Center line blue channel should be black");
    }

    std::filesystem::remove(outputPath);
}

void testRendererDrawsWaveformColorAwayFromCenter() {
    std::filesystem::path outputPath = "renderer_waveform_color_test.bmp";

    std::vector<wavtool::WaveformPoint> waveform{
        wavtool::WaveformPoint{-32768, 32767}
    };

    size_t width = waveform.size();
    size_t height = 100;

    wavtool::Renderer::saveWaveformBmp(
        waveform,
        height,
        wavtool::RgbColor{0, 100, 255},
        outputPath
    );

    auto pixels = readBmpPixelsTopDown(outputPath, width, height);

    bool foundNonWhiteNonBlackPixel = false;

    for (const BgrPixel& pixel : pixels) {
        bool isWhite = pixel.r == 255 && pixel.g == 255 && pixel.b == 255;
        bool isBlack = pixel.r == 0 && pixel.g == 0 && pixel.b == 0;

        if (!isWhite && !isBlack) {
            foundNonWhiteNonBlackPixel = true;
            break;
        }
    }

    require(foundNonWhiteNonBlackPixel, "Renderer should draw colored waveform pixels");

    std::filesystem::remove(outputPath);
}

} // namespace

int main() {
    try {
        testRendererRejectsEmptyWaveform();
        testRendererRejectsTinyHeight();
        testRendererWritesValidBmpHeader();
        testRendererDrawsBlackCenterLine();
        testRendererDrawsWaveformColorAwayFromCenter();

        std::cout << "All Renderer tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Renderer test failed: " << e.what() << '\n';
        return 1;
    }
}