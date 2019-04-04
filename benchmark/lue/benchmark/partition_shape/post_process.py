# -*- encoding: utf8 -*-
from .benchmark import *
from .experiment import *
from ..cluster import *

import lue

import dateutil.relativedelta
import dateutil.parser

import pandas as pd
import matplotlib
# matplotlib.use("PDF")
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import pandas as pd
import seaborn as sns

import json
import shlex
import subprocess
import tempfile



# import math
# 
# millnames = ['',' thousand',' Million',' Billion',' Trillion']
# 
# def millify(n):
#     n = float(n)
#     millidx = max(0,min(len(millnames)-1,
#     int(math.floor(0 if n == 0 else math.log10(abs(n))/3))))
# 
#     return '{:.0f}{}'.format(n / 10**(3 * millidx), millnames[millidx])


def format_duration(
        duration):
    # TODO Pass in units and be smarter

    return "{:,}".format(int(duration))


def format_partition_size(
        size):
    # TODO Use powers of 3, 6, 9, etc

    return "{:,}".format(int(size))


def lue_translate():
    return os.path.expandvars("$LUE_OBJECTS/bin/lue_translate")


def benchmark_meta_to_lue_json(
        benchmark_pathname,
        lue_pathname,
        cluster,
        experiment):

    # Read benchmark JSON
    benchmark_json = json.loads(open(benchmark_pathname).read())
    environment_json = benchmark_json["environment"]
    nr_localities = [environment_json["nr_localities"]]
    nr_threads = [environment_json["nr_threads"]]
    array_shape = list(experiment.array_shape)

    lue_json = {
        "dataset": {
            "phenomena": [
                {
                    "name": "benchmark",
                    "collection_property_sets": [
                        {
                            "name": "meta_information",
                            "properties": [
                                {
                                    "name": "name",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": ["{}".format(
                                        experiment.program_name)]
                                },
                                {
                                    "name": "system_name",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": ["{}".format(cluster.name)]
                                },
                                {
                                    "name": "command",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": ["{}".format(
                                        experiment.command_pathname)]
                                },
                                {
                                    "name": "kind",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": ["{}".format("partition_shape")]
                                },
                                {
                                    "name": "description",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": ["{}".format(
                                        experiment.description)]
                                },
                                {
                                    "name": "nr_localities",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "value": nr_localities
                                },
                                {
                                    "name": "nr_threads",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "value": nr_threads
                                },
                                {
                                    "name": "array_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "shape": [len(array_shape)],
                                    "value": array_shape
                                },
                            ]
                        }
                    ]
                }
            ]
        }
    }

    # Write results
    open(lue_pathname, "w").write(
        json.dumps(lue_json, sort_keys=False, indent=4))


def benchmark_to_lue_json(
        benchmark_pathname,
        lue_json_pathname,
        epoch):

    # Read benchmark JSON
    benchmark_json = json.loads(open(benchmark_pathname).read())

    time_units = "second"
    benchmark_epoch = dateutil.parser.isoparse(benchmark_json["start"])
    epoch_offset = int((benchmark_epoch - epoch).total_seconds())

    if epoch_offset < 0:
        raise RuntimeError(
            "epoch passed in is later than epoch from benchmark: "
            "{} > {}".format(epoch, benchmark_epoch))

    # Calculate number of seconds sinds the epoch
    time_points = [
        dateutil.parser.isoparse(timing["start"])
            for timing in benchmark_json["timings"]]
    time_points = [
        epoch_offset + int((time_point - benchmark_epoch).total_seconds())
            for time_point in time_points]
    time_points = [time_points[0]]

    property_description = "Amount of time a measurement took"
    durations = [timing["duration"] for timing in benchmark_json["timings"]]

    # Object tracking: a benchmark contains property values (durations)
    # for a single object (piece of software being benchmarked). The ID of
    # this object is some value, like 5.
    # The active set indices are 0, 1, 2, 3, ...
    nr_active_sets = len(time_points)
    active_set_idx = list(range(nr_active_sets))
    active_object_id = nr_active_sets * [5]

    partition_shape = list(benchmark_json["task"]["partition_shape"])

    lue_json = {
        "dataset": {
            "phenomena": [
                {
                    "name": "benchmark",
                    "property_sets": [
                        {
                            "name": "measurement",
                            "description":
                                "Information per benchmark measurement",
                            "object_tracker": {
                                "active_set_index": active_set_idx,
                                "active_object_id": active_object_id
                            },
                            "time_domain": {
                                "clock": {
                                    "epoch": {
                                        "kind": "anno_domini",
                                        "origin": epoch.isoformat(),
                                        "calendar": "gregorian"
                                    },
                                    "unit": time_units,
                                    "tick_period_count": 1
                                },
                                "time_point": time_points
                            },
                            "properties": [
                                {
                                    "name": "duration",
                                    "description": property_description,
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "shape": [len(durations)],
                                    "value": durations
                                },
                                {
                                    "name": "partition_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "shape": [len(partition_shape)],
                                    "value": partition_shape
                                },
                            ]
                        }
                    ]
                }
            ]
        }
    }

    # Write results
    open(lue_json_pathname, "w").write(
        json.dumps(lue_json, sort_keys=False, indent=4))


