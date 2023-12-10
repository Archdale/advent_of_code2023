#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <cctype>
#include <vector>
#include <fmt/core.h>
#include <re2/re2.h>

class Network {
  public:
    enum Direction : bool {
        LEFT  = false,
        RIGHT = true,
    };

    struct Node {
        std::string left{};
        std::string right{};

        std::string to_string() {
            return fmt::format("< {} | {} >", left, right);
        }
    };

    Network(const std::string& filepath) {
        std::ifstream input(filepath, std::ios::binary);
        input_.swap(input);
        if (!input_.is_open()) {
            fmt::print("File permissions?\n");
        }
    }

    uint32_t process() {
        uint32_t retval{};

        // Process game order
        std::string direction{};
        std::getline(input_, direction);
        for (auto& lr : direction) {
            if (lr == 'L') {
                game_order_.push_back(LEFT);
            } else {
                game_order_.push_back(RIGHT);
            }
        }

        // Clear empty line
        std::getline(input_, direction);
 
        // Process Nodes
        for (std::string node; std::getline(input_, node);) {
            std::string_view node_v(node.begin(), node.end());
     
            std::string value{};
            RE2::FindAndConsume(&node_v, "([[:alpha:]]{3})", &value);
            auto node_pair = node_map_.try_emplace(value, Node{});
            RE2::FindAndConsume(&node_v, "([[:alpha:]]{3})", &value);
            (node_pair.first->second).left = value;
            RE2::FindAndConsume(&node_v, "([[:alpha:]]{3})", &value);
            (node_pair.first->second).right = value;
        }

        // Process Network
        std::string curr_node{first_node_};
        uint32_t dir_index{0};
        while (curr_node != last_node_) {
            auto node_iter = node_map_.find(curr_node);
            fmt::print("{} = {}\n", node_iter->first, node_iter->second.to_string());
            if (game_order_[dir_index] == LEFT) {
                fmt::print("left!\n");
                curr_node = node_iter->second.left;
            } else {
                fmt::print("right!\n");
                curr_node = node_iter->second.right;
            }

            dir_index = (dir_index + 1) % (game_order_.size() - 1);
            retval++;
        }

        return retval;
    }

  private:
    std::ifstream input_{};
    std::vector<Direction> game_order_{};
    std::string first_node_{"AAA"};
    std::string last_node_{"ZZZ"};
    std::unordered_map<std::string, Node> node_map_{};
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

    Network net(file);

    auto final_val = net.process();

    fmt::print("Final Value: {}\n", final_val);
    
    return 0;
}
