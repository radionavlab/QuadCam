// Author: Tucker Haydon

#pragma once

#include "frame_data.h"

#include <string>

namespace quadcam {

class CameraClient {
public:
    CameraClient(const std::string& path);
    FrameData RequestFrame(); 

private:
    std::string path_;
    int server_fd_;

    void ReportError(const char* msg);
    void Connect();

    // RAII wrapper for the server connection
    struct ConnectionRAII {
        ConnectionRAII(CameraClient* client);
        ~ConnectionRAII();
        CameraClient* client_;
    };
};

}; // namespace quadcam
