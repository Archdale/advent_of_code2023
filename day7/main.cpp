#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <cctype>
#include <vector>
#include <fmt/core.h>
#include <re2/re2.h>

class CamelCards {
    enum CardType : uint8_t {
        TWO = 2,
        THREE = 3,
        FOUR = 4,
        FIVE = 5,
        SIX = 6,
        SEVEN = 7,
        EIGHT = 8,
        NINE = 9,
        TEN = 10,
        JACK = 11,
        QUEEN = 12,
        KING = 13,
        ACE = 14,
    };

    static CardType str_to_card(char c) {
        switch (c) {
            case '2': return TWO;
            case '3': return THREE;
            case '4': return FOUR;
            case '5': return FIVE;
            case '6': return SIX;
            case '7': return SEVEN;
            case '8': return EIGHT;
            case '9': return NINE;
            case 'T': return TEN;
            case 'J': return JACK;
            case 'Q': return QUEEN;
            case 'K': return KING;
            case 'A': return ACE;
        }
    };

    static std::string card_to_str(CardType c) {
        switch (c) {
            case TWO: return "2";
            case THREE: return "3";
            case FOUR: return "4";
            case FIVE: return "5";
            case SIX: return "6";
            case SEVEN: return "7";
            case EIGHT: return "8";
            case NINE: return "9";
            case TEN: return "T";
            case JACK: return "J";
            case QUEEN: return "Q";
            case KING: return "K";
            case ACE: return "A";
        }
    };

    enum HandType : uint8_t {
        HIGH_CARD = 1,
        ONE_PAIR = 2,
        TWO_PAIR = 3,
        THREE_OF_A_KIND = 4,
        FULL_HOUSE = 5,
        FOUR_OF_A_KIND = 6,
        FIVE_OF_A_KIND = 7,
    };

    static std::string hand_to_str(HandType t) {
        switch (t) {
            case HIGH_CARD: return "High Card";
            case ONE_PAIR: return "One Pair";
            case TWO_PAIR: return "Two Pair";
            case THREE_OF_A_KIND: return "Three-of-a-kind";
            case FULL_HOUSE: return "Full House";
            case FOUR_OF_A_KIND: return "Four-of-a-kind";
            case FIVE_OF_A_KIND: return "Five-of-a-kind";
        }
    };

    struct Hand {
        std::vector<CardType> cards{};
        HandType hand_type{HIGH_CARD};
        uint32_t worth{};

        std::strong_ordering operator<=>(const Hand& rhs) {
            if (hand_type == rhs.hand_type) {
                for(auto i = 0; i < cards.size(); i++) {
                    if (cards[i] == rhs.cards[i]) {
                        continue;
                    }
                    return (cards[i] <=> rhs.cards[i]);
                }
                return std::strong_ordering::equal;
            }
            return (hand_type <=> rhs.hand_type);
        }

        std::string to_string() {
            std::string retval;
            for (auto& card : cards) {
                auto card_str = card_to_str(card);
                retval += fmt::format("{} ", card_str);
            }
            auto hand_str = hand_to_str(hand_type);
            retval += fmt::format("- Worth: {:>5} - Type: {}", worth, hand_str);
            
            return retval;
        }
    };

  public:
    CamelCards(const std::string& filepath) {
        std::ifstream input(filepath, std::ios::binary);
        input_.swap(input);
        if (!input_.is_open()) {
            fmt::print("File permissions?\n");
        }
    }
    uint32_t process() {
        uint32_t retval{};

        for (std::string game_line; std::getline(input_, game_line);) {
            std::string_view game_v(game_line.begin(), game_line.end());
            Hand hand{};
            
            std::string hand_str{};
            std::unordered_map<CardType, uint32_t> counts{};
            RE2::FindAndConsume(&game_v, "([[:alnum:]]{5})", &hand_str);
            for (auto& card_str : hand_str) {
                auto card = str_to_card(card_str);
                hand.cards.push_back(card);
                auto count = counts.try_emplace(card, 0);
                count.first->second++;
            }

            uint32_t highest_cnt{};
            uint32_t second_highest_cnt{};
            for (auto& count : counts) {
                //fmt::print("Count: {}\n", count.second);
                if (count.second >= highest_cnt) {
                    second_highest_cnt = highest_cnt;
                    highest_cnt = count.second;
                } else if (count.second >= second_highest_cnt) {
                    second_highest_cnt = count.second;
                }
            }
            //fmt::print("1st: {}, 2nd: {}\n", highest_cnt, second_highest_cnt);

            if (highest_cnt == 5 && second_highest_cnt == 0) {
                hand.hand_type = FIVE_OF_A_KIND;
            } else if (highest_cnt == 4 && second_highest_cnt == 1) {
                hand.hand_type = FOUR_OF_A_KIND;
            } else if (highest_cnt == 3 && second_highest_cnt == 2) {
                hand.hand_type = FULL_HOUSE;
            } else if (highest_cnt == 3 && second_highest_cnt == 1) {
                hand.hand_type = THREE_OF_A_KIND;
            } else if (highest_cnt == 2 && second_highest_cnt == 2) {
                hand.hand_type = TWO_PAIR;
            } else if (highest_cnt == 2 && second_highest_cnt == 1) {
                hand.hand_type = ONE_PAIR;
            }

            uint32_t worth;
            RE2::FindAndConsume(&game_v, "([[:digit:]]+)", &worth);
            hand.worth = worth;

            //fmt::print("{}\n",hand.to_string());
            hands_.push_back(hand);
        }

        // Sort
        std::sort(hands_.begin(), hands_.end());

        // Process
        uint32_t rank{1};
        for (auto& hand : hands_) {
            retval += (hand.worth * rank);
            rank++;
        }

        return retval;
    }

  private:
    std::ifstream input_{};
    std::vector<Hand> hands_{};
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

    CamelCards cards(file);

    auto final_val = cards.process();

    fmt::print("Final Value: {}\n", final_val);
    
    return 0;
}

