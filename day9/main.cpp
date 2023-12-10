#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <cctype>
#include <vector>
#include <fmt/core.h>
#include <re2/re2.h>

class OasisSensor {
  public:
    struct Reading {
        std::vector<std::vector<int32_t>> dataset{};
        
        bool is_complete() {
            std::vector<int32_t> empty(dataset.back().size());
            return (dataset.back() == empty);
        }

        std::string to_string() {
            std::string retval;
            uint8_t indent{};
            for (auto& data : dataset) {
                for (auto& val : data) {
                    retval += fmt::format("{:>4} ", val);
                }
                retval = fmt::format("{}\n{:>{}}", retval, " ", indent);
                indent += 3;
            }

            return retval;
        }
    };

    OasisSensor (const std::string& filepath) {
        std::ifstream input(filepath, std::ios::binary);
        input_.swap(input);
        if (!input_.is_open()) {
            fmt::print("File permissions?\n");
        }
    }

    uint32_t process() {
        uint32_t retval{};

        // Read in
        for (std::string node; std::getline(input_, node);) {
            std::string_view node_v(node.begin(), node.end());

            Reading reading{};
            reading.dataset.push_back({});
            int32_t value{};
            while (RE2::FindAndConsume(&node_v, "(-*[[:digit:]]+) *", &value)) {
                reading.dataset.back().push_back(value);
            }
            readings_.push_back(reading);
        }

        // Generate diffs
        for (auto& reading : readings_) {
            while (!reading.is_complete()) {
                std::vector<int32_t> delta{};
                auto& data = reading.dataset.back();
                for (auto i = 0; i < data.size() - 1; i++) {
                    //fmt::print("{}\n", (data[i] - data[i+1]));
                    delta.push_back(data[i] - data[i+1]);
                }
                reading.dataset.push_back(delta);
            }
        }

        for (auto& reading : readings_) {
          fmt::print("{}\n", reading.to_string());
        }
        
        // Extrapolate
        for (auto& reading : readings_) {
            reading.dataset.back().push_back(0);
            auto previous = reading.dataset.rbegin();
            for (auto data = reading.dataset.rbegin() + 1; data != reading.dataset.rend(); data++) {
                data->push_back(data->back() - previous->back());
                previous = data;
            }
            retval += reading.dataset.front().back();
        }

        for (auto& reading : readings_) {
          fmt::print("{}\n", reading.to_string());
        }

        return retval;
    }

  private:
    std::ifstream input_{};
    std::vector<Reading> readings_{};
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

    OasisSensor oasis(file);

    auto final_val = oasis.process();

    fmt::print("Final Value: {}\n", final_val);
    
    return 0;
}

