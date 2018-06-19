// Author: Tucker Haydon

#include "camera_server.h"
#include <boost/program_options.hpp>

#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <set>

bool OK{true};

namespace po = boost::program_options;

namespace 
{ 
    const size_t ERROR_IN_COMMAND_LINE = 1; 
    const size_t SUCCESS = 0; 
    const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
    const size_t ERROR_BAD_INPUT = 3;
    const std::set<std::string> VALID_CAMERA_SET = {"forward", "down"};
    const std::set<std::string> VALID_RESOLUTION_SET = {"4k", "1080p", "720p", "VGA"};
} // namespace

po::variables_map ParseCommandLineOptions(int argc, char** argv) {
    try { 
        po::options_description desc("Options"); 
        desc.add_options() 
          ("help,h", "Creates a sever to distribute camera images") 
          ("example,e", "./server -s /tmp/forward-server -c forward -r VGA")
          ("server,s", po::value<std::string>()->required(), "Full path of camera server") 
          ("camera,c", po::value<std::string>()->required(), "forward, down") 
          ("resolution,r", po::value<std::string>()->required(), "4k, 1080p, 720p, VGA");
 
        po::variables_map vm; 
        try { 
            // Can throw
            po::store(po::parse_command_line(argc, argv, desc),  vm);
 
            if (vm.count("help")) { 
                std::cout << "Help Options" << std::endl << desc << std::endl; 
                exit(EXIT_SUCCESS);
            } 
 
            po::notify(vm); // throws on error
            if(
                VALID_CAMERA_SET.find(vm["camera"].as<std::string>()) == VALID_CAMERA_SET.end() || 
                VALID_RESOLUTION_SET.find(vm["resolution"].as<std::string>()) == VALID_RESOLUTION_SET.end()) {
                std::cerr << "Invalid camera or resolution" << std::endl;
                exit(ERROR_BAD_INPUT);
            }

            return vm;
        } 
        catch(po::error& e) { 
          std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
          std::cerr << desc << std::endl; 
          exit(ERROR_IN_COMMAND_LINE); 
        } 
  
    } 
    catch(std::exception& e) { 
        std::cerr << "Unhandled Exception reached the top of main: " 
            << e.what() << ", application will now exit" << std::endl; 
        exit(ERROR_UNHANDLED_EXCEPTION);
    } 
}

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
    po::variables_map opt = ParseCommandLineOptions(argc, argv);
    const std::string server_path = opt["server"].as<std::string>();
    const std::string camera_type = opt["camera"].as<std::string>();
    const std::string camera_resolution = opt["resolution"].as<std::string>();
    quadcam::CameraServer server(server_path, camera_type, camera_resolution);
    server.StartCamera();
    while(OK && sleep(1) == 0);
    return EXIT_SUCCESS;
}
