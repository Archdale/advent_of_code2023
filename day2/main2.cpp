#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <re2/re2.h>
#include <fmt/core.h>

using re2::RE2;

class CountGame {
  public:
    struct ColorCount {
        uint32_t red{};
        uint32_t green{};
        uint32_t blue{};

        uint32_t cube_power() {
            return red * green * blue;
        }

        std::string to_string() {
            std::stringstream ss;
            ss << "Red: " << red;
            ss << "; Green: " << green;
            ss << "; Blue: " << blue;
            return ss.str();
        }

        ColorCount& operator+=(const ColorCount& rhs) {
            if (this->red < rhs.red) {
                this->red = rhs.red;
            }
            if (this->green < rhs.green) {
                this->green = rhs.green;
            }
            if (this->blue < rhs.blue) {
                this->blue = rhs.blue;
            }
            return *this;
        }

        friend ColorCount operator+(ColorCount lhs, const ColorCount& rhs) {
            if (lhs.red < rhs.red) {
                lhs.red = rhs.red;
            }
            if (lhs.green < rhs.green) {
                lhs.green = rhs.green;
            }
            if (lhs.blue < rhs.blue) {
                lhs.blue = rhs.blue;
            }
            return lhs;
        }
    };

    CountGame(std::string filepath) : file_(filepath) {}

    void fill_in_color(ColorCount& cc, uint32_t count, std::string color) {
        if (color == "red") {
            cc.red = count;
        } else if (color == "green") {
            cc.green = count;
        } else {
            cc.blue = count;
        }
    }

    ColorCount process_set(std::string_view set) {
        ColorCount retval{};
        uint32_t count{0};
        std::string color{};

        auto inner_split = set.find(',');
        while (inner_split != std::string::npos) {
            auto one_color = set.substr(0,inner_split);
            RE2::PartialMatch(one_color,
                "([[:digit:]]+) (red|green|blue)",
                &count, &color);

            fill_in_color(retval, count, color);

            set.remove_prefix(inner_split+2);
            inner_split = set.find(',');
        }

        // Last Color
        RE2::PartialMatch(set,
            "([[:digit:]]+) (red|green|blue)",
            &count, &color);

        fill_in_color(retval, count, color);

        return retval;
    }
    

    uint32_t process() {
        std::ifstream input(file_, std::ios::binary);
        if (!input.is_open()) {
            fmt::print("File permissions?\n");
            return 1;
        }

        uint32_t retval{0};

        for (std::string line; std::getline(input, line);) {
            std::string_view line_v(line.begin(), line.end());

            //first whitespace to colon is game ID
            auto start = line_v.find(':');

            line_v.remove_prefix(start+2);

            // while not npos keep getting sets
            ColorCount game{};
            start = line_v.find(';');
            while (start != std::string::npos) {
                auto set = line_v.substr(0,start);

                game += process_set(set);

                line_v.remove_prefix(start+1);
                start = line_v.find(';');
            }


            // And then the last set
            game += process_set(line_v);

            retval += game.cube_power();
        }

        return retval;
    }

  private:
    std::filesystem::path file_{};
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

    CountGame game(file);

    auto final_val = game.process();

    fmt::print("Final Value: {}\n", final_val);
    
    return 0;
}

