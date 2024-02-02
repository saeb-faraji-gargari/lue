#include "lue/gdal/dataset.hpp"
#include "lue/gdal/driver.hpp"
#include <fmt/format.h>
#include <cassert>
#include <cmath>
#include <stdexcept>


namespace lue::gdal {

    /*!
        @brief      Open dataset @name
        @param      open_mode Open mode to use: `GDALAccess::GA_ReadOnly` or `GDALAccess::GA_Update`

        In case the dataset cannot be opened, the smart pointer returned will evaluate to false.
    */
    auto try_open_dataset(std::string const& name, GDALAccess open_mode) -> DatasetPtr
    {
#ifndef NDEBUG
        CPLPushErrorHandler(CPLQuietErrorHandler);
#endif

        DatasetPtr dataset_ptr{static_cast<GDALDataset*>(GDALOpen(name.c_str(), open_mode)), gdal_close};

#ifndef NDEBUG
        CPLPopErrorHandler();
#endif

        return dataset_ptr;
    }


    /*!
        @brief      Open dataset @name
        @param      open_mode Open mode to use: `GDALAccess::GA_ReadOnly` or `GDALAccess::GA_Update`
        @exception  std::runtime_error In case the dataset cannot be opened
    */
    auto open_dataset(std::string const& name, GDALAccess open_mode) -> DatasetPtr
    {
        DatasetPtr dataset_ptr{try_open_dataset(name, open_mode)};

        if (!dataset_ptr)
        {
            throw std::runtime_error("Raster " + name + " cannot be opened");
        }

        return dataset_ptr;
    }


    /*!
        @brief      Create a new dataset, called @name
        @param      shape Shape of the raster bands in the dataset
        @param      nr_bands Number of bands to add to the dataset
        @param      data_type Type of the elements in the raster band(s)
        @exception  std::runtime_error In case the dataset cannot be created
    */
    auto create_dataset(
        std::string const& driver_name,
        std::string const& dataset_name,
        Shape const& shape,
        Count nr_bands,
        GDALDataType data_type) -> DatasetPtr
    {
#ifndef NDEBUG
        CPLPushErrorHandler(CPLQuietErrorHandler);
#endif

        DatasetPtr dataset_ptr{
            driver(driver_name)
                ->Create(dataset_name.c_str(), shape[1], shape[0], nr_bands, data_type, nullptr),
            gdal_close};

#ifndef NDEBUG
        CPLPopErrorHandler();
#endif

        if (!dataset_ptr)
        {
            throw std::runtime_error(fmt::format("Raster {} cannot be created", dataset_name));
        }

        return dataset_ptr;
    }


    auto create_dataset(std::string const& driver_name, std::string const& dataset_name) -> DatasetPtr
    {
        return create_dataset(driver_name, dataset_name, Shape{0, 0}, 0, GDT_Unknown);
    }


    /*!
        @brief      Create a copy of a dataset
        @param      name Name of the new dataset
        @param      clone_dataset Dataset to copy
        @exception  std::runtime_error In case the dataset cannot be created
    */
    auto create_copy(std::string const& name, DatasetPtr& clone_dataset) -> DatasetPtr
    {
        assert(clone_dataset);

#ifndef NDEBUG
        CPLPushErrorHandler(CPLQuietErrorHandler);
#endif

        // TODO let GDAL pick the driver and/or use extension(?)
        DriverPtr driver{gdal::driver("GTiff")};

        DatasetPtr dataset_ptr{
            driver->CreateCopy(name.c_str(), clone_dataset.get(), FALSE, nullptr, nullptr, nullptr),
            gdal_close};

#ifndef NDEBUG
        CPLPopErrorHandler();
#endif

        if (!dataset_ptr)
        {
            throw std::runtime_error("Raster " + name + " cannot be created");
        }

#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 2, 0)
        // The statistics of the clone raster are written to the new raster, which is
        // very inconvenient
        dataset_ptr->ClearStatistics();
#endif

        return dataset_ptr;
    }


    auto nr_raster_bands(GDALDataset& dataset) -> Count
    {
        return dataset.GetRasterCount();
    }


    /*!
        @brief      Return a raster band from the @a dataset passed in
        @param      band_nr Number of band to return. Must be larger than zero.
        @exception  std::runtime_error In case the band cannot be obtained
    */
    auto raster_band(GDALDataset& dataset, Count const band_nr) -> RasterBandPtr
    {
        RasterBandPtr band_ptr{dataset.GetRasterBand(band_nr)};

        if (band_ptr == nullptr)
        {
            throw std::runtime_error(fmt::format("Band {} cannot be obtained", band_nr));
        }

        return band_ptr;
    }


    /*!
        @overload
        @brief      Return the first raster band.
    */
    auto raster_band(GDALDataset& dataset) -> RasterBandPtr
    {
        return raster_band(dataset, 1);
    }


    /*!
        @overload
        @brief      Return the datatype of the elements in the dataset
    */
    auto data_type(GDALDataset& dataset) -> GDALDataType
    {
        RasterBandPtr band_ptr{raster_band(dataset)};

        return data_type(*band_ptr);
    }


    /*!
        @overload
        @brief      Return the datatype of the elements in the dataset named @a name
    */
    auto data_type(std::string const& name) -> GDALDataType
    {
        DatasetPtr dataset_ptr{open_dataset(name, GDALAccess::GA_ReadOnly)};

        return data_type(*dataset_ptr);
    }


    /*!
        @brief      Return the shape of the rasters in the @a dataset
    */
    auto shape(GDALDataset& dataset) -> Shape
    {
        return {dataset.GetRasterYSize(), dataset.GetRasterXSize()};
    }


    auto up_is_north(GeoTransform const& geo_transform) -> bool
    {
        return geo_transform[2] == 0.0 && geo_transform[4] == 0.0 && geo_transform[5] < 0.0;
    }


    auto cells_are_square(GeoTransform const& geo_transform) -> bool
    {
        return geo_transform[1] == std::abs(geo_transform[5]);
    }


    /*!
        @brief      Return the information about the affine transformation from raster coordinates
                    (row, col indices) to georeferenced coordinates (projected or geographic
                    coordinates)
        @return     Collection of six coefficients

        Index | Meaning
        ----- | -------
        0 | X-coordinate of the upper-left corner of the upper-left cell (west)
        1 | Cell width
        2 | Row rotation (typically zero)
        3 | Y-coordinate of the upper-left corner of the upper-left cell (north)
        4 | Column rotation (typically zero)
        5 | Cell height (negative value for a north-up raster)
    */
    auto geo_transform(GDALDataset& dataset) -> GeoTransform
    {
        GeoTransform result{};
        dataset.GetGeoTransform(result.data());

        if (!up_is_north(result))
        {
            throw std::runtime_error("Only rasters oriented to the north are currently supported");
        }

        if (!cells_are_square(result))
        {
            throw std::runtime_error("Only rasters with square cells are currently supported");
        }

        return result;
    }


    auto set_geo_transform(GDALDataset& dataset, GeoTransform const& geo_transform) -> void
    {
        // For some reason, GDAL wants a non-const pointer...
        dataset.SetGeoTransform(const_cast<double*>(geo_transform.data()));
    }

}  // namespace lue::gdal
