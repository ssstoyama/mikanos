#pragma once

#include <vector>
#include <optional>

#include "graphics.hpp"
#include "frame_buffer.hpp"

class Window {
public:
    /** @brief WindowWriter は Window と関連づけられた PixelWriter を提供する。 */
    class WindowWriter: public PixelWriter {
    private:
        Window &window_;
    
    public:
        WindowWriter(Window &window): window_{window} {}

        void Write(int x, int y, const PixelColor &color) override {
            Write(Vector2D<int>{x, y}, color);
        }
        void Write(Vector2D<int> pos, const PixelColor &color) override {
            window_.Write(pos, color);
        }

        int Width() const override {
            return window_.Width();
        }

        int Height() const override {
            return window_.Height();
        }
    };

    /** @brief 指定されたピクセル数の平面描画領域を作成する。 */
    Window(int width, int height, PixelFormat shadow_format);
    ~Window() = default;
    Window(const Window& rhs) = delete;
    Window &operator =(const Window& rhs) = delete;

    /** @brief PixelWriter にウィンドウの表示領域を描画する。
     * 
     * @param writer 描画先
     * @param position writer の左上を基準とした描画位置
     */
    void DrawTo(FrameBuffer &dst, Vector2D<int> position);

    /** @brief 透過色を設定する。 */
    void SetTransparentColor(std::optional<PixelColor> c);

    WindowWriter *Writer();
    const PixelColor &At(int x, int y) const;
    const PixelColor &At(Vector2D<int>) const;
    void Write(Vector2D<int> pos, PixelColor color);

    int Width() const;
    int Height() const;

private:
    int width_, height_;
    std::vector<std::vector<PixelColor>> data_{};
    WindowWriter writer_{ *this };
    std::optional<PixelColor> transparent_color_{std::nullopt};
    FrameBuffer shadow_buffer_{};
};
