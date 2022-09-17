#pragma once

#include <memory>
#include <algorithm>

#include "graphics.hpp"
#include "window.hpp"

class Layer {
private:
    unsigned int id_;
    Vector2D<int> pos_;
    std::shared_ptr<Window> window_;

public:
    Layer(unsigned int id = 0);

    unsigned int ID() const;

    /** @brief ウィンドウを設定する。既存のウィンドウはこのレイヤーから外れる。 */
    Layer &SetWindow(const std::shared_ptr<Window> &window);
    std::shared_ptr<Window> GetWindow() const;

    /** @brief レイヤーの位置情報を指定された絶対座標へと更新する。再描画はしない。 */
    Layer &Move(Vector2D<int> pos);
    /** @brief レイヤーの位置情報を指定された相対座標へと更新する。再描画はしない。 */
    Layer &MoveRelative(Vector2D<int> pos_diff);

    /** @brief writer に現在設定されているウィンドウの内容を描画する。 */
    void DrawTo(PixelWriter &writer) const;
};
