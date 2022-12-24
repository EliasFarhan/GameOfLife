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
    for (int n = 0; n < N; ++n)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                const auto tmpPos = pos + Point(n, 0);
                if (dx == 0 && dy == 0) [[unlikely]]
                    continue;
                auto delta = Point(dx, dy);
                if (tmpPos.x + dx < 0) [[unlikely]]
                {
                    delta.x += width;
                }
                    if (tmpPos.y + dy < 0) [[unlikely]]
                    {
                        delta.y += height;
                    }
                        if (tmpPos.x + dx >= width) [[unlikely]]
                        {
                            delta.x -= width;
                        }
                            if (tmpPos.y + dy >= height) [[unlikely]]
                            {
                                delta.y -= height;
                            }

                        const auto neighbor = tmpPos + delta;
                        if (previousMap[static_cast<std::size_t>(neighbor)])
                        {
                            ++counts[n];
                        }
            }
        }
    }
    std::array<unsigned char, N> nextCells;
    for(int n = 0; n < N; ++n)
    {
        nextCells[n] = (!cells[n] && (counts[n] == 3)) || (cells[n] && (counts[n] >= 2) && (counts[n] <= 3));
    }
    return nextCells;
}