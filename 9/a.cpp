#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "order.h"
#include "numbers.h"
#include "parse.h"

int main() {
    std::string input = Trim(GetContents("input.txt"));
    int n = (input.length() + 1) / 2;

    std::vector<int> disk;
    std::vector<int> free_cells;

    int id = 0;
    for (int i = 0; i < input.length(); i++) {
        int cur = (i % 2 == 0) ? (id++) : -1;
        int len = input[i] - '0';
        for (int j = 0; j < len; j++) {
            disk.push_back(cur);
            if (cur == -1) {
                free_cells.push_back(disk.size() - 1);
            }
        }
    }
    assert (id == n);

    auto it = free_cells.begin();
    for (int i = disk.size() - 1; i >= 0 && it != free_cells.end(); i--) {
        if (disk[i] == -1) {
            continue;
        }
        int free_i = *it++;
        if (free_i >= i) {
            break;
        }
        std::swap(disk[free_i], disk[i]);
    }

    long long answer = 0;
    for (int i = 0; i < disk.size(); i++) {
        if (disk[i] == -1) {
            continue;
        }
        answer += i * disk[i];
    }
    std::cout << answer << std::endl;

    return 0;
}