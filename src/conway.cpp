#include "conway.h"
#include "constant.h"


unsigned char Compute(const std::vector<unsigned char>& previousMap, const Point& pos) noexcept
{
    unsigned char nextCell;
    const auto cell = previousMap[static_cast<std::size_t>(pos)];
    int count = 0;
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            if (dx == 0 && dy == 0)
                continue;
            auto delta = Point(dx, dy);
            if (pos.x + dx < 0)
            {
                delta.x += width;
            }
            if (pos.y + dy < 0)
            {
                delta.y += height;
            }
            if (pos.x + dx >= width)
            {
                delta.x -= width;
            }
            if (pos.y + dy >= height)
            {
                delta.y -= height;
            }

            const auto neighbor = pos + delta;
            if (previousMap[static_cast<std::size_t>(neighbor)])
            {
                ++count;
            }
        }
    }
    //nextCell = (!cell & (count == 3)) | (cell & (count >= 2) & (count <= 3));

    if (!cell)
    {
        if (count == 3)
        {
            nextCell = 1;
        }
        else
        {
            nextCell = 0;
        }
    }
    else
    {
        if (count < 2 || count > 3)
        {
            nextCell = 0;
        }
        else
        {
            nextCell = 1;
        }
    }

    return nextCell;
}

unsigned char Compute2(const std::vector<unsigned char>& previousMap, const Point& pos) noexcept
{
    const auto cell = previousMap[static_cast<std::size_t>(pos)];
    int count = 0;
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
    for(const auto& delta: deltas)
    {
#define MODULO
#ifndef MODULO
        Point d = delta;
        //const auto delta = Point((dx+width)%width, (dy+height)%height);
        if (pos.x + delta.x < 0) [[unlikely]]
        {
            d.x += width;
        }
        if (pos.y + delta.y < 0) [[unlikely]]
        {
            d.y += height;
        }
        if (pos.x + delta.x >= width) [[unlikely]]
        {
            d.x -= width;
        }
        if (pos.y + delta.y >= height) [[unlikely]]
        {
            d.y -= height;
        }
        const auto neighbor = pos + d;
#else
        const auto neighbor = Point((pos.x + delta.x + width) % width, (pos.y + delta.y + height) % height);
#endif
        if (previousMap[static_cast<std::size_t>(neighbor)])
        {
            ++count;
        }
        
    }
    const unsigned char nextCell = (!cell & (count == 3)) | (cell & (count >= 2) & (count <= 3));
    return nextCell;
}

