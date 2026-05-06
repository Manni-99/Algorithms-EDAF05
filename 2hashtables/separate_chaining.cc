#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <functional>

using Table = std::vector<std::list<std::pair<std::string, int>>>;

void resize_table(Table& table, std::size_t new_size) {
    Table new_table(new_size);

    for (const auto& chain : table) {
        for (const auto& pair : chain) {
            std::size_t new_index =
                std::hash<std::string>{}(pair.first) % new_size;

            new_table[new_index].push_back(pair);
        }
    }

    table = new_table;
}

int main() {
    Table table(1);
    std::size_t num_pairs = 0;

    std::string line;
    int i = 0;

    while (std::getline(std::cin, line)) {
        std::string word = line;
        std::size_t index = std::hash<std::string>{}(word) & (table.size() - 1);
        //std::size_t index = std::hash<std::string>{}(word) % table.size();

        auto& chain = table[index];

        auto it = chain.begin();
        while (it != chain.end() && it->first != word) {
            ++it;
        }

        bool is_present = it != chain.end();
        bool remove_it = i % 16 == 0;

        if (is_present) {
            if (remove_it) {
                chain.erase(it);
                num_pairs--;

                if (table.size() > 1 && num_pairs < table.size() / 4) {
                    resize_table(table, table.size() / 2);
                }
            } else {
                it->second++;
            }
        } else if (!remove_it) {
            chain.push_back({word, 1});
            num_pairs++;

            if (num_pairs > table.size()) {
                resize_table(table, table.size() * 2);
            }
        }

        i++;
    }

    int count = -1;
    std::string best_word = "";

    for (const auto& chain : table) {
        for (const auto& pair : chain) {
            const std::string& k = pair.first;
            int v = pair.second;

            if (v > count || (v == count && k < best_word)) {
                count = v;
                best_word = k;
            }
        }
    }

    if (count != -1) {
        std::cout << best_word << " " << count << std::endl;
    }

    return 0;
}