def execute_command(
        command):

    result = subprocess.call(shlex.split(command))

    if result != 0:
        raise RuntimeError("Error executing {}".format(command))


def import_lue_json(
        lue_json_pathname,
        lue_dataset_pathname):

    command = "{} import --add {} {}".format(
        lue_translate(),
        lue_dataset_pathname,
        lue_json_pathname)
    execute_command(command)


def determine_epoch(
        cluster,
        benchmark,
        experiment):

    partition_shapes = experiment.partition_shapes()

    epoch = None

    for partition_shape in partition_shapes:

        benchmark_pathname = experiment.benchmark_result_pathname(
            cluster.name, partition_shape)
        assert os.path.exists(benchmark_pathname), benchmark_pathname

        benchmark_json = json.loads(open(benchmark_pathname).read())
        benchmark_start = dateutil.parser.isoparse(benchmark_json["start"])

        if epoch is None:
            epoch = benchmark_start
        else:
            epoch = epoch if epoch < benchmark_start else benchmark_start

    return epoch


def import_raw_results(
        cluster,
        benchmark,
        experiment):
    """
    Import all raw benchmark results into a new LUE file

    This is a two step process:
    1. Translate each raw benchmark result into a LUE JSON file
    2. Import all LUE JSON files into a single LUE file
    """
    # Each benchmark containing timings has a start location in time and
    # an overall duration. The location in time can be used to position
    # the benchmark in time. Most likely, all benchmarks are started at
    # different locations in time. The duration is not that relevant.

    # The timings are represented by a location in time and a
    # duration. The location in time is not that relevant. Duration is.

    # To position all benchmarks in time, we need a single starting time
    # point to use as the clock's epoch and calculate the distance of
    # each benchmark's start time point from this epoch.
    epoch = determine_epoch(cluster, benchmark, experiment)

    lue_dataset_pathname = experiment.result_pathname(cluster.name, "lue")

    if os.path.exists(lue_dataset_pathname):
        os.remove(lue_dataset_pathname)

    partition_shapes = experiment.partition_shapes()
    metadata_written = False

    for partition_shape in partition_shapes:

        result_pathname = experiment.benchmark_result_pathname(
            cluster.name, partition_shape)
        assert os.path.exists(result_pathname), result_pathname

        if not metadata_written:
            with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
                benchmark_meta_to_lue_json(
                    result_pathname, lue_json_file.name, cluster, experiment)
                import_lue_json(lue_json_file.name, lue_dataset_pathname)
            metadata_written = True

        with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
            benchmark_to_lue_json(result_pathname, lue_json_file.name, epoch)
            import_lue_json(lue_json_file.name, lue_dataset_pathname)


def create_dot_graph(
        cluster,
        experiment):
    """
    Create a dot graph of the LUE file containing the experiment results
    """

    lue_dataset_pathname = experiment.result_pathname(cluster.name, "lue")
    dot_properties_pathname = os.path.expandvars(
        "$LUE/document/lue_translate/dot_properties.json")
    pdf_graph_pathname = experiment.result_pathname(
        cluster.name, "pdf", kind="graph")

    with tempfile.NamedTemporaryFile(suffix=".dot") as dot_graph_file:
        commands = []
        commands.append(
            "{} export --meta {} {} {}".format(
                lue_translate(),
                dot_properties_pathname,
                lue_dataset_pathname,
                dot_graph_file.name))
        commands.append(
            "dot -Tpdf -o {} {}".format(
                pdf_graph_pathname,
                dot_graph_file.name))

        for command in commands:
            execute_command(command)


def meta_information_dataframe(
        lue_meta_information):

    # TODO: BUG: double delete
    # lue_meta_information.properties["kind"].value[0:0]

    assert \
        lue_meta_information.properties["kind"].value[:] == ["partition_shape"]

    name = lue_meta_information.properties["name"].value[:]
    system_name = lue_meta_information.properties["system_name"].value[:]
    array_shape = lue_meta_information.properties["array_shape"].value[:]

    # Pandas does not support nD array elements. Convert (each) shape
    # to string.
    assert array_shape.dtype == np.uint64
    array_shape = [str(shape) for shape in array_shape]

    meta_information = pd.DataFrame({
            "name": name,
            "system_name": system_name,
            "array_shape": array_shape,
        })

    return meta_information


