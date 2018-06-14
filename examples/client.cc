// Author: Tucker Haydon

#include "camera_client.h"

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <chrono>

bool OK{true};

void SigHandler(int s) {
    OK = false;
}

long long CurrentTimeMillis() {
    return std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void TicToc() {
    static long long previous = 0L;
    static bool tic = false;

    long long now = std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    if(tic) {
       tic = false;
       std::cout << "Time Elapsed: " << now - previous << " ms" << std::endl; 
    } else {
        tic = true;
        previous = now;
    }
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
