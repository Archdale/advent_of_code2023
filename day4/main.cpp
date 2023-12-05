#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <cctype>
#include <vector>
#include <fmt/core.h>
#include <re2/re2.h>

using re2::RE2;

class ScratchCard {
  public:
    struct Game {
        uint32_t game_num;
        std::vector<uint32_t> winning;

        uint32_t game_value{};
    };

    ScratchCard(const std::string& filepath) {
        std::ifstream input(filepath, std::ios::binary);
        input_.swap(input);
        if (!input_.is_open()) {
            fmt::print("File permissions?\n");
        }
    }

    uint32_t process() {
        uint32_t retval{};

        for (std::string line; std::getline(input_, line);) {
            std::string_view line_v(line.begin(), line.end());

            Game game{};
            // Populate game number
            auto game_num_prefix =  line_v.find(':');
            RE2::PartialMatch(line_v.substr(0,game_num_prefix),
                              "([[:digit:]]+)", &(game.game_num));

            //fmt::print("Game Num: {}\n", game.game_num);

            line_v.remove_prefix(game_num_prefix+1);

            // Populate Winning
            auto number_split =  line_v.find('|');
            auto winning_cards = line_v.substr(0, number_split);
            uint32_t win_val{};
            while (RE2::FindAndConsume(&winning_cards, "([[:digit:]]+)", &win_val)) {
                game.winning.push_back(win_val);
            }

            //fmt::print("Winning Num:");
            //for (auto& num : game.winning) {
            //    fmt::print(" {}", num);
            //}
            //fmt::print("\n");

            line_v.remove_prefix(number_split+1);

            // Check if they win
            uint32_t card_val{};
            while (RE2::FindAndConsume(&line_v, "([[:digit:]]+)", &card_val)) {
                for (auto& winner : game.winning) {
                    if (winner == card_val) {
                        if (game.game_value == 0) {
                            game.game_value++;
                        }  else {
                            game.game_value = game.game_value * 2;
                        }
                    }
                }
            }
            //fmt::print("Game {}: {}\n", game.game_num, game.game_value);
            retval += game.game_value;

        }

        return retval;
    }

  private:

    std::ifstream input_{};
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

    //std::ifstream input(file_, std::ios::binary);
    //if (!input.is_open()) {
    //    fmt::print("File permissions?\n");
    //    return 1;
    //}

    ScratchCard part(file);

    auto final_val = part.process();

    fmt::print("Final Value: {}\n", final_val);
    
    return 0;
}
