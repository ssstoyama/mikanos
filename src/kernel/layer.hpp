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
    bool draggable_{false};

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
    void DrawTo(FrameBuffer &screen) const;
    void DrawTo(FrameBuffer &screen, const Rectangle<int> &area);

    Vector2D<int> GetPosition() const;

    Layer &SetDraggable(bool draggable);
    bool IsDraggable() const;
};

class LayerManager {
private:
    FrameBuffer *screen_{nullptr};
    mutable FrameBuffer back_buffer_{};
    std::vector<std::unique_ptr<Layer>> layers_{};
    /** 先頭が最背面、末尾が最前面 */
    std::vector<Layer *> layer_stack_{};
    unsigned int latest_id_{0};

    Layer *findLayer(unsigned int id);

public:
    /** @brief 描画先を設定する。 */
    void SetWriter(FrameBuffer *screen);
    /** @brief 新しいレイヤーを生成して参照を返す。
     * 
     * 新しく生成されたレイヤーの実体は LayerManager 内部のコンテナで保持される。
     */
    Layer &NewLayer();
    /** @brief 現在表示状態にあるレイヤーを描画する。 */
    void Draw() const;
    void Draw(const Rectangle<int> &area) const ;
    void Draw(unsigned int id) const;
    /** @brief レイヤの位置を絶対座標へと更新する。再描画はしない。 */
    void Move(unsigned int id, Vector2D<int> new_position);
    /** @brief レイヤの位置を相対座標へと更新する。再描画はしない。 */
    void MoveRelative(unsigned int id, Vector2D<int> pos_diff);
    /** @brief レイヤーの高さ(z座標)の方向の位置を指定された位置に移動する。
     * 
     * new_height に負の高さを指定するとレイヤーは非表示となり、
     * 0 以上を指定するとその高さとなる。
     * 現在のレイヤー数以上の数値を指定した場合は最前面のレイヤーとなる。
     */
    void UpDown(unsigned int id, int new_height);
    /** @brief レイヤーを非表示にする。 */
    void Hide(unsigned int id);

    Layer *FindLayerByPosition(Vector2D<int> pos, unsigned int exclude_id) const;
};

// global LayerManager
extern LayerManager *layer_manager;

void InitializeLayer();
