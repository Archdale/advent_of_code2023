#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <cctype>
#include <re2/re2.h>

using re2::RE2;

class ValidateGame {
  public:
    ValidateGame(std::string filepath, uint32_t red,uint32_t green, uint32_t blue) 
    : file_(filepath), red_(red), green_(green), blue_(blue) {}

    bool invalid_color(uint32_t count, std::string color) {
        if (color == "red") {
            if (count > red_) {
                return true;
            }
        } else if (color == "green") {
            if (count > green_) {
                return true;
            }
        } else {
            if (count > blue_) {
                return true;
            }
        }
        return false;
    }

    bool process_set(std::string_view set) {
        bool invalid{false};
        uint32_t count{0};
        std::string color{};

        auto inner_split = set.find(',');
        while (inner_split != std::string::npos) {
            auto one_color = set.substr(0,inner_split);
            RE2::PartialMatch(one_color,
                "([[:digit:]]+) (red|green|blue)",
                &count, &color);

            invalid = invalid_color(count, color);

            if (invalid) {
                return true;
            }

            set.remove_prefix(inner_split+2);
            inner_split = set.find(',');
        }

        if (invalid) {
            return true;
        }

        // Last Color
        RE2::PartialMatch(set,
            "([[:digit:]]+) (red|green|blue)",
            &count, &color);


        invalid = invalid_color(count, color);

        if (invalid) {
            return true;
        }

        return false;
    }
    

    uint32_t process() {
        std::ifstream input(file_, std::ios::binary);
        if (!input.is_open()) {
            std::cout << "File permissions?" << std::endl;
            return 1;
        }

        uint32_t retval{0};

        for (std::string line; std::getline(input, line);) {
            std::string_view line_v(line.begin(), line.end());
            uint32_t game_id{};
            //first whitespace to colon is game ID
            auto start = line_v.find(':');

            RE2::PartialMatch(
                line_v.substr(0,start),
                "Game ([[:digit:]]+)",
                &game_id);

            line_v.remove_prefix(start+2);


            // while not npos keep getting sets
            bool invalid{false};
            start = line_v.find(';');
            while (start != std::string::npos) {
                auto set = line_v.substr(0,start);

                invalid = process_set(set);

                if (invalid) {
                    break;
                }

                line_v.remove_prefix(start+1);
                start = line_v.find(';');
            }

            if (invalid) {
                // This game is junk
                continue;
            }

            // And then the last set
            invalid = process_set(line_v);
            if (invalid) {
                // This game is junk
                continue;
            }

            retval += game_id;
        }

        return retval;
    }

  private:
    std::filesystem::path file_{};
    uint32_t red_{0};
    uint32_t green_{0};
    uint32_t blue_{0};
};

int main(int argc, char* arcv[]) {
    uint32_t max_red{0};
    uint32_t max_green{0};
    uint32_t max_blue{0};

    if (argc == 1) {
        std::cout << "Pass in the path to the file dingus." << std::endl;
        return 1;
    }

    std::filesystem::path file(arcv[1]);
    if (!std::filesystem::exists(file)) {
        std::cout << "Pass in the CORRECT path to the file dingus." << std::endl;
        return 1;
    }

    if (argc == 2) {
        std::cout << "Pass in the parameters dingus." << std::endl;
        return 1;
    }

    max_red = std::stoi(arcv[2]);
    max_green = std::stoi(arcv[3]);
    max_blue = std::stoi(arcv[4]);

    ValidateGame game(file, max_red, max_green, max_blue);

    auto final_val = game.process();

    std::cout << "final value: " << final_val << std::endl;

    return 0;
}
