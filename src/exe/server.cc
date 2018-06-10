// Author: Tucker Haydon

#include "camera_server.h"

#include <cstdlib>
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

    ConfigureSigHandler();
    quadcam::CameraServer server("/tmp/camera_server");
    server.StartCamera();
    while(OK && sleep(1) == 0);
    return EXIT_SUCCESS;
}
