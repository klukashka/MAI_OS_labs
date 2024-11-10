#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <csignal>

const int BUFFER_SIZE = 1024;

char* mapped_memory;
int file;
pid_t parent_pid;

void handle_signal(int) {
    std::string data(mapped_memory);
    std::reverse(data.begin(), data.end());

    if (write(file, data.c_str(), data.size()) == -1) {
        perror("Error during writing into the file");
    }
    if (write(file, "\n", 1) == -1) {
        perror("Error during writing into the file");
    }

    kill(parent_pid, SIGUSR2);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: no file descriptor provided" << std::endl;
        return 1;
    }

    file = std::stoi(argv[1]);
    parent_pid = getppid();

    int shared_fd = shm_open("/mapped", O_RDONLY, 0644);
    if (shared_fd == -1) {
        perror("Error during connecting to the mapped file");
        return 1;
    }
    mapped_memory = (char*)mmap(nullptr, BUFFER_SIZE, PROT_READ, MAP_SHARED, shared_fd, 0);

    signal(SIGUSR1, handle_signal);

    while (true) {
        pause();
    }

    close(file);
    munmap(mapped_memory, BUFFER_SIZE);
    return 0;
}