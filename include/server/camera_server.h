// Author: Tucker Haydon

#pragma once

#include "types.h"
#include "snapcam.h"

#include <string>
#include <memory>
#include <atomic>


namespace snapcam {

class CameraServer {
public:
   CameraServer(std::string path); 
   ~CameraServer(); 

   void PublishFrame(camera::ICameraFrame* frame, const size_t& width, const size_t& height);


private:
    void ReportError(const std::string& msg);
    void SendFD(const FD& socket_fd, const FD& fd, const std::string& frame_info);
    void StartCamera();
    void FrameHandler(camera::ICameraFrame* frame);

    FD fd_;
    std::string path_;
    std::shared_ptr<SnapCam> camera_;
    std::atomic<bool> busy_publishing{false};

};

}; // namespace snapcam
