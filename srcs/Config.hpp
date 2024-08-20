#pragma once

#include <string>
#include <iostream>
#include <fstream>

class Config
{
public:
    Config();
    Config(const std::string& config);
    ~Config();
    bool valid() const;

private:
    bool _valid;
};