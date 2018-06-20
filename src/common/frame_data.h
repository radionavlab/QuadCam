// Author: Tucker Haydon

#pragma once

#include <memory>

namespace quadcam {

typedef struct {
    uint32_t data_size;
    uint32_t width;
    uint32_t height;
} FrameMetaData;

typedef struct {
    FrameMetaData meta_data;
    std::shared_ptr<uint8_t> data;
} FrameData;

}; // namespace quadcam
