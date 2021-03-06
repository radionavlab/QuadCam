#pragma once

#include <functional>
#include <camera.h>
#include <camera_parameters.h>
#include <iostream>

// workaround to only have to define these once
struct CameraSizes {
	static camera::ImageSize FourKSize()        {static camera::ImageSize is(4096, 2160); return is;};
	static camera::ImageSize UHDSize()          {static camera::ImageSize is(3840, 2160); return is;};
	static camera::ImageSize FHDSize()          {static camera::ImageSize is(1920, 1080); return is;};
	static camera::ImageSize HDSize()           {static camera::ImageSize is(1280, 720); return is;};
	static camera::ImageSize VGASize()          {static camera::ImageSize is(640, 480); return is;};
	static camera::ImageSize stereoVGASize()    {static camera::ImageSize is(1280, 480); return is;};
	static camera::ImageSize QVGASize()         {static camera::ImageSize is(320, 240); return is;};
	static camera::ImageSize stereoQVGASize()   {static camera::ImageSize is(640, 240); return is;};
};

enum CamFunction {
    CAM_FUNC_UNKNOWN = -1,
    CAM_FORWARD = 0,
    CAM_DOWN = 1,
};

/**
*  Helper class to store all parameter settings
*/
struct CamConfig {
        int cameraId;
        std::string focusMode;
        std::string whiteBalance;
        std::string ISO;
        std::string previewFormat;
        int sharpness;
        int brightness;
        int contrast;
	int exposure;
	int gain;
	camera::ImageSize previewSize;
	camera::ImageSize pictureSize;
        int func;
};

// Callback function.
typedef std::function<void(camera::ICameraFrame *frame)> CallbackFunction;

/**
 * CLASS  QuadCam
 *
 * inherits ICameraListers which provides core functionality
 */
class QuadCam : camera::ICameraListener
{
public:
        QuadCam();
	QuadCam(const CamConfig& cfg);
	~QuadCam();

	void SetListener(CallbackFunction fun);  // register a function callback
        void Start();
        void Stop();
        int CameraType();
        void Configure(const CamConfig& cfg);

        void UpdateGain(int gain);
        void UpdateExposure(int exposure);

	/* listener methods */
	virtual void onError();
        virtual void onVideoFrame(camera::ICameraFrame *frame);
        virtual void onPreviewFrame(camera::ICameraFrame *frame);

        static const CamConfig DEFAULT_CONFIG;

private:
	void Initialize();

	camera::ICameraDevice *camera_;
	camera::CameraParams params_;
	CamConfig config_;
	CallbackFunction cb_;
};

