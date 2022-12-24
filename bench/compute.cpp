#include "constant.h"
#include "conway.h"

#include <benchmark/benchmark.h>

static void BM_ComputeBranch(benchmark::State& state) {
    std::vector<unsigned char> previousMap(size, 0);
    {
        std::srand(std::time(nullptr));
        for (auto& cell : previousMap)
        {
            if (std::rand() % 2 == 0)
            {
                cell = 1;
            }
        }
    }
    // Perform setup here
    for (auto _ : state) {
        // This code gets timed
        for (std::size_t i = 0; i < size; ++i)
        {
            const auto v = Compute(previousMap,Point { i });
            benchmark::DoNotOptimize(v);
        }
    }
}
// Register the function as a benchmark
BENCHMARK(BM_ComputeBranch);

static void BM_Compute2Branchless(benchmark::State& state) {
    std::vector<unsigned char> previousMap(size, 0);
    {
        std::srand(std::time(nullptr));
        for (auto& cell : previousMap)
        {
            if (std::rand() % 2 == 0)
            {
                cell = 1;
            }
        }
    }
    // Perform setup here
    for (auto _ : state) {
        // This code gets timed
        for (std::size_t i = 0; i < size; ++i)
        {
            const auto v = Compute2(previousMap, Point{ i });
            benchmark::DoNotOptimize(v);
        }
    }
}
// Register the function as a benchmark
BENCHMARK(BM_Compute2Branchless);

static void BM_Compute4Branchless(benchmark::State& state) {
    std::vector<unsigned char> previousMap(size, 0);
    {
        std::srand(std::time(nullptr));
        for (auto& cell : previousMap)
        {
            if (std::rand() % 2 == 0)
            {
                cell = 1;
            }
        }
    }
    // Perform setup here
    for (auto _ : state) {
        // This code gets timed
        for (std::size_t i = 0; i < size; i+=4)
        {
            const auto v = ComputeN<4>(previousMap, Point{ i });
            benchmark::DoNotOptimize(v);
        }
    }
}
// Register the function as a benchmark
BENCHMARK(BM_Compute4Branchless);

static void BM_Compute8Branchless(benchmark::State& state) {
    std::vector<unsigned char> previousMap(size, 0);
    {
        std::srand(std::time(nullptr));
        for (auto& cell : previousMap)
        {
            if (std::rand() % 2 == 0)
            {
                cell = 1;
            }
        }
    }
    // Perform setup here
    for (auto _ : state) {
        // This code gets timed
        for (std::size_t i = 0; i < size; i += 8)
        {
            const auto v = ComputeN<8>(previousMap, Point{ i });
            benchmark::DoNotOptimize(v);
        }
    }
}
// Register the function as a benchmark
BENCHMARK(BM_Compute8Branchless);


static void BM_Compute16Branchless(benchmark::State& state) {
    std::vector<unsigned char> previousMap(size, 0);
    {
        std::srand(std::time(nullptr));
        for (auto& cell : previousMap)
        {
            if (std::rand() % 2 == 0)
            {
                cell = 1;
            }
        }
    }
    // Perform setup here
    for (auto _ : state) {
        // This code gets timed
        for (std::size_t i = 0; i < size; i += 16)
        {
            const auto v = ComputeN<16>(previousMap, Point{ i });
            benchmark::DoNotOptimize(v);
        }
    }
}
// Register the function as a benchmark
BENCHMARK(BM_Compute16Branchless);

static void BM_Compute24Branchless(benchmark::State& state) {
    std::vector<unsigned char> previousMap(size, 0);
    {
        std::srand(std::time(nullptr));
        for (auto& cell : previousMap)
        {
            if (std::rand() % 2 == 0)
            {
                cell = 1;
            }
        }
    }
    // Perform setup here
    for (auto _ : state) {
        // This code gets timed
        for (std::size_t i = 0; i < size; i += 24)
        {
            const auto v = ComputeN<24>(previousMap, Point{ i });
            benchmark::DoNotOptimize(v);
        }
    }
}
// Register the function as a benchmark
BENCHMARK(BM_Compute24Branchless);
// Run the benchmark
BENCHMARK_MAIN();