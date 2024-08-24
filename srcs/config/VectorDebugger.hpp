#pragma once

#include <vector>
#include <iostream>

template <typename T>
class VectorDebugger
{
public:
    virtual ~VectorDebugger() = 0;
    static void print(const std::vector<T> &v);
};

#include "VectorDebugger.tpp"