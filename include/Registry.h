//
// Created by Luca Teodorescu on 17.05.2026.
//

#ifndef PROJECT_OOP_REGISTRY_H
#define PROJECT_OOP_REGISTRY_H

#include <vector>
#include <iostream>
#include <algorithm>

template <typename T>
class Registry {
private:
    std::vector<T> records;

public:
    void AddRecord(const T& item) {
        records.push_back(item);
    }
    void PrintAll() const {
        for (const auto& item : records) {
            std::cout << " -> " << item << '\n';
        }
    }
    template <typename CompareFunction>
    void SortRecords(CompareFunction comp) {
        std::sort(records.begin(), records.end(), comp);
    }
    template <typename Predicate>
    int CountMatches(Predicate pred) const {
        return std::count_if(records.begin(), records.end(), pred);
    }
};

#endif //PROJECT_OOP_REGISTRY_H