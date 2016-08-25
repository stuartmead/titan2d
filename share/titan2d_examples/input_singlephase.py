#
# About this simulation:
#
# NOTE what this simulation uses a different DEM
# This simulation require bigger region and to keep 
# example sizes small, the fine map was coarsened
#
# it also require much more iterations and execution time
# because pile project itself further

sim=TitanSimulation(overwrite_output=True)

sim.setGIS(
    gis_format='GDAL', 
    gis_map='DEM/ruapehu_utm.vrt',
    region_limits=[374677.05,395603.56,-4351473.78,-4337363.57],
)

sim.setScale(
    length_scale=20000.0,
    gravity_scale=9.8,
    height_scale=1.0,
)

sim.setNumProp(
    AMR=False,
    number_of_cells_across_axis=16,
    order='First',
)
sim.setMatModel(
    model='Coulomb',#TwoPhases-Pitman-Le',
    int_frict=37.0,
    bed_frict=27.0
)
sim.addPile(
    pile_type='RASTER',
    height=1.0,
    center=[377675.265,-4347880.186],
    radii=[1871.0, 838.7],
    orientation=-30.0,
    Vmagnitude=0.0,
    Vdirection=0.0,
    rasterfile='mang_upper.tif'
)

sim.setTimeProps(
    max_time=1800,
)

sim.setTimeSeriesOutput(
    vizoutput='xdmf',
    dtime=1.0,
)

#if not set default settings are used
sim.setOutlineProps(
    output_prefix='',
    enabled=True,
    max_linear_size=1024,
    init_size='AMR'
)
#dump restart file every dtime simulated seconds and/or diter interations
sim.setRestartOutput(
    enabled=True,
    dtime=1.0,#float or None, default=None
    diter=None,#int or None, default=None
    keep_all=True, #True - keep all restart files or False - only last default=False?
    keep_redundant_data=True,
    output_prefix='restart', #directory name where restart h5 and individual xdmf are stored, prefix for time series xdmf, default='restart'
)

#start simulation
sim.run()
