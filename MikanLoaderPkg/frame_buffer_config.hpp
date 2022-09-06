#pragma once

#include <stdint.h>

enum PixelFormat {
  kPixelRGBResv8BitPerColor,
  kPixelBGRResv8BitPerColor,
};

struct FrameBufferConfig {
  // フレームバッファ領域へのポインタ
  uint8_t* frame_buffer;
  // フレームバッファの余白を含めた横方向のピクセル数
  uint32_t pixels_per_scan_line;
  // 解像度(水平)
  uint32_t horizontal_resolution;
  // 解像度(垂直)
  uint32_t vertical_resolution;
  // ピクセルのデータ形式
  enum PixelFormat pixel_format;
};
