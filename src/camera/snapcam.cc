// Author: Tucker Haydon

#include "snapcam.h"
#include "utils.h"

#include <iostream>
#include <thread>


SnapCam::SnapCam(CamConfig cfg) {
    cb_=nullptr;
    initialize(cfg);
}

int SnapCam::initialize(CamConfig cfg) {

    // Ensure camera is connected and accessible
    if (camera::getNumberOfCameras() < 1) {
        printf("No cameras detected. Are you using sudo?\n");
        return EXIT_FAILURE;
    }

    // Create camera device
    if(camera::ICameraDevice::createInstance(cfg.cameraId, &camera_) != 0) {
        printf("Could not open camera.");
        return EXIT_FAILURE;
    }

    // Add listener
    camera_->addListener(this);

    // Initialize camera device
    if(params_.init(camera_) != 0) {
        printf("failed to init parameters\n");
        camera::ICameraDevice::deleteInstance(&camera_);
        return EXIT_FAILURE;
    }

    if(cfg.func == CAM_FORWARD) {
        // Set the image sizes
        params_.setPreviewSize(cfg.previewSize); 
        params_.setVideoSize(cfg.previewSize); 

        // Set image format. Pretty much only YUV420sp
        params_.setPreviewFormat(cfg.previewFormat);

        // Leave these for 60 fps
        params_.setPreviewFpsRange(params_.getSupportedPreviewFpsRanges()[3]);
        params_.setVideoFPS(params_.getSupportedVideoFps()[0]);

        // Set picture parameters
        params_.setFocusMode(cfg.focusMode);
        params_.setWhiteBalance(cfg.whiteBalance);
        params_.setISO(cfg.ISO);
        params_.setSharpness(cfg.sharpness);
        params_.setBrightness(cfg.brightness);
        params_.setContrast(cfg.contrast);
    }

    if(cfg.func == CAM_DOWN) {
        // Set the image sizes
        params_.setPreviewSize(cfg.previewSize); 
        params_.setVideoSize(cfg.previewSize); 

        // Set image format. Pretty much only YUV420sp
        params_.setPreviewFormat(cfg.previewFormat);

        // Leave these for 60 fps
        params_.setPreviewFpsRange(params_.getSupportedPreviewFpsRanges()[2]);
        params_.setVideoFPS(params_.getSupportedVideoFps()[0]);

        // Set picture parameters
        // params_.setFocusMode(cfg.focusMode);
        // params_.setWhiteBalance(cfg.whiteBalance);
        // params_.setISO(cfg.ISO);
        params_.setSharpness(cfg.sharpness);
        params_.setBrightness(cfg.brightness);
        params_.setContrast(cfg.contrast);

        params_.set("raw-size", "640x480");
    }

    if (params_.commit() != 0) {
        printf("Commit failed\n");
        return EXIT_FAILURE;
    }


    config_ = cfg;
}

void SnapCam::updateGain(int gain) {
    params_.setManualGain(gain);
    if(params_.commit() != 0) {
        printf("Could not update gain!\n");
    }
}

void SnapCam::updateExposure(int exposure) {
    params_.setManualExposure(exposure);
    if(params_.commit() != 0) {
        printf("Could not update exposure!\n");
    }
}

void SnapCam::start() {
    // Start the camera preview and recording. Will start pushing frames asynchronously to the callbacks
    camera_->startPreview();
    camera_->startRecording();
}

SnapCam::~SnapCam() {
    camera_->stopRecording();
    camera_->stopPreview();

    /* release camera device */
    camera::ICameraDevice::deleteInstance(&camera_);
}

void SnapCam::onError() {
    printf("camera error!, aborting\n");
    exit(EXIT_FAILURE);
}

void SnapCam::onPreviewFrame(camera::ICameraFrame *frame) {
    if (!cb_ || config_.func != CAM_DOWN) { return; }
    cb_(frame);
}

void SnapCam::onVideoFrame(camera::ICameraFrame *frame) {
    if (!cb_ || config_.func != CAM_FORWARD) { return; }
    cb_(frame);
}

void SnapCam::setListener(CallbackFunction fun) {
    cb_ = fun;
}

int SnapCam::cameraType() {
    return config_.func;
}