def measurement_dataframe(
        lue_measurement):

    partition_shape = lue_measurement.properties["partition_shape"].value[:]
    duration = lue_measurement.properties["duration"].value[:]
    assert len(duration) == len(partition_shape)

    duration = pd.DataFrame(duration)
    partition_shape = pd.DataFrame(partition_shape)
    assert (duration.index == partition_shape.index).all()
    measurement = pd.concat([partition_shape, duration], axis=1)

    return measurement


def post_process_raw_results(
        cluster,
        benchmark,
        experiment):
    """
    Create plots and tables from raw benchmark results
    """
    lue_dataset_pathname = experiment.result_pathname(cluster.name, "lue")
    lue_dataset = lue.open_dataset(lue_dataset_pathname)
    lue_benchmark = lue_dataset.phenomena["benchmark"]
    lue_meta_information = \
        lue_benchmark.collection_property_sets["meta_information"]
    lue_measurement = lue_benchmark.property_sets["measurement"]

    rank = len(lue_meta_information.properties["array_shape"].value.shape)
    nr_benchmarks = lue_measurement.object_tracker.active_set_index.shape[0]
    count = lue_measurement.properties["duration"].value.shape[1]

    meta_information = meta_information_dataframe(lue_meta_information)
    name = meta_information.name[0]
    system_name = meta_information.system_name[0]

    measurement = measurement_dataframe(lue_measurement)

    # Select shape columns
    partition = measurement.iloc[:, 0:rank]

    partition.insert(0, "partition_size", partition.product(axis=1))
    assert partition["partition_size"].is_unique
    assert partition["partition_size"].is_monotonic_increasing

    # Select count duration columns
    duration = measurement.iloc[:, rank:]

    duration = pd.DataFrame(
        pd.concat([pd.Series(array) for array in duration.values]) # ,
        # index=[count * [v] for v in range(count)]
    )

    duration.set_index(
        np.repeat([v for v in partition["partition_size"]], count),
        inplace=True)

    # The time point at which the experiment was performed is the epoch
    # of the time domain used to store the durations
    lue_clock = lue_measurement.time_domain.clock
    assert lue_clock.nr_units == 1
    time_point_units = lue_clock.unit

    lue_epoch = lue_clock.epoch
    assert lue_epoch.kind == lue.Epoch.Kind.anno_domini
    assert lue_epoch.calendar == lue.Calendar.gregorian
    time_point = dateutil.parser.isoparse(lue_epoch.origin)


    # String containing time point in local time zone and conventions
    time_point = time_point.astimezone().strftime("%c")

    figure, axes = plt.subplots(
            nrows=1, ncols=1,
            figsize=(15, 5)
            # sharex=False
        )  # Inches...

    # Duration by partition size

    actual_color = sns.xkcd_rgb["denim blue"]

    sns.lineplot(data=duration, ax=axes, color=actual_color, legend=False)

    axes.set_ylabel(u"duration ± 1 std ({})".format(time_point_units))
    axes.yaxis.set_major_formatter(
        ticker.FuncFormatter(
            lambda y, pos: format_duration(y)))

    axes.set_xlabel(u"partition size")
    axes.xaxis.set_major_formatter(
        ticker.FuncFormatter(
            lambda x, pos: format_partition_size(x)))

    figure.suptitle(
        "{}\nPartition shape scaling experiment performed at {}, on {}"
            .format(name, time_point, system_name))

    plot_pathname = experiment.result_pathname(
        cluster.name, "pdf", kind="plot")
    plt.savefig(plot_pathname)


def post_process_results(
        cluster_settings_json,
        benchmark_settings_json,
        experiment_settings_json,
        command_pathname):
    """
    Post-process the results of executing the benchmark script generated
    by the generate_script function.
    """
    job_scheduler = cluster_settings_json["job_scheduler"]
    assert job_scheduler in ["shell", "slurm"]

    if job_scheduler == "slurm":
        cluster = SlurmCluster(cluster_settings_json)
    elif job_scheduler == "shell":
        cluster = ShellCluster(cluster_settings_json)

    benchmark = Benchmark(benchmark_settings_json)
    experiment = Experiment(experiment_settings_json, command_pathname)

    # 1. Import raw results data
    # 2. Add data for plotting
    # 3. Create initial set of plots

    import_raw_results(cluster, benchmark, experiment)

    # TODO Add data underlying the scaling plots to the same dataset
    #      The goal is that publication-ready plots can easily be made
    #      later on
    # add_plot_data()

    create_dot_graph(cluster, experiment)
    post_process_raw_results(cluster, benchmark, experiment)
