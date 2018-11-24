#pragma once

#include <stdexcept>

class NotYetImplementedException : public std::runtime_error
{
public:
    NotYetImplementedException(const std::string &message) : std::runtime_error(message) {}
};