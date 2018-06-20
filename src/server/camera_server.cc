// Author: Tucker Haydon

#include "camera_server.h"
#include "utils.h"

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/un.h>
#include <thread>
#include <chrono>
#include <csignal>


namespace quadcam {

CameraServer::CameraServer(const std::string& server_path,
                           const std::string& camera_type,
                           const std::string& camera_resolution) 
    : server_path_(server_path),
      camera_type_(camera_type),
      camera_resolution_(camera_resolution) {

    // Configure: Ignore broken pipes
    std::signal(SIGPIPE, SIG_IGN);

    struct sockaddr_un server_address;
    constexpr size_t ADDRESS_SIZE = sizeof(struct sockaddr_un);

    // Create a server socket as a UNIX TCP socket.
    if ((this->fd_ = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0)) <= 0) { 
            this->ReportError("Server socket creation"); 
    } 

    // Clear and fill server_address
    memset(&server_address, 0, ADDRESS_SIZE);
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, this->server_path_.c_str(), sizeof(server_address.sun_path) - 1);

    // Socket options
    constexpr bool OPT = true;
    constexpr size_t OPT_SIZE = sizeof(bool);
    setsockopt(this->fd_, SOL_SOCKET, SO_REUSEADDR, &OPT, OPT_SIZE);

    // Bind the socket
    if (bind(this->fd_, (struct sockaddr *) &server_address, ADDRESS_SIZE) < 0) {
        this->ReportError("Error binding server socket."); 
    }

    // Start the socket listening
    #define MAX_CLIENTS 128
    if (listen(this->fd_, MAX_CLIENTS) < 0) { 
        this->ReportError("listen"); 
    }
};

void CameraServer::FrameHandler(camera::ICameraFrame* frame) {
    if(this->camera_->CameraType() == CAM_FORWARD) {
        this->PublishFrame(frame, this->image_width_, this->image_height_);
    } else if(this->camera_->CameraType() == CAM_DOWN) {
        std::cout << "This feature not yet implemented! Images can stream, but the bytes are out of order. Please fix this in node.cc!" << std::endl;
    }
};

CamConfig CameraServer::ComposeCamConfig() {
    CamConfig cfg = QuadCam::DEFAULT_CONFIG;

    if(this->camera_type_ == "forward") {
        cfg.cameraId = 1;
        cfg.previewFormat = "yuv420sp";
        cfg.func = CAM_FORWARD;
        if(this->camera_resolution_ == "4k") {
            cfg.previewSize = CameraSizes::UHDSize();
            cfg.pictureSize = CameraSizes::UHDSize();
        } else if(this->camera_resolution_ == "1080p") {
            cfg.previewSize = CameraSizes::FHDSize();
            cfg.pictureSize = CameraSizes::FHDSize();
        } else if(this->camera_resolution_ == "720p") {
            cfg.previewSize = CameraSizes::HDSize();
            cfg.pictureSize = CameraSizes::HDSize();
        } else {
            cfg.previewSize = CameraSizes::VGASize();
            cfg.pictureSize = CameraSizes::VGASize();
        }
    } else {
        cfg.cameraId = 0;
        cfg.previewFormat = "yuv420sp";
        cfg.func = CAM_DOWN;
        cfg.previewSize = CameraSizes::VGASize();
        cfg.pictureSize = CameraSizes::VGASize();
    }

    this->image_width_ = cfg.previewSize.width;
    this->image_height_ = cfg.previewSize.height;

    return cfg;
}

void CameraServer::StartCamera() {
    this->camera_ = std::make_shared<QuadCam>(this->ComposeCamConfig());
    this->camera_->SetListener(std::bind(&CameraServer::FrameHandler, this, std::placeholders::_1));
    this->camera_->Start();
};

CameraServer::~CameraServer() {
    if (shutdown(this->fd_, SHUT_RDWR) < 0) {
        if (errno != ENOTCONN && errno != EINVAL) {
            this->ReportError("Client Socket Shutdown");
        }
    }

    if (close(this->fd_) < 0) { 
        this->ReportError("Client Socket Close");
    }
    unlink(this->server_path_.c_str());
};

void CameraServer::PublishFrame(camera::ICameraFrame* frame, const size_t& width, const size_t& height) {
    if(this->busy_publishing) {
        return;
    } else {
        this->busy_publishing = true;
    }

    const size_t data_size = frame->size;
    const int frame_fd = frame->fd;

    FrameMetaData frame_meta_data{
            .data_size = data_size,
            .width = width,
            .height = height
    };

    struct sockaddr_un client_address;
    socklen_t ADDRESS_SIZE = sizeof(struct sockaddr_un);

    while(true) {
        // Accept a client
        int client_fd = accept(this->fd_, (struct sockaddr *) &client_address, &ADDRESS_SIZE);
        if (client_fd < 0) { 
                // If no queued clients, break
                if(errno == EWOULDBLOCK) {
                    break;
                } else {
                    this->ReportError("Error connecting client");
                }
        }
        SendFD(frame_fd, client_fd, frame_meta_data);
        close(client_fd);
    }
  
    // Permit subscribers to use the frame for 200 ms 
    // std::this_thread::sleep_for(std::chrono::milliseconds(200));  

    this->busy_publishing = false;
}

void CameraServer::SendFD(const int& frame_fd, const int& client_fd, FrameMetaData frame_meta_data) {

    struct msghdr msg = { 0 };
    char buf[CMSG_SPACE(sizeof(frame_fd))];
    memset(buf, '\0', sizeof(buf));
    struct iovec io = { .iov_base = (void*)(&frame_meta_data), .iov_len = sizeof(FrameMetaData) }; 

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    struct cmsghdr * cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(frame_fd));

    *((int *) CMSG_DATA(cmsg)) = frame_fd;

    msg.msg_controllen = cmsg->cmsg_len;

    if (sendmsg(client_fd, &msg, 0) < 0) {
        // Ignore broken pipes
        if(errno != EPIPE) {
            this->ReportError("Failed to send int.");
        }
    }
}


void CameraServer::ReportError(const std::string& msg) {
    std::cout << msg << std::endl;
    std::cout << "Errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
}

}; // namespace quadcam
