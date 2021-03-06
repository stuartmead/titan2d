#include <iostream>
#include <cstdlib>
using namespace std;

#include <gdal_priv.h>
#include <ogr_spatialref.h>

#include "GisApi.h"
#include "gmfg_GdalApi.h"

extern Gis_Grid gis_grid;

int Initialize_GDAL_data(const char *fullGispath)
{
    GDALDataset *dataset;
    int nx, ny;
    double adfGeoTransform[6];
    Gis_Head gishead;

    // Initialize GDAL
    GDALAllRegister();

    // open GIS dataset
    dataset = (GDALDataset *) GDALOpen(fullGispath, GA_ReadOnly);
    if(dataset == NULL)
    {
        cout << "GIS ERROR: Unable to open GIS DATA" << endl;
        return (-4);
    }

    // check if the data is in UTM coordinates
    OGRSpatialReference *oSRS = new OGRSpatialReference(NULL);
    char *pszProjection = (char *) dataset->GetProjectionRef();
    if(oSRS->importFromWkt(&pszProjection) == CE_None)
    {
        int pbxmax;
        int zone = oSRS->GetUTMZone(&pbxmax);
        if(zone == 0)
        {
            if ( !strcmp(oSRS->GetAuthorityCode(NULL), "27200") || !strcmp(oSRS->GetAuthorityCode(NULL), "2193"))
            {
                cout << "WARNING:" << endl;
                cout << "The mapset seems to be in coordinates, other than UTM." << endl;
                cout << "TITAN needs the mapset to be in UTM coordinates." << endl;
                cout << "Consider using \"gdalwarp\" to transform." << endl;
                cout << "We are passing this through, because the EPSG" << " is NZGD/NZTM" << endl;
            }
            else
            {
                cout << "FATAL ERROR:" << endl;
                cout << "The mapset seems to be in coordinates, other than UTM." << endl;
                cout << "TITAN needs the mapset to be in UTM coordinates." << endl;
                cout << "Consider using \"gdalwarp\" to transform." << endl;
                cout << "Authority code is " << oSRS->GetAuthorityCode(NULL) << endl;
                cout << "If you sure about the coordinate system" << " change ZONE to something other than zero" << endl;
                return (-4);
            }
        }
    }
    else
    {
        cout << "FATAL ERROR:" << endl;
        cout << "Can't read the projection information from mapset" << endl;
        return (-4);
    }
    nx = dataset->GetRasterXSize();
    ny = dataset->GetRasterYSize();
    if(nx < 1 || ny < 1)
    {
        cout << "FATAL ERROR:" << endl;
        cout << "GDAL could not read, metadata from the mapset." << endl;
        return (-4);
    }
    gishead.ncols = nx;
    gishead.nrows = ny;

    // read the window information
    if(dataset->GetGeoTransform(adfGeoTransform) == CE_None)
    {
        if(adfGeoTransform[2] != 0 || adfGeoTransform[4] != 0)
        {
            cout << "FATAL ERROR:" << endl;
            cout << "Raster Map is not North-up." << endl;
            cout << "Consider using \"gdalwarp\" to transform" << endl;
            return (-4);
        }
        //adfGeoTransform[0] - top left x
        //adfGeoTransform[1] - w-e pixel resolution
        //adfGeoTransform[3] - top left y
        //adfGeoTransform[5] - n-s pixel resolution (negative value)
        gishead.xmin = adfGeoTransform[0];
        gishead.wresolution = fabs(adfGeoTransform[1]);
        gishead.xmax = gishead.xmin + nx * gishead.wresolution;
        gishead.ymax = adfGeoTransform[3];
        gishead.resolution = fabs(adfGeoTransform[5]);
        gishead.ymin = gishead.ymax - gishead.resolution * ny;
        gishead.Format = GDAL;
        gishead.compressed = 1; // doesn't mean anything if GDAL is being used
        gis_grid.ghead = gishead;
        gis_grid.ghead.datafile = strdup(fullGispath);

        return 0;
    }
    return -4;
}

