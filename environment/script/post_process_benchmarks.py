#!/usr/bin/env python
# -*- encoding: utf8 -*-
import lue
import matplotlib
# matplotlib.use("PDF")
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
import docopt
import os.path
import sys


usage = """\
Post-process benchmark results

Usage:
    {command} <lue_file>

Options:
    lue_file        Pathname to file containing benchmark results
    -h --help       Show this screen
""".format(
    command = os.path.basename(sys.argv[0]))


def post_process_strong_scaling_benchmarks(
        name,
        time_point,
        system_name,
        environment,
        durations):

    # TODO
    #     - Add units to y-axis duration plot
    #     - Add subtitle
    #         time point of benchmark
    #     - Add work-span stuff

    data = pd.merge(environment, durations, left_index=True, right_index=True)

    # Group measurements by benchmark and calculate the mean
    durations_by_nr_threads = \
        data[["nr_threads", "duration"]].groupby("nr_threads").mean()
    t1 = durations_by_nr_threads.at[1, "duration"]

    # Best case: duration scales perfect with the number of threads
    # 100% parallel code, but without parallelization overhead
    durations_by_nr_threads["linear_duration"] = \
        t1 / durations_by_nr_threads.index

    # Worst case: duration does not scale with the number of threads
    # 100% serial code, but without parallelization overhead
    durations_by_nr_threads["serial_duration"] = t1

    # speedup = t1 / tn
    durations_by_nr_threads["relative_speedup"] = \
        t1 / durations_by_nr_threads["duration"]
    durations_by_nr_threads["linear_relative_speedup"] = \
        t1 / durations_by_nr_threads["linear_duration"]
    durations_by_nr_threads["serial_relative_speedup"] = \
        t1 / durations_by_nr_threads["serial_duration"]

    # efficiency = 100% * speedup / nr_workers
    durations_by_nr_threads["efficiency"] = \
        100 * durations_by_nr_threads["relative_speedup"] / \
        durations_by_nr_threads.index
    durations_by_nr_threads["linear_efficiency"] = \
        100 * durations_by_nr_threads["linear_relative_speedup"] / \
        durations_by_nr_threads.index
    durations_by_nr_threads["serial_efficiency"] = \
        100 * durations_by_nr_threads["serial_relative_speedup"] / \
        durations_by_nr_threads.index

    max_nr_threads = environment["nr_threads"].max()

    figure, axes = plt.subplots(
            nrows=1, ncols=3,
            figsize=(15, 5)
            # sharex=False
        )  # Inches...

    # grid = sns.relplot(x="nr_threads", y="duration", kind="line", data=data,
    #     legend="full", ci="sd", ax=axes[0, 0])

    # https://xkcd.com/color/rgb/
    serial_color = sns.xkcd_rgb["pale red"]
    linear_color = sns.xkcd_rgb["medium green"]
    actual_color = sns.xkcd_rgb["denim blue"]

    # duration by nr_threads
    sns.lineplot(
        data=durations_by_nr_threads["linear_duration"],
        ax=axes[0], color=linear_color)
    sns.lineplot(
        data=durations_by_nr_threads["serial_duration"],
        ax=axes[0], color=serial_color)
    sns.lineplot(
        x="nr_threads", y="duration", data=data,
        ax=axes[0], color=actual_color)
    axes[0].set_ylabel(u"duration ± 1 std ({})".format("todo"))
    axes[0].set_xlabel("number of threads")

    # speedup by nr_threads
    sns.lineplot(
        data=durations_by_nr_threads["linear_relative_speedup"],
        ax=axes[1], color=linear_color)
    sns.lineplot(
        data=durations_by_nr_threads["serial_relative_speedup"],
        ax=axes[1], color=serial_color)
    sns.lineplot(
        data=durations_by_nr_threads["relative_speedup"],
        ax=axes[1], color=actual_color)
    axes[1].set_ylim(0, max_nr_threads + 1)
    axes[1].set_ylabel("relative speedup (-)")
    axes[1].set_xlabel("number of threads")

    # efficiency by nr_threads
    sns.lineplot(
        data=durations_by_nr_threads["linear_efficiency"],
        ax=axes[2], color=linear_color)
    sns.lineplot(
        data=durations_by_nr_threads["serial_efficiency"],
        ax=axes[2], color=serial_color)
    sns.lineplot(
        data=durations_by_nr_threads["efficiency"],
        ax=axes[2], color=actual_color)
    axes[2].set_ylim(0, 110)
    axes[2].set_ylabel("efficiency (%)")
    axes[2].set_xlabel("number of threads")

    figure.legend(labels=["linear", "serial", "actual"])

    # plt.setp(axes, xlabel="meh")
    figure.suptitle(
        "{}\nStrong scaling experiment performed at {}, on {}".format(
            name, time_point, system_name))

    plt.savefig("benchmark.pdf")


def post_process_weak_scaling_benchmarks(
        name,
        time_point,
        system_name,
        environment,
        durations):

    # TODO
    pass


def post_process_benchmarks(
        lue_pathname):

    lue_dataset = lue.open_dataset(lue_pathname)
    lue_benchmark = lue_dataset.phenomena["benchmark"]

    lue_meta_information = \
        lue_benchmark.collection_property_sets["meta_information"]
    lue_name = lue_meta_information.properties["name"]
    lue_system_name = lue_meta_information.properties["system_name"]

    benchmark_name = lue_name.value[:]
    assert(len(benchmark_name) == 1)
    benchmark_name = benchmark_name[0]

    time_point = "todo"

    system_name = lue_system_name.value[:]
    assert(len(system_name) == 1)
    system_name = system_name[0]

    lue_measurement = lue_benchmark.property_sets["measurement"]
    lue_nr_localities = lue_measurement.properties["nr_localities"]
    lue_nr_threads = lue_measurement.properties["nr_threads"]
    lue_work_size = lue_measurement.properties["work_size"]
    lue_duration = lue_measurement.properties["duration"]

    nr_localities = lue_nr_localities.value[:]
    nr_measurements = len(nr_localities)
    nr_threads = lue_nr_threads.value[:]
    assert(len(nr_threads) == nr_measurements)
    work_size = lue_work_size.value[:]
    assert(len(work_size) == nr_measurements)

    duration = lue_duration.value[:]
    assert(len(duration) == nr_measurements)
    nr_durations = len(duration[0])

    # Set up data frames
    # The (default) index is the index of the benchmark
    environment = pd.DataFrame({
            "nr_localities": nr_localities,
            "nr_threads": nr_threads,
            "work_size": work_size,
        })

    # Per benchmark a series. Each series contains all duration measurements.
    # These series are concatenated in one long series containing the
    # durations for all benchmarks. The index contains the index of
    # the benchmark.
    durations = [
            pd.Series(duration[b], index=nr_durations*[b]) for b in
                range(nr_measurements)
        ]
    durations = pd.DataFrame({
            "duration": pd.concat(durations)
        })


    nr_equal_work_sizes = \
        (environment["work_size"] == environment["work_size"][0]).sum()
    constant_work_size = nr_equal_work_sizes == nr_measurements

    if constant_work_size:
        post_process_strong_scaling_benchmarks(
            benchmark_name, time_point, system_name, environment, durations)
    else:
        post_process_weak_scaling_benchmarks(
            benchmark_name, time_point, system_name, environment, durations)


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    lue_pathname = arguments["<lue_file>"]

    post_process_benchmarks(lue_pathname)
