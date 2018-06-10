// Author: Tucker Haydon

#include "camera_client.h"
#include "utils.h"

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <signal.h>

bool OK{true};

void SigHandler(int s) {
    OK = false;
}

void ConfigureSigHandler() {

    struct sigaction sigIntHandler;
    
    sigIntHandler.sa_handler = SigHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    
    sigaction(SIGINT, &sigIntHandler, NULL);
    sigaction(SIGKILL, &sigIntHandler, NULL);
}

int main(int argc, char** argv) {
    quadcam::CameraClient client("/tmp/camera_server");

    while(OK) {
        TicToc();
        quadcam::FrameData frame_data = client.RequestFrame();
        TicToc();
        std::cout << frame_data.meta_data.width << ", " << frame_data.meta_data.height << std::endl;
    }

    return EXIT_SUCCESS;
}
