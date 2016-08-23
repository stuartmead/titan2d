/*******************************************************************
 * Copyright (C) 2003 University at Buffalo
 *
 * This software can be redistributed free of charge.  See COPYING
 * file in the top distribution directory for more details.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author: Geophysical Mass Flow Group, University at Buffalo
 * Description:
 *
 *******************************************************************
 * $Id: $
 */

#ifndef GMFG_GDALAPI_H
#define GMFG_GDALAPI_H

#include "GisApi.h"

int
Initialize_GDAL_data(const char *fullGisPath);

int
load_GDAL_data();

int
Initialize_GDAL_data_grid(const char *fullGispath, Gis_Grid grid);

int
load_GDAL_data_grid(Gis_Grid grid);

#endif // GMFG_GDALAPI_H
