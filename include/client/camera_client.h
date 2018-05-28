// Author: Tucker Haydon

#pragma once

#include "types.h"

#include <string>

namespace snapcam {

class CameraClient {
public:
    CameraClient(const std::string& path);
    FrameData RequestFrame(); 

private:
    std::string path_;
    FD server_fd_;

    void ReportError(const char* msg);
    void Connect();

    // RAII wrapper for the server connection
    struct ConnectionRAII {
        ConnectionRAII(CameraClient* client);
        ~ConnectionRAII();
        CameraClient* client_;
    };
};

}; // namespace snapcam
