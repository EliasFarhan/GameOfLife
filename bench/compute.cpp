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

static void BM_ComputeBranchless(benchmark::State& state) {
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
BENCHMARK(BM_ComputeBranchless);
// Run the benchmark
BENCHMARK_MAIN();