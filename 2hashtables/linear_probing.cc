#include <iostream>
#include <string>
#include <vector>
#include <functional>

enum class State {
    EMPTY,
    OCCUPIED,
    DELETED
};

struct Entry {
    std::string key;
    int value = 0;
    State state = State::EMPTY;
};

using Table = std::vector<Entry>;

std::size_t hash_index(const std::string& key, std::size_t table_size) {
    return std::hash<std::string>{}(key) & (table_size - 1);
}

int find_slot(Table& table, const std::string& key) {
    std::size_t start = hash_index(key, table.size());

    for (std::size_t step = 0; step < table.size(); step++) {
        std::size_t index = (start + step) & (table.size() - 1);

        if (table[index].state == State::EMPTY) {
            return -1;
        }

        if (table[index].state == State::OCCUPIED &&
            table[index].key == key) {
            return static_cast<int>(index);
        }
    }

    return -1;
}

int find_insert_slot(Table& table, const std::string& key) {
    std::size_t start = hash_index(key, table.size());
    int first_deleted = -1;

    for (std::size_t step = 0; step < table.size(); step++) {
        std::size_t index = (start + step) % table.size();

        if (table[index].state == State::OCCUPIED &&
            table[index].key == key) {
            return static_cast<int>(index);
        }

        if (table[index].state == State::DELETED && first_deleted == -1) {
            first_deleted = static_cast<int>(index);
        }

        if (table[index].state == State::EMPTY) {
            if (first_deleted != -1) {
                return first_deleted;
            }
            return static_cast<int>(index);
        }
    }

    return first_deleted;
}

void resize_table(Table& table, std::size_t new_size) {
    Table new_table(new_size);

    for (const Entry& entry : table) {
        if (entry.state == State::OCCUPIED) {
            int index = find_insert_slot(new_table, entry.key);
            new_table[index] = entry;
        }
    }

    table = new_table;
}

int main() {
    Table table(16);

    std::size_t num_pairs = 0;

    double max_alpha = 0.5;
    double min_alpha = 0.125;

    std::string line;
    int i = 0;

    while (std::getline(std::cin, line)) {
        std::string word = line;

        bool remove_it = i % 16 == 0;
        int index = find_slot(table, word);

        bool is_present = index != -1;

        if (is_present) {
            if (remove_it) {
                table[index].state = State::DELETED;
                num_pairs--;

                double alpha = static_cast<double>(num_pairs) / table.size();

                if (table.size() > 16 && alpha < min_alpha) {
                    resize_table(table, table.size() / 2);
                }
            } else {
                table[index].value++;
            }
        } else if (!remove_it) {
            double alpha_after_insert =
                static_cast<double>(num_pairs + 1) / table.size();

            if (alpha_after_insert > max_alpha) {
                resize_table(table, table.size() * 2);
            }

            int insert_index = find_insert_slot(table, word);

            table[insert_index].key = word;
            table[insert_index].value = 1;
            table[insert_index].state = State::OCCUPIED;

            num_pairs++;
        }

        i++;
    }

    int count = -1;
    std::string best_word = "";

    for (const Entry& entry : table) {
        if (entry.state == State::OCCUPIED) {
            const std::string& k = entry.key;
            int v = entry.value;

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