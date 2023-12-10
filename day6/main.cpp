#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <cctype>
#include <vector>
#include <fmt/core.h>
#include <re2/re2.h>

class Races {
    struct Race {
        uint32_t time;
        uint32_t distance;
        uint32_t min;
        uint32_t max;
    };

  public:
    Races(const std::string& filepath) {
        std::ifstream input(filepath, std::ios::binary);
        input_.swap(input);
        if (!input_.is_open()) {
            fmt::print("File permissions?\n");
        }
    }
    uint32_t process() {
        uint32_t retval{};
    
        populate_data();

        for (auto& race : races_) {
            // Find lower bound
            for (uint32_t held = 1; held < race.time; held++) {
                uint32_t moving = race.time - held;             
                uint32_t travelled = held * moving;
                if (travelled > race.distance) {
                    race.min = held;
                    break;
                }
            }

            // Find upper bound
            for (uint32_t held = (race.time - 1); held > 0; held--) {
                uint32_t moving = race.time - held;             
                uint32_t travelled = held * moving;
                if (travelled > race.distance) {
                    race.max = held;
                    break;
                }
            }
        }

        // Debug
        for (auto& race : races_) {
            fmt::print("{:>3}mm in {:>3}ms; Min: {}, Max: {}\n", race.distance, race.time, race.min, race.max);
            // +1 cause its inclusive
            uint32_t total_r = (race.max - race.min) + 1;
            if (retval == 0) {
                retval += total_r;
            } else {
                retval *= total_r;
            }
        }


        return retval;
    }

  private:

    void populate_data() {
        std::string time_line;
        // Read times
        std::getline(input_, time_line);
        std::string_view time_v(time_line.begin(), time_line.end());
        uint32_t race_time{};
        while (RE2::FindAndConsume(&time_v, "([[:digit:]]+)", &race_time)) {
            Race race{};
            race.time = race_time;
            races_.push_back(race);
        }

        // Read distances
        std::string dist_line;
        std::getline(input_, dist_line);
        std::string_view dist_v(dist_line.begin(), dist_line.end());
        uint32_t distance{};
        uint32_t index{};
        while (RE2::FindAndConsume(&dist_v, "([[:digit:]]+)", &distance)) {
            races_[index].distance = distance;
            index++;
        }
    }

    std::ifstream input_{};
    std::vector<Race> races_{};
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

    Races part(file);

    auto final_val = part.process();

    fmt::print("Final Value: {}\n", final_val);
    
    return 0;
}
