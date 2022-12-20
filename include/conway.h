#pragma once

#include <vector>

#include "constant.h"


class Point
{
public:
    constexpr Point() = default;
    constexpr Point(int x, int y) : x(x), y(y) {}
    constexpr explicit Point(std::size_t index) : x(static_cast<int>(index% width)), y(static_cast<int>(index / width))
    {

    }

    explicit constexpr operator std::size_t() const
    {
        return y * width + x;
    }

    constexpr Point operator+(const Point& p) const
    {
        return { p.x + x, p.y + y };
    }

    int x = 0;
    int y = 0;
};

unsigned char Compute(const std::vector<unsigned char>& previousMap, const Point& pos) noexcept;
unsigned char Compute2(const std::vector<unsigned char>& previousMap, const Point& pos) noexcept;