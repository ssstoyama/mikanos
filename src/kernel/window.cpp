#include "window.hpp"
#include "logger.hpp"

Window::Window(int width, int height, PixelFormat shadow_format)
    : width_{width}, height_{height} {
    data_.resize(height);
    for (int i = 0; i < height; ++i) {
        data_[i].resize(width);
    }

    FrameBufferConfig config{};
    config.frame_buffer = nullptr;
    config.horizontal_resolution = width;
    config.vertical_resolution = height;
    config.pixel_format = shadow_format;

    if (auto err = shadow_buffer_.Initialize(config)) {
        Log(kError, "failed to initialize shadow buffer: %s at %s:%d\n",
            err.Name(), err.File(), err.Line());
    }
}

void Window::DrawTo(FrameBuffer &dst, Vector2D<int> position) {
    // 背景色がない場合
    if (!transparent_color_) {
        dst.Copy(position, shadow_buffer_);
        return;
    }

    const auto tc = transparent_color_.value();
    auto &writer = dst.Writer();
    for (int y = std::max(0, 0-position.y);
        y < std::min(Height(), writer.Height() - position.y); ++y) {
        for (int x = std::max(0, 0-position.x);
            x < std::min(Width(), writer.Width()-position.x); ++x) {
            const auto c = At(x, y);
            if (c != tc) {
                writer.Write(position.x+x, position.y+y, c);
            }
        }
    }
}

void Window::SetTransparentColor(std::optional<PixelColor> c) {
    transparent_color_ = c;
}

Window::WindowWriter *Window::Writer() {
    return &writer_;
}

const PixelColor &Window::At(int x, int y) const {
    return data_[y][x];
}

const PixelColor &Window::At(Vector2D<int> pos) const {
    return data_[pos.y][pos.x];
}

void Window::Write(Vector2D<int> pos, PixelColor color) {
    data_[pos.y][pos.x] = color;
    shadow_buffer_.Writer().Write(pos, color);
}

void Window::Move(Vector2D<int> dst_pos, const Rectangle<int> &src) {
    shadow_buffer_.Move(dst_pos, src);
}

int Window::Width() const {
    return width_;
}

int Window::Height() const {
    return height_;
}