int load_GDAL_data()
{
    GDALRasterBand *poBand;
    GDALAllRegister();
    int nrows, ncols;

    // open GIS dataset
    GDALDataset *dataset = (GDALDataset *) GDALOpen(gis_grid.ghead.datafile, GA_ReadOnly);

        // allocate memory
        // must use malloc as delete_GIS_data uses free
        nrows = gis_grid.ghead.nrows;
        ncols = gis_grid.ghead.ncols;
        gis_grid.elev = (float **) malloc(nrows * sizeof(float *));
        for(int irow = 0; irow < nrows; irow++)
            gis_grid.elev[irow] = (float *) malloc(ncols * sizeof(float));

        poBand = dataset->GetRasterBand(1);
        if(poBand->GetColorTable() != NULL)
        {
            cout << "Band has a color table with " << poBand->GetColorTable()->GetColorEntryCount() << " entries." << endl;
            cout << "Can't process RGB information." << endl;
            return -4;
        }

        // check if the raster band size matches with the size in header
        int nXSize = poBand->GetXSize();
        int nYSize = poBand->GetYSize();
        if((nXSize != ncols) || (nYSize != nrows))
        {
            cout << "Size mismatch between, header and raster band data." << endl;
            cout << "Header rows, cols =" << ncols << ", " << nrows << endl;
            cout << "Raster rows, cols =" << nXSize << ", " << nYSize << endl;
            return -4;
        }
        // read the data,
        for(int i = 0; i < nrows; i++)
            if(poBand->RasterIO(GF_Read, 0, i, ncols, 1, gis_grid.elev[i], ncols, 1, GDT_Float32, 0, 0) != CE_None)
            {
                cout << "FATAL ERROR:" << endl;
                cout << "Failed to read data from mapset." << endl;
                return -4;
            }
        return 0;
}

