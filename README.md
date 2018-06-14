# Quadcam
## System Description
Quadcam is a program that distributes images to subscribers by sharing a file descriptor to a Direct Memory Access (DMA) buffer. Current Snapdragon Flight camera implementations monopolize and consume camera images, only allowing one program to use the images at a time. This presents a challenge when multiple, independent programs require access to the images (saving the images, VIO, optic flow, hide-and-seek, etc). 

Quadcam addresses this problem by publishing an image's file descriptor, allowing other processes to access and copy the image into their own process memory space. This is advantageous over a traditional network-sharing protocol as it involves only a single copy operation. For small images (VGA or below) network solutions (sharing image using UNIX or TCP port) may be viable, but the time requires to copy the image into and out of the buffer multiple times becomes unacceptable for large ~20 MB 4k images.

At the moment, thee camera server provides no guarantee that the memory buffer will not be modified while the clients are copying data. The camera itself writes camera frames into the DMA buffer. There appears to be enough memory for 9 consecutive images to be stored in the buffer before previous images are overwritten. 4K images arrive at ~15-30 fps. Therefor, a client has fewer than 33 ms to copy an image before it it overwritten. 15 ms for a 60 fps VGA image. In the future, a reference counter will be implemented to permit clients to access an image for longer.

## Client
The camera client library provides an interface for C++ programs to receive camera images. The images are returned as FrameData objects that use shared pointers to manage the image data. No direct memory management is necessary.

## Example Client
An example client program is in the examples directory. The client requests an image from the server and prints out the wall time between consecutive frames. A client must link against the camera_client library installed in /usr/local/lib/quadcam and include the corresponding header file located in /usr/local/include/quadcam. Look at the example CMakeLists.txt file for an example of how to link against the library.

## Building and Running
Due to hardware constraints, both the client and server must be built and executed on the server. 

### Build
```
mkdir build
cd build
cmake ..
sudo make install
```

If an error occurs when building the example client, the client may have been built before the libraries and header files were installed. Try rebuilding.

### Running
Both the client and server must be run with root acces. Access to the camera is resticted to sudo users only. As a result, the file descriptor passed between the server and client is resticted to sudo users only. 

#### Server
```
sudo -s
./build/src/exe/server
```

#### Example Client
```
sudo -s
./build/examples/quad_cam_client
```
