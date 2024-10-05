#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << " Failed to pass the filename";
        return 1;
    }

    std::string filename = argv[1];
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << " Failed to open the file";
        return 1;
    }

    std::string data;
    while (std::getline(std::cin, data)) {
        std::reverse(data.begin(), data.end());
        file << data << std::endl;
    }

    file.close();
    return 0;
}