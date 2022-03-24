#pragma once

#include <vector>

// This file extends std::vector by Quality of Life (qol) methods.

template <typename T>
void remove(std::vector<T>& vec, size_t pos)
{
    auto it = vec.begin();
    std::advance(it, pos);
    vec.erase(it);
}

template <typename T>
void replace(std::vector<T>& vec, size_t pos, const T& newValue)
{
    auto it = vec.begin();
    std::advance(it, pos);
    *it = newValue;
}

template <typename T>
bool removeOne(std::vector<T>& vec, const T& value)
{
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end()) {
        vec.erase(it);
        return true;
    }
    return false;
}