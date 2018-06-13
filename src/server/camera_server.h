// Author: Tucker Haydon

#pragma once

#include "types.h"
#include "quadcam.h"

#include <string>
#include <memory>
#include <atomic>


namespace quadcam {

class CameraServer {
public:
   CameraServer(const std::string& path); 
   ~CameraServer(); 

    void StartCamera();
    void ConfigureCamera(const CamConfig& cfg);
    void ConfigureCameraDefault();


private:
    void ReportError(const std::string& msg);
    void SendFD(const FD& fd, const FD& socket_fd, const std::string& frame_info);
    void FrameHandler(camera::ICameraFrame* frame);
    void PublishFrame(camera::ICameraFrame* frame, const size_t& width, const size_t& height);

    FD fd_;
    std::string path_;
    std::shared_ptr<QuadCam> camera_;
    std::atomic<bool> busy_publishing{false};

};

}; // namespace quadcam