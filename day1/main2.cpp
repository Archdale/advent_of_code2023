#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <regex>

const std::regex FORWARD("(1|2|3|4|5|6|7|8|9|one|two|three|four|five|six|seven|eight|nine)");

std::string word_to_num_str(const std::string& word) {
    if (word == "one") {
        return "1";
    } else if (word == "two") {
        return "2";
    } else if (word == "three") {
        return "3";
    } else if (word == "four") {
        return "4";
    } else if (word == "five") {
        return "5";
    } else if (word == "six") {
        return "6";
    } else if (word == "seven") {
        return "7";
    } else if (word == "eight") {
        return "8";
    } else if (word == "nine") {
        return "9";
    }
    return word;
}

int main(int argc, char* arcv[]) {
    if (argc == 1) {
        std::cout << "Pass in the path to the file, dingus." << std::endl;
        return 1;
    }

    std::filesystem::path file(arcv[1]);
    if (!std::filesystem::exists(file)) {
        std::cout << "Pass in the CORRECT path to the file, dingus." << std::endl;
        return 1;
    }

    std::ifstream input(file, std::ios::binary);
    if (!input.is_open()) {
        std::cout << "File permissions?" << std::endl;
        return 1;
    }

    uint32_t value{0};
    for (std::string line; std::getline(input, line);) {
        std::vector<std::string> values{};
        std::string first{};
        std::string last{};

        for (std::smatch sm; std::regex_search(line, sm, FORWARD);) {
            values.push_back(sm.str()); 
            line = line.substr(1,line.size());
        }
        
        if ((values.front() == "") | (values.back() == "")) {
            return 1;
        }
        
        first = word_to_num_str(values.front());
        last  = word_to_num_str(values.back());

        value += std::stoi(first + last);
    }

    std::cout << "Final value: " << value << std::endl;

    return 0;
}