int
Initialize_GDAL_data_grid(std::string fullGispath, Gis_Grid &grid)
{
    GDALDataset *dataset;
    int nx, ny;
    double adfGeoTransform[6];
    Gis_Head gishead;

    // Initialize GDAL
    GDALAllRegister();
    printf("Reading gdal raster grid from %s\n", fullGispath.c_str());
    
    // open GIS dataset
    dataset = (GDALDataset *) GDALOpen(fullGispath.c_str(), GA_ReadOnly);
    if(dataset == NULL)
    {
        cout << "GIS ERROR: Unable to open GIS DATA" << endl;
        return (-4);
    }

    // check if the data is in UTM coordinates
    OGRSpatialReference *oSRS = new OGRSpatialReference(NULL);
    char *pszProjection = (char *) dataset->GetProjectionRef();
    if(oSRS->importFromWkt(&pszProjection) == CE_None)
    {
        int pbxmax;
        int zone = oSRS->GetUTMZone(&pbxmax);
        if(zone == 0)
        {
            if (!strcmp(oSRS->GetAuthorityCode(NULL), "27200") || !strcmp(oSRS->GetAuthorityCode(NULL), "2193"))
            {
                cout << "WARNING:" << endl;
                cout << "The mapset seems to be in coordinates, other than UTM." << endl;
                cout << "TITAN needs the mapset to be in UTM coordinates." << endl;
                cout << "Consider using \"gdalwarp\" to transform." << endl;
                cout << "We are passing this through, because the EPSG " << "is NZGD/NZTM" << endl;
            }
            else
            {
                cout << "FATAL ERROR:" << endl;
                cout << "The mapset seems to be in coordinates, other than UTM." << endl;
                cout << "TITAN needs the mapset to be in UTM coordinates." << endl;
                cout << "Consider using \"gdalwarp\" to transform." << endl;
                cout << "Authority code is " << oSRS->GetAuthorityCode(NULL) << endl;
                cout << "If you sure about the coordinate system" << "change ZONE to something other than zero" << endl;
                return (-4);
            }
        }
    }
    else
    {
        cout << "FATAL ERROR:" << endl;
        cout << "Can't read the projection information from mapset" << endl;
        return (-4);
    }
    nx = dataset->GetRasterXSize();
    ny = dataset->GetRasterYSize();
    if(nx < 1 || ny < 1)
    {
        cout << "FATAL ERROR:" << endl;
        cout << "GDAL could not read, metadata from the mapset." << endl;
        return (-4);
    }
    gishead.ncols = nx;
    gishead.nrows = ny;
    cout << "Header cols, rows =" << gishead.ncols << ", " << gishead.nrows << endl;
    // read the window information
    if(dataset->GetGeoTransform(adfGeoTransform) == CE_None)
    {
        if(adfGeoTransform[2] != 0 || adfGeoTransform[4] != 0)
        {
            cout << "FATAL ERROR:" << endl;
            cout << "Raster Map is not North-up." << endl;
            cout << "Consider using \"gdalwarp\" to transform" << endl;
            return (-4);
        }
        //adfGeoTransform[0] - top left x
        //adfGeoTransform[1] - w-e pixel resolution
        //adfGeoTransform[3] - top left y
        //adfGeoTransform[5] - n-s pixel resolution (negative value)
        gishead.xmin = adfGeoTransform[0];
        gishead.wresolution = fabs(adfGeoTransform[1]);
        gishead.xmax = gishead.xmin + nx * gishead.wresolution;
        gishead.ymax = adfGeoTransform[3];
        gishead.resolution = fabs(adfGeoTransform[5]);
        gishead.ymin = gishead.ymax - gishead.resolution * ny;
        gishead.Format = GDAL;
        gishead.compressed = 1; // doesn't mean anything if GDAL is being used
        grid.ghead = gishead;
        //grid.ghead.datafile = strdup(fullGispath.c_str());
        return 0;
    }
    return -4;
}

int
load_GDAL_data_grid(Gis_Grid &grid)
{
    GDALRasterBand *poBand;
    GDALAllRegister();
    int nrows, ncols;
    printf("Loading gdal raster grid from %s\n", grid.ghead.datafile);

	GDALDataset *dataset = (GDALDataset *) GDALOpen(grid.ghead.datafile, GA_ReadOnly);

        // allocate memory
        // must use malloc as delete_GIS_data uses free
        nrows = grid.ghead.nrows;
        ncols = grid.ghead.ncols;
        grid.elev = (float **) malloc(nrows * sizeof(float *));
        for(int irow = 0; irow < nrows; irow++)
            grid.elev[irow] = (float *) malloc(ncols * sizeof(float));

        poBand = dataset->GetRasterBand(1);
        if(poBand->GetColorTable() != NULL)
        {
            cout << "Band has a color table with " << poBand->GetColorTable()->GetColorEntryCount() << " entries." << endl;
            cout << "Can't process RGB information." << endl;
            return -4;
        }

        // check if the raster band size matches with the size in header
        int nXSize = poBand->GetXSize();
        int nYSize = poBand->GetYSize();
        if((nXSize != ncols) || (nYSize != nrows))
        {
            cout << "Size mismatch between, header and raster band data." << endl;
            cout << "Header rows, cols =" << ncols << ", " << nrows << endl;
            cout << "Raster rows, cols =" << nXSize << ", " << nYSize << endl;
            return -4;
        }
        // read the data,
        for(int i = 0; i < nrows; i++)
            if(poBand->RasterIO(GF_Read, 0, i, ncols, 1, grid.elev[i], ncols, 1, GDT_Float32, 0, 0) != CE_None)
            {
                cout << "FATAL ERROR:" << endl;
                cout << "Failed to read data from mapset." << endl;
                return -4;
            }
        return 0;
}
