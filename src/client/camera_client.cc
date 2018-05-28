// Author: Tucker Haydon

#include "camera_client.h"

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace snapcam {

CameraClient::CameraClient(const std::string& path)
    : path_{path} {
};

void CameraClient::ReportError(const char* msg) {
   std::cout << msg << std::endl; 
   std::cout << "Errno: " << errno << std::endl;
   exit(EXIT_FAILURE);
};

FrameData CameraClient::RequestFrame() {
    // RAII object for connection
    ConnectionRAII connection(this);

    static const int BUFFER_SIZE = 256;
    struct msghdr msg = {0};

    char m_buffer[BUFFER_SIZE];
    struct iovec io = { .iov_base = m_buffer, .iov_len = sizeof(m_buffer) };
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;

    char c_buffer[BUFFER_SIZE];
    msg.msg_control = c_buffer;
    msg.msg_controllen = sizeof(c_buffer);

    // Receive the message containing the file descriptor
    if (recvmsg(this->server_fd_, &msg, 0) < 0) {
        this->ReportError("Could not receive message.");
    }

    // Extract the meta data from the message
    FrameMetaData meta_data;
    meta_data.Deserialize(std::string((char*)io.iov_base));

    struct cmsghdr * c_msg = CMSG_FIRSTHDR(&msg);
    unsigned char * c_data = CMSG_DATA(c_msg);

    // Extract file descriptor
    const int fd = *((int*) c_data);

    // Memory map the buffer into this process' virtual memory space
    // std::shared_ptr<int> sp( new int[10], std::default_delete<int[]>() );
    // uint8_t* data = (uint8_t *) mmap(NULL, meta_data.data_size, PROT_READ, MAP_SHARED, fd, 0);
    // memcpy(buff, data, meta_data.data_size);
    // close(fd);

    return FrameData{meta_data, NULL};
};

CameraClient::ConnectionRAII::ConnectionRAII(CameraClient* client)
    : client_{client} {

    struct sockaddr_un server_address;

    // Creating socket file descriptor
    if ((client_->server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        client_->ReportError("Client Socket Creation");
    }

    // Connect to unix port server
    memset(&server_address, '0', sizeof(struct sockaddr_un)); 
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, client_->path_.c_str(), sizeof(server_address.sun_path) -1); 

    if (connect(client_->server_fd_, (struct sockaddr *)&server_address, sizeof(struct sockaddr_un)) < 0) {
        client->ReportError("Client Socket Connection");
    }
}

CameraClient::ConnectionRAII::~ConnectionRAII() {
    if (shutdown(client_->server_fd_, SHUT_RDWR) < 0) {
        if (errno != ENOTCONN && errno != EINVAL) {
            client_->ReportError("Client Socket Shutdown");
        }
    }

    if (close(client_->server_fd_) < 0) { 
        client_->ReportError("Client Socket Close");
    }
}

};

