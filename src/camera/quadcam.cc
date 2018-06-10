// Author: Tucker Haydon

#include "quadcam.h"
#include "utils.h"

#include <iostream>
#include <thread>

const CamConfig QuadCam::DEFAULT_CONFIG = {
            .cameraId       = 1,
            .focusMode      = "continuous-video",
            .whiteBalance   = "auto",
            .ISO            = "auto", 
            .previewFormat  = "yuv420sp",
            .sharpness      = 18,
            .brightness     = 3,
            .contrast       = 5,
	    .exposure       = 100,
	    .gain           = 50, 
	    .previewSize    = CameraSizes::VGASize(), 
	    .pictureSize    = CameraSizes::VGASize(),
            .func           = CAM_FORWARD,
        };

QuadCam::QuadCam() : QuadCam(DEFAULT_CONFIG) {}

QuadCam::QuadCam(const CamConfig& cfg) {
    cb_=nullptr;
    this->Configure(cfg);
}

void QuadCam::Configure(const CamConfig& cfg) {
    config_ = cfg;
}

void QuadCam::Initialize() {

    std::cout << "Added Listner" << std::endl;
    // Ensure camera is connected and accessible
    if (camera::getNumberOfCameras() < 1) {
        std::cout << "No cameras detected. Are you using sudo?" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create camera device
    if(camera::ICameraDevice::createInstance(config_.cameraId, &camera_) != 0) {
        std::cout << "Could not open camera." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Add listener
    camera_->addListener(this);

    // Initialize camera device
    if(params_.init(camera_) != 0) {
        std::cout << "Failed to init parameters." << std::endl;
        camera::ICameraDevice::deleteInstance(&camera_);
        exit(EXIT_FAILURE);
    }

    if(config_.func == CAM_FORWARD) {
        // Set the image sizes
        params_.setPreviewSize(config_.previewSize); 
        params_.setVideoSize(config_.previewSize); 

        // Set image format. Pretty much only YUV420sp
        params_.setPreviewFormat(config_.previewFormat);

        // Leave these for 60 fps
        params_.setPreviewFpsRange(params_.getSupportedPreviewFpsRanges()[3]);
        params_.setVideoFPS(params_.getSupportedVideoFps()[0]);

        // Set picture parameters
        params_.setFocusMode(config_.focusMode);
        params_.setWhiteBalance(config_.whiteBalance);
        params_.setISO(config_.ISO);
        params_.setSharpness(config_.sharpness);
        params_.setBrightness(config_.brightness);
        params_.setContrast(config_.contrast);
    }

    if(config_.func == CAM_DOWN) {
        // Set the image sizes
        params_.setPreviewSize(config_.previewSize); 
        params_.setVideoSize(config_.previewSize); 

        // Set image format. Pretty much only YUV420sp
        params_.setPreviewFormat(config_.previewFormat);

        // Leave these for 60 fps
        params_.setPreviewFpsRange(params_.getSupportedPreviewFpsRanges()[2]);
        params_.setVideoFPS(params_.getSupportedVideoFps()[0]);

        // Set picture parameters
        // params_.setFocusMode(config_.focusMode);
        // params_.setWhiteBalance(config_.whiteBalance);
        // params_.setISO(config_.ISO);
        params_.setSharpness(config_.sharpness);
        params_.setBrightness(config_.brightness);
        params_.setContrast(config_.contrast);

        params_.set("raw-size", "640x480");
    }

    if (params_.commit() != 0) {
        std::cout << "Commit failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void QuadCam::UpdateGain(int gain) {
    params_.setManualGain(gain);
    if(params_.commit() != 0) {
        std::cout << "Could not update gain!" << std::endl;
    }
}

void QuadCam::UpdateExposure(int exposure) {
    params_.setManualExposure(exposure);
    if(params_.commit() != 0) {
        std::cout << "Could not update exposure!" << std::endl;
    }
}

void QuadCam::Start() {
    // Start the camera preview and recording. Will start pushing frames asynchronously to the callbacks
    Initialize();
    camera_->startPreview();
    camera_->startRecording();
}

QuadCam::~QuadCam() {
    camera_->stopRecording();
    camera_->stopPreview();

    /* release camera device */
    camera::ICameraDevice::deleteInstance(&camera_);
}

void QuadCam::onError() {
    std::cout << "Camera error!, aborting\n" << std::endl;
    exit(EXIT_FAILURE);
}

void QuadCam::onPreviewFrame(camera::ICameraFrame *frame) {
    if (!cb_ || config_.func != CAM_DOWN) { return; }
    cb_(frame);
}

void QuadCam::onVideoFrame(camera::ICameraFrame *frame) {
    if (!cb_ || config_.func != CAM_FORWARD) { return; }
    cb_(frame);
}

void QuadCam::SetListener(CallbackFunction fun) {
    cb_ = fun;
}

int QuadCam::CameraType() {
    return config_.func;
}
