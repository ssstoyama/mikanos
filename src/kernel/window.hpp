#pragma once

#include <vector>
#include <optional>
#include <string>

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
    virtual ~Window() = default;

    Window(const Window& rhs) = delete;
    Window &operator =(const Window& rhs) = delete;

    /** @brief PixelWriter にウィンドウの表示領域を描画する。
     * 
     * @param writer 描画先
     * @param position writer の左上を基準とした描画位置
     */
    void DrawTo(FrameBuffer &dst, Vector2D<int> position);
    void DrawTo(FrameBuffer &dst, Vector2D<int> position, const Rectangle<int> &area);

    /** @brief 透過色を設定する。 */
    void SetTransparentColor(std::optional<PixelColor> c);

    WindowWriter *Writer();
    const PixelColor &At(int x, int y) const;
    const PixelColor &At(Vector2D<int>) const;
    void Write(Vector2D<int> pos, PixelColor color);

    void Move(Vector2D<int> dst_pos, const Rectangle<int> &src);

    int Width() const;
    int Height() const;
    Vector2D<int> Size() const;

    virtual void Activate() {};
    virtual void Deactivate() {};

private:
    int width_, height_;
    std::vector<std::vector<PixelColor>> data_{};
    WindowWriter writer_{ *this };
    std::optional<PixelColor> transparent_color_{std::nullopt};
    FrameBuffer shadow_buffer_{};
};

class TopLevelWindow: public Window {
public:
    static constexpr Vector2D<int> kTopLeftMargin{4, 24};
    static constexpr Vector2D<int> kBottomRightMargin{4, 4};

    class InnerAreaWriter: public PixelWriter {
    public:
        InnerAreaWriter(TopLevelWindow &window): window_{window} {}
        virtual void Write(Vector2D<int> pos, const PixelColor &color) override {
            window_.Write(pos+kTopLeftMargin, color);
        }

        virtual int Width() const override {
            return window_.Width() - kTopLeftMargin.x - kBottomRightMargin.x;
        }

        virtual int Height() const override {
            return window_.Height() - kTopLeftMargin.y - kBottomRightMargin.y;
        }

    private:
        TopLevelWindow &window_;
    };

    TopLevelWindow(int width, int height, PixelFormat shadow_format, const std::string &title);

    virtual void Activate() override;
    virtual void Deactivate() override;

    InnerAreaWriter *InnerWriter() {
        return &inner_writer_;
    }
    Vector2D<int> InnerSize() const;

private:
    std::string title_;
    InnerAreaWriter inner_writer_{*this};
};

void DrawWindow(PixelWriter &writer, const char *title);
void DrawTextbox(PixelWriter &writer, Vector2D<int> pos, Vector2D<int> size);
void DrawWindowTitle(PixelWriter &writer, const char *title, bool active);
