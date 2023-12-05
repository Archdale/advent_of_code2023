#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <cctype>
#include <vector>
#include <fmt/core.h>

class PartNum {
  public:
    struct NumVal {
        std::string value{};

        uint32_t num1{};
        uint32_t num2{};

        bool add_ratio(uint32_t ratio) {
            if (num1 == 0) {
                num1 = ratio; 
                return false;
            } else if (num2 == 0) {
                num2 = ratio; 
                return true;
            } else {
                return false;
            }
        } 

        uint32_t get_ratio() {
            return num1 * num2;
        }

        char at(size_t i) {
            return value.at(i);
        }

        bool empty() {
            return value.empty();
        }

        bool is_dig() {
            return (!value.empty() && std::isdigit(value.at(0)));
        }

        std::string to_string() {
            return fmt::format("{} {} {}", num1, value, num2);
        }
    };

    PartNum(std::ifstream file) : file_(std::move(file)) {}

    uint32_t process() {
        uint32_t retval{};

        std::vector<NumVal> current;
        for (std::string line; std::getline(file_, line);) {
            std::string_view line_v(line.begin(), line.end());

            current = process_line(line_v);
            
            if (!previous_.empty()) {
                for(size_t i = 0; i < previous_.size(); i++) {
                    if (previous_.at(i).empty()) {
                        continue;
                    }

                    if (!std::isdigit(previous_.at(i).at(0))) {
                        // Check before symbol
                        if (i != 0 &&
                            previous_.at(i-1).is_dig()) {
                            
                            if (previous_.at(i).add_ratio(std::stol(previous_.at(i-1).value))) {
                                retval += previous_.at(i).get_ratio();
                            }
                            
                        }

                        // Check after symbol
                        if ((i != previous_.size() - 1) &&
                            previous_.at(i+1).is_dig()) {

                            if (previous_.at(i).add_ratio(std::stol(previous_.at(i+1).value))) {
                                retval += previous_.at(i).get_ratio();
                            }

                        }

                        // Check current line
                        if (current.at(i).is_dig()){

                            if (previous_.at(i).add_ratio(std::stol(current.at(i).value))) {
                                retval += previous_.at(i).get_ratio();
                            }
                        } else {

                            if (i != 0 &&
                                current.at(i-1).is_dig()) {

                                if (previous_.at(i).add_ratio(std::stol(current.at(i-1).value))) {
                                    retval += previous_.at(i).get_ratio();
                                }

                            }

                            if ((i != previous_.size() - 1) &&
                                current.at(i+1).is_dig()) {
                                if (previous_.at(i).add_ratio(std::stol(current.at(i+1).value))) {
                                    retval += previous_.at(i).get_ratio();
                                }

                            }
                        }
                    }

                }

                for(size_t i = 0; i < current.size(); i++) {
                    if (current.at(i).empty()) {
                        continue;
                    }
                    
                    if (!std::isdigit(current.at(i).at(0))) {
                        // Check previous_ line
                        if (previous_.at(i).is_dig()) {
                            if (current.at(i).add_ratio(std::stol(previous_.at(i).value))) {
                                retval += current.at(i).get_ratio();
                            }


                        } else {
                            if (i != 0 &&
                                previous_.at(i-1).is_dig()) {
                                if (current.at(i).add_ratio(std::stol(previous_.at(i-1).value))) {
                                    retval += current.at(i).get_ratio();
                                }

                            }


                            if ((i != previous_.size() - 1) &&
                                previous_.at(i+1).is_dig()) {
                                if (current.at(i).add_ratio(std::stol(previous_.at(i+1).value))) {
                                    retval += current.at(i).get_ratio();
                                }

                            }
                        }
                    }
                }
            }

            previous_ = current;
        }
        // Last line check
        for(size_t i = 0; i < current.size(); i++) {
            if (current.at(i).empty()) {
                continue;
            }

            if (!std::isdigit(current.at(i).at(0))) {
                // Check before symbol
                if (i != 0 && current.at(i-1).is_dig()) {

                    if (current.at(i).add_ratio(std::stol(current.at(i-1).value))) {
                        retval += current.at(i).get_ratio();
                    }
                }

                // Check after symbol
                if ((i != current.size() - 1) && current.at(i+1).is_dig()) {

                    if (current.at(i).add_ratio(std::stol(current.at(i+1).value))) {
                        retval += current.at(i).get_ratio();
                    }
                }
            }
        }

        return retval;
    };

  private:
    std::vector<NumVal> process_line(std::string_view line) {
        std::vector<NumVal> retval;
        std::string num_str{};
        uint32_t index{};

        for (auto& num : line) {
            if (num == '.' || std::iscntrl(num)) {
                if (!num_str.empty()) {
                    for (size_t i = 0; i < num_str.length(); i++) {
                        retval.push_back(NumVal{num_str,false});
                    }
                    num_str.clear();
                }

                if (num == '.') {
                    retval.push_back(NumVal{"",false});
                }
                index++;
                continue;
            }

            if (std::isdigit(num)){
                num_str += num;
            } else if (num == '*') {
                // A symbol
                if (!num_str.empty()) {
                    for (size_t i = 0; i < num_str.length(); i++) {
                        retval.push_back(NumVal{num_str,false});
                    }
                    num_str.clear();
                }
                retval.push_back(NumVal{std::string(1,num),false});
            } else {
                retval.push_back(NumVal{"",false});
            }

            index++;
        }

        return retval;
    }

    std::ifstream file_{};

    std::vector<NumVal> previous_;
};

int main(int argc, char* arcv[]) {
    if (argc == 1) {
        fmt::print("Pass in the path to the file dingus.\n");
        return 1;
    }

    std::filesystem::path file(arcv[1]);
    if (!std::filesystem::exists(file)) {
        fmt::print("Pass in the CORRECT path to the file dingus.\n");
        return 1;
    }

    std::ifstream input(file, std::ios::binary);
    if (!input.is_open()) {
        fmt::print("File permissions?\n");
        return 1;
    }

    PartNum part(std::move(input));

    auto final_val = part.process();

    fmt::print("Final Value: {}\n", final_val);
    
    return 0;
}

