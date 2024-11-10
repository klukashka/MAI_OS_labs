// 20

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>

#define FILE_SIZE 4096
const int BUFFER_SIZE = 1024;

bool child_ready = false;

void handle_child_ready(int) {
    child_ready = true;
}

int main() {
    signal(SIGUSR2, handle_child_ready);

    int fd1, fd2, shr_fd;
    char* mapped;

    std::string filename1, filename2;

    std::cout << "Enter the name of File 1: ";
    std::getline(std::cin, filename1);

    std::cout << "Enter the name of File 2: ";
    std::getline(std::cin, filename2);

    fd1 = open(filename1.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd1 == -1) {
        perror("Error during opening the first file");
        return 1;
    }

    fd2 = open(filename2.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd2 == -1) {
        perror("Error during opening the second file");
        close(fd1);
        return 1;
    }

     
    int shared_fd = shm_open("/mapped", O_CREAT | O_RDWR, 0644);
    if (shared_fd == -1) {
        perror("Error during creating mapped memory");
        close(fd1);
        close(fd2);
        munmap(mapped, BUFFER_SIZE);
        shm_unlink("/mapped");
        return 1;
    }
    ftruncate(shared_fd, BUFFER_SIZE);
    mapped = (char*)mmap(nullptr, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0);

     
    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 == 0) {
        execl("./child", "child", std::to_string(fd1).c_str(), (char*)NULL);
        perror("Error during creatign the first child process");
        exit(1);
    }

    pid2 = fork();
    if (pid2 == 0) {
        execl("./child", "child", std::to_string(fd2).c_str(), (char*)NULL);
        perror("Error during creating the second child process");
        exit(1);
    }

    std::string data;
    while (std::getline(std::cin, data)) {
        std::memset(mapped, 0, BUFFER_SIZE);
        std::strncpy(mapped, data.c_str(), BUFFER_SIZE);

        if (data.length() > 10) {
            kill(pid2, SIGUSR1);   
        } else {
            kill(pid1, SIGUSR1);   
        }

        child_ready = false;
        while (!child_ready) {
            pause();   
        }
    }

    close(fd1);
    close(fd2);
    munmap(mapped, BUFFER_SIZE);
    shm_unlink("/mapped");

    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);
    return 0;
}