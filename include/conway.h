#pragma once

#include <vector>
#include <array>

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

template<int N>
std::array<unsigned char, N> ComputeN(const std::vector<unsigned char>& previousMap, const Point& pos) noexcept
{
    std::array<unsigned, N> cells;
    for (int n = 0; n < N; ++n)
    {
        cells[n] = previousMap[static_cast<std::size_t>(pos) + n];
    }
    std::array<int, N> counts{};

    constexpr std::array<Point, 8> deltas
    {
        {
            {-1,-1},
            {-1,0},
            {0,-1},
            {0,1},
            {1,0},
            {1,-1},
            {-1,1},
            {1,1},
        }
    };

    std::array<unsigned char, N> nextCells;
    for (int n = 0; n < N; ++n)
    {
        for (const auto& delta : deltas)
        {
            const auto tmpPos = pos + Point(n, 0);
            const auto neighbor = Point((tmpPos.x + delta.x + width) % width, (tmpPos.y + delta.y + height) % height);
            counts[n] += previousMap[static_cast<std::size_t>(neighbor)];
        }
        nextCells[n] = (!cells[n] && (counts[n] == 3)) || (cells[n] && (counts[n] >= 2) && (counts[n] <= 3));
    }
    return nextCells;
}