#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// 20

int main() {
    int pipe_1[2], pipe_2[2];                                     // 0 - reading, 1 - writing
    pid_t pid1, pid2;
    std::string filename1, filename2;

    std::cout << "Enter the name of File 1: ";
    std::getline(std::cin, filename1);

    std::cout << "Enter the name of File 2: ";
    std::getline(std::cin, filename2);

    if (pipe(pipe_1) == -1) {                                    // create pipe_1
        std::cerr << " pipe_1\n";
        return 1;
    }

    if (pipe(pipe_2) == -1) {                                    // create pipe_2
        std::cerr << " pipe_2\n";
        close(pipe_1[0]);
        close(pipe_1[1]);
        return 1;
    }

    pid1 = fork();                                              // first child process
    if (pid1 < 0) {
        std::cerr << " child1\n";
        close(pipe_1[0]);
        close(pipe_1[1]);
        close(pipe_2[0]);
        close(pipe_2[1]);
        return 1;
    }

    if (pid1 == 0) {
        close(pipe_1[1]);
        dup2(pipe_1[0], STDIN_FILENO);
        close(pipe_1[0]);
        close(pipe_2[0]);
        close(pipe_2[1]);

        execl("./child", "child", filename1.c_str(), (char*)NULL);  // launch the first child process
        std::cerr << " Failed to launch child1\n";
        exit(1);
    }

    pid2 = fork();                                              // the second child process
    if (pid2 < 0) {
        std::cerr << " Failed to fork child2\n";
        close(pipe_1[0]);
        close(pipe_1[1]);
        close(pipe_2[0]);
        close(pipe_2[1]);
        return 1;
    }

    if (pid2 == 0) {
        close(pipe_2[1]);
        dup2(pipe_2[0], STDIN_FILENO);
        close(pipe_2[0]);
        close(pipe_1[0]);
        close(pipe_1[1]);

        execl("./child", "child", filename2.c_str(), (char*)NULL);  // launch the second child process
        std::cerr << " Failed to launch child2\n";
        exit(1);
    }

    // Родительский процесс
    close(pipe_1[0]);
    close(pipe_2[0]);

    std::string data;
    while (std::getline(std::cin, data)) {
        if (data.size() <= 10) {
            write(pipe_1[1], data.c_str(), data.length());
            write(pipe_1[1], "\n", 1);
        } else {
            write(pipe_2[1], data.c_str(), data.length());
            write(pipe_2[1], "\n", 1);
        }
    }

    close(pipe_1[1]);
    close(pipe_2[1]);

    wait(NULL);
    wait(NULL);

    return 0;
}