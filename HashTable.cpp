#include "HashTable.h"
#include <iostream>
#include <functional>
#include <cstdint>
#include <stdexcept>
HashTable::HashTable(size_t size) noexcept : _capacity(static_cast<int32_t>(size)), _filled(0), table(size) {}

HashTable::~HashTable() = default;

size_t HashTable::hash_function(const KeyType &key) const {
    std::hash<KeyType> hashed;
    return hashed(key) % _capacity;
}

double HashTable::getLoadFactor(){
    return static_cast<double> (_filled)/_capacity;
}

void HashTable::insert(const KeyType &key, const ValueType &value){
    size_t index = hash_function(key);
    auto &bucket = table[index];
    auto it = bucket.begin();
    while (it != bucket.end() && it->first != key) {
        ++it;
    }
    if (it != bucket.end()) {
        it->second = value;
        return;
    }

    bucket.emplace_back(key, value);
    ++_filled;

    if (getLoadFactor() > 0.75) {
        size_t new_capacity = _capacity * 2;
        std::vector<std::list<std::pair<KeyType, ValueType>>> new_table(new_capacity);

        for (const auto &bucket : table) {
            for (const auto &pair : bucket) {
                std::hash<KeyType> hasher;
                size_t new_index = hasher(pair.first) % new_capacity;
                new_table[new_index].emplace_back(pair.first, pair.second);
            }
        }

        table = std::move(new_table);
        _capacity = static_cast<int32_t>(new_capacity);
    }
}

bool HashTable::find(const KeyType &key, ValueType &value) const{
    size_t index = hash_function(key);
    const auto &bucket = table[index];
    auto it = bucket.begin();   
    while (it != bucket.end() && it->first != key) {
        ++it;
    }
    if (it == bucket.end()) {
        return false;
    }
    value = it->second;
    return true;
}

void HashTable::remove(const KeyType &key){
    size_t index = hash_function(key);
    auto &bucket = table[index];
    auto it = bucket.begin();
    while (it != bucket.end() && it->first != key) {
        ++it;
    }
    if (it != bucket.end()) {
        bucket.erase(it);
        --_filled;
    }
    return;
}

ValueType& HashTable::operator[](const KeyType &key){
    size_t index = hash_function(key);
    auto &bucket = table[index];

    auto it = bucket.begin();
    while (it != bucket.end() && it->first != key) {
        ++it;
    }

    if (it == bucket.end()) {
        throw std::out_of_range("Ключа не существует");
    }

    return it->second;
}