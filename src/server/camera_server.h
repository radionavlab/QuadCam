// Author: Tucker Haydon

#pragma once

#include "quadcam.h"
#include "frame_data.h"

#include <string>
#include <memory>
#include <atomic>


namespace quadcam {

class CameraServer {
public:
   CameraServer(const std::string& server_path,
                const std::string& camera_type,
                const std::string& camera_resolution); 
   ~CameraServer(); 

    void StartCamera();

private:
    CamConfig ComposeCamConfig();
    void ReportError(const std::string& msg);
    void SendFD(const int& fd, const int& socket_fd, FrameMetaData frame_meta_data);
    void FrameHandler(camera::ICameraFrame* frame);
    void PublishFrame(camera::ICameraFrame* frame, const size_t& width, const size_t& height);

    int fd_;
    std::string server_path_;
    std::string camera_type_;
    std::string camera_resolution_;
    int image_width_;
    int image_height_;
    std::shared_ptr<QuadCam> camera_;
    std::atomic<bool> busy_publishing{false};

};

}; // namespace quadcam
