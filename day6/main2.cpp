#include <algorithm>
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
        uint64_t time;
        uint64_t distance;
        uint64_t min;
        uint64_t max;
    };

  public:
    Races(const std::string& filepath) {
        std::ifstream input(filepath, std::ios::binary);
        input_.swap(input);
        if (!input_.is_open()) {
            fmt::print("File permissions?\n");
        }
    }
    uint64_t process() {
        uint64_t retval{};
    
        populate_data();

        for (auto& race : races_) {
            // Find lower bound
            for (uint64_t held = 1; held < race.time; held++) {
                uint64_t moving = race.time - held;             
                uint64_t travelled = held * moving;
                if (travelled > race.distance) {
                    race.min = held;
                    break;
                }
            }

            // Find upper bound
            for (uint64_t held = (race.time - 1); held > 0; held--) {
                uint64_t moving = race.time - held;             
                uint64_t travelled = held * moving;
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
            uint64_t total_r = (race.max - race.min) + 1;
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
        auto just_time =  time_line.substr(time_line.find(':')+1);
        just_time.erase(std::remove_if(just_time.begin(), just_time.end(), [](auto x) { return std::isspace(x);}), just_time.end());
        std::string_view time_v(just_time.begin(), just_time.end());
        uint64_t race_time{};
        while (RE2::FindAndConsume(&time_v, "([[:digit:]]+)", &race_time)) {
            Race race{};
            race.time = race_time;
            races_.push_back(race);
        }

        // Read distances
        std::string dist_line;
        std::getline(input_, dist_line);
        auto just_dist =  dist_line.substr(dist_line.find(':')+1);
        just_dist.erase(std::remove_if(just_dist.begin(), just_dist.end(), [](auto x) { return std::isspace(x);}), just_dist.end());
        std::string_view dist_v(just_dist.begin(), just_dist.end());
        uint64_t distance{};
        uint64_t index{};

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

