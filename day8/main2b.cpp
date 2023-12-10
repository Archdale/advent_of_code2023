#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <execution>
#include <algorithm>
#include <cctype>
#include <vector>
#include <barrier>
#include <thread>
#include <fmt/core.h>
#include <fmt/color.h>
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

    struct GameNode {
        std::string name{};

        bool is_done() {
            return (name.back() == 'Z');
        }
        std::string to_string() {
            return (is_done() ? fmt::format(fg(fmt::color::green),"{}", name) : fmt::format("{}", name));
        }
    };

    Network(const std::string& filepath) {
        std::ifstream input(filepath, std::ios::binary);
        input_.swap(input);
        if (!input_.is_open()) {
            fmt::print("File permissions?\n");
        }
    }

    uint64_t process() {
        uint64_t retval{};

        // Process game order
        std::string direction{};
        std::getline(input_, direction);
        for (auto& lr : direction) {
            if (lr == 'L') {
                game_order_.push_back(LEFT);
            } else if (lr == 'R') {
                game_order_.push_back(RIGHT);
            }
        }

        // Clear empty line
        std::getline(input_, direction);
 
        // Process Nodes
        for (std::string node; std::getline(input_, node);) {
            std::string_view node_v(node.begin(), node.end());
     
            std::string value{};
            RE2::FindAndConsume(&node_v, entry_regex_, &value);
            if (value.back() == 'A') {
                //fmt::print("{}\n", value);
                nodes_.push_back({value});
            }
            auto node_pair = node_map_.try_emplace(value, Node{});
            RE2::FindAndConsume(&node_v, entry_regex_, &value);
            (node_pair.first->second).left = value;

            RE2::FindAndConsume(&node_v, entry_regex_, &value);
            (node_pair.first->second).right = value;
        }

        fmt::print("Num of starts: {}\n", nodes_.size());
        fmt::print("Game Order Size: {}\n", game_order_.size());

        // Process Network
        std::barrier sync(std::ssize(nodes_) + 1);
        std::vector <std::jthread> threads;
        //threads.reserve(std::size(nodes_));

        uint64_t dir_index{0};
        bool done{false};
        std::atomic_uint16_t dones{};
        auto worker = [&](GameNode* work) {  
                        while (!done) {
                            sync.arrive_and_wait();

                            if (work->is_done()) {
                                dones++;
                            }
                            bool retval = work->is_done();
                            auto node_iter = node_map_.find(work->name);
                            //fmt::print("{} = {}\n", work->name, node_iter->second.to_string());

                            if (game_order_[dir_index] == LEFT) {
                                work->name = node_iter->second.left;
                            } else {
                                work->name = node_iter->second.right;
                            }
                            
                            sync.arrive_and_wait();
                        }
                    };

        threads.reserve(std::size(nodes_));
        //for (auto node : nodes_) {
        for (auto i = 0; i < nodes_.size(); i++) {
            threads.emplace_back(worker, &(nodes_[i]));
        }

        sync.arrive_and_wait();
        while (!done) {
            //fmt::print("{0:>12} {1}-----{1}\r", retval, (game_order_[dir_index] == LEFT ? "<<" : ">>" ));

            // Next rule incr
            sync.arrive_and_wait();
            dir_index = (dir_index + 1) % (game_order_.size());

            // Once for all starts
            if (dones == nodes_.size()) {
                done = true;
            }

            if (!done) {
                dones = 0;
                retval++;
            } else {
                for (auto& thread : threads) {
                    thread.detach();
                }
                break;
            }

            sync.arrive_and_wait();
        }

        return retval;
    }

  private:
    std::ifstream input_{};
    std::vector<Direction> game_order_{};
    std::vector<GameNode> nodes_{};
    RE2 entry_regex_{"([[:alnum:]]{3})"};
    //RE2 first_node_regex_{"([[:alpha:]]{2}A)"};
    //RE2 last_node_regex_{"([[:alpha:]]{2}Z)"};
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

    fmt::print("\nFinal Value: {}\n", final_val);
    
    return 0;
}


