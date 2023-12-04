#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <cctype>

int main(int argc, char* arcv[]) {
    if (argc == 1) {
        std::cout << "Pass in the path to the file dingus." << std::endl;
        return 1;
    }

    std::filesystem::path file(arcv[1]);
    if (!std::filesystem::exists(file)) {
        std::cout << "Pass in the CORRECT path to the file dingus." << std::endl;
        return 1;
    }

    std::ifstream input(file, std::ios::binary);
    if (!input.is_open()) {
        std::cout << "File permissions?" << std::endl;
        return 1;
    }

    uint32_t value{0};
    for (std::string line; std::getline(input, line);) {
        // Find first number
        std::string first;
        for (auto& num : line) {
            if (std::isdigit(num)){
                first = num;
                break;
            }
        }

       // Find last number
       std::string last;
       for (auto num = line.rbegin(); num != line.rend(); num++) {
           if (std::isdigit(*num)){
               last = *num;
               break;
           }
       }

        // Concat
        value += std::stoi(first+last);
    }

    std::cout << "Final value: " << value << std::endl;

    return 0;
}
