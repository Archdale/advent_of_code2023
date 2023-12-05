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
        bool is_counted{false};

        char at(size_t i) {
            return value.at(i);
        }

        bool counted() {
            return is_counted;
        }

        bool empty() {
            return value.empty();
        }

        bool is_dig() {
            return (!value.empty() && std::isdigit(value.at(0)));
        }
    };

    PartNum(std::string filepath) : file_(filepath) {}

    uint32_t process() {
        std::ifstream input(file_, std::ios::binary);
        if (!input.is_open()) {
            fmt::print("File permissions?\n");
            return 1;
        }

        uint32_t retval{};

        std::vector<NumVal> current;
        for (std::string line; std::getline(input, line);) {
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
                            previous_.at(i-1).is_dig() && !previous_.at(i-1).counted()) {
                            
                            retval += std::stol(previous_.at(i-1).value);
                            
                            for (auto j = (i-1); previous_.at(j).is_dig(); j--) {
                                previous_.at(j).is_counted = true;
                                if (j == 0) {
                                    break;
                                }
                            }
                        }

                        // Check after symbol
                        if ((i != previous_.size() - 1) &&
                            previous_.at(i+1).is_dig() &&
                            !previous_.at(i+1).counted()) {

                            retval += std::stol(previous_.at(i+1).value);

                            for (auto j = (i+1); j == previous_.size() || previous_.at(j).is_dig(); j++) {
                                previous_.at(j).is_counted = true;
                            }
                        }

                        // Check current line
                        if (current.at(i).is_dig() &&
                            !current.at(i).counted()) {
                            retval += std::stol(current.at(i).value);

                            //look left
                            for (auto j = (i); current.at(j).is_dig(); j--) {
                                current.at(j).is_counted = true;
                                if (j == 0) {
                                    break;
                                }
                            }

                            //look right
                            for (auto j = (i); j == current.size() || current.at(j).is_dig(); j++) {
                                current.at(j).is_counted = true;
                            }

                        } else {
                            if (i != 0 &&
                                current.at(i-1).is_dig() &&
                                !current.at(i-1).counted()) {

                                retval += std::stol(current.at(i-1).value);

                                //look left
                                for (auto j = (i-1); current.at(j).is_dig(); j--) {
                                    current.at(j).is_counted = true;
                                    if (j == 0) {
                                        break;
                                    }
                                }

                                //look right
                                for (auto j = (i-1); j == current.size() || current.at(j).is_dig(); j++) {
                                    current.at(j).is_counted = true;
                                }
                            }

                            if ((i != previous_.size() - 1) &&
                                current.at(i+1).is_dig() && 
                                !current.at(i+1).counted()) {
                                retval += std::stol(current.at(i+1).value);

                                //look left
                                for (auto j = (i+1); current.at(j).is_dig(); j--) {
                                    current.at(j).is_counted = true;
                                    if (j == 0) {
                                        break;
                                    }
                                }
                                
                                // Look right
                                for (auto j = (i+1); j == current.size() || current.at(j).is_dig(); j++) {
                                    current.at(j).is_counted = true;
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
                        if (previous_.at(i).is_dig() && 
                            !previous_.at(i).counted()) {
                            retval += std::stol(previous_.at(i).value);

                            // look left
                            for (auto j = (i); current.at(j).is_dig(); j--) {
                                current.at(j).is_counted = true;
                                if (j == 0) {
                                    break;
                                }
                            }

                            // look right
                            for (size_t j = (i); j == current.size() || current.at(j).is_dig(); j++) {
                                current.at(j).is_counted = true;
                            }

                        } else {
                            if (i != 0 &&
                                previous_.at(i-1).is_dig() &&
                                !previous_.at(i-1).counted()) {
                                retval += std::stol(previous_.at(i-1).value);

                                // look left
                                for (auto j = (i-1); current.at(j).is_dig(); j--) {
                                    current.at(j).is_counted = true;
                                    if (j == 0) {
                                        break;
                                    }
                                }

                                // look right
                                for (auto j = (i-1); j == current.size() || current.at(j).is_dig(); j++) {
                                    current.at(j).is_counted = true;
                                }
                            }


                            if ((i != previous_.size() - 1) &&
                                previous_.at(i+1).is_dig() &&
                                !previous_.at(i+1).counted()) {
                                retval += std::stol(previous_.at(i+1).value);

                                // look right
                                for (auto j = (i+1); j == current.size() || current.at(j).is_dig(); j++) {
                                    current.at(j).is_counted = true;
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
                if (i != 0 &&
                    current.at(i-1).is_dig() &&
                    !current.at(i-1).counted()) {

                    retval += std::stol(current.at(i-1).value);
                }

                // Check after symbol
                if ((i != current.size() - 1) &&
                    current.at(i+1).is_dig() &&
                    !current.at(i+1).counted()) {

                    retval += std::stol(current.at(i+1).value);
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
            } else {
                // A symbol
                if (!num_str.empty()) {
                    for (size_t i = 0; i < num_str.length(); i++) {
                        retval.push_back(NumVal{num_str,false});
                    }
                    num_str.clear();
                }
                retval.push_back(NumVal{std::string(1,num),false});
            }

            index++;
        }

        return retval;
    }

    std::filesystem::path file_{};

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

    PartNum part(file);

    auto final_val = part.process();

    fmt::print("Final Value: {}\n", final_val);
    
    return 0;
}
