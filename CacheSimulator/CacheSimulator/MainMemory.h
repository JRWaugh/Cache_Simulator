#pragma once

#include <optional>
using uint32c_t = unsigned int const;
constexpr char load = 'l', store = 's';

class MainMemory {
protected:
    unsigned int mAccessTime;

public:
    MainMemory(uint32c_t accessTime = 400) : mAccessTime{ accessTime } {};
    virtual std::optional<uint32c_t> accessAddress(uint32c_t address, const unsigned char instruction) {
        return mAccessTime;
    }
    virtual const double getAMAT() const {
        return mAccessTime;
    }
};
