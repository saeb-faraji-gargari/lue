#include "polute_air.hpp"
#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"


auto setup_benchmark(
        int /* argc */,
        char* /* argv */[],
        lue::benchmark::Environment const& environment)
{
    // This function is called on each locality. Return a callable that
    // performs work on the current locality

    std::string const name = "polute_air";
    std::string const cluster_name = "eejit";
    std::string const description = "Airpolution model benchmark";

    // Function to benchmark
    auto callable = [](
        lue::benchmark::Environment const& /* environment */)
    {
        std::uint64_t const max_tree_depth =
            lue::required_configuration_entry<std::uint64_t>("max_tree_depth");
        std::uint64_t const nr_time_steps =
            lue::required_configuration_entry<std::uint64_t>("nr_time_steps");
        std::uint64_t const nr_rows =
            lue::required_configuration_entry<std::uint64_t>("nr_rows");
        std::uint64_t const nr_cols =
            lue::required_configuration_entry<std::uint64_t>("nr_cols");
        std::uint64_t const nr_rows_partition =
            lue::required_configuration_entry<std::uint64_t>(
                "nr_rows_partition");
        std::uint64_t const nr_cols_partition =
            lue::required_configuration_entry<std::uint64_t>(
                "nr_cols_partition");

        lue::polute_air(
            max_tree_depth,
            nr_time_steps, nr_rows, nr_cols,
            nr_rows_partition, nr_cols_partition);
    };

    return lue::benchmark::Benchmark{
        std::move(callable), cluster_name, environment, name, description};
}


LUE_CONFIGURE_HPX_BENCHMARK(
        "hpx.run_hpx_main!=1"
    )