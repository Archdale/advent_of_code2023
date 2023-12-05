#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <fmt/core.h>
#include <re2/re2.h>

using re2::RE2;

class ScratchCard {
  public:
    struct Game {
        uint32_t game_num;
        std::vector<uint32_t> winning;
        std::vector<uint32_t> cards;

        uint32_t cur_num_of{1};
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

        read_cards(); 

        retval = process_cards();

        // Debug print
        //for (auto& game : games_) {
        //    fmt::print("Game {:>2} ({}): Winning: ", game.game_num, game.cur_num_of);
        //    for (auto& winning : game.winning) {
        //        fmt::print(" {:>2}",winning);
        //    }
        //    fmt::print("; Cards: ");
        //    for (auto& card : game.cards) {
        //        fmt::print(" {:>2}", card);
        //    }
        //    fmt::print("\n");
        //}
        //fmt::print("\n");

        return retval;
    }

  private:
    void read_cards() {
        for (std::string line; std::getline(input_, line);) {
            std::string_view line_v(line.begin(), line.end());

            Game game{};
            // Populate game number
            auto game_num_prefix =  line_v.find(':');
            RE2::PartialMatch(line_v.substr(0,game_num_prefix),
                              "([[:digit:]]+)", &(game.game_num));

            line_v.remove_prefix(game_num_prefix+1);

            // Populate Winning
            auto number_split =  line_v.find('|');
            auto winning_cards = line_v.substr(0, number_split);
            uint32_t win_val{};
            while (RE2::FindAndConsume(&winning_cards, "([[:digit:]]+)", &win_val)) {
                game.winning.push_back(win_val);
            }

            line_v.remove_prefix(number_split+1);

            // Check if they win
            uint32_t card_val{};
            while (RE2::FindAndConsume(&line_v, "([[:digit:]]+)", &card_val)) {
                game.cards.push_back(card_val);
            }

            games_.push_back(game);
        }
    }

    uint32_t process_cards() {
        uint32_t retval{0};
        for (auto game = games_.begin(); game != games_.end(); game++) {
            // Get number of winning for this game
            for (uint32_t i = 0; i < game->cur_num_of; i++) {
                uint32_t num_winning{};
                for (auto& winner : game->winning) {
                    for (auto& card : game->cards) {
                        if (winner == card) {
                            num_winning++; 
                        }
                    }
                }

                for (uint32_t offset = 1; offset <= num_winning; offset++) {
                    if (game + offset == games_.end()) {
                        break;
                    }
                    auto other_game = game + offset;
                    other_game->cur_num_of++;
                }
            }
            retval += game->cur_num_of;
        }
        return retval;
    }

    std::vector<Game> games_{};
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

