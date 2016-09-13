#
# About this simulation:
#
# NOTE that this is an example input file demonstrating
# how to specify a raster pile input

sim=TitanSimulation(overwrite_output=True)

sim.setGIS(
    gis_format='GDAL', 
    gis_map='PATH/TO/MAP/file.tif',
)

sim.setScale(
    length_scale=20000.0,
    gravity_scale=9.8,
    height_scale=1.0,
)

sim.setNumProp(
    AMR=False,
    number_of_cells_across_axis=32,
    order='First',
)
sim.setMatModel(
    model='TwoPhases-Pitman-Le',#Using a two-phase material here, but a single phase one should work as well
    int_frict=37.0,
    bed_frict=27.0
)
sim.addPile(
    pile_type='RASTER',#Set pile type to 'RASTER'
    height=1.0,#Specify an approximate ellipse and height (important for AMR)
    center=[377675.265,-4347880.186],
    radii=[1871.0, 838.7],
    orientation=-30.0,
    Vmagnitude=0.0,
    Vdirection=0.0,
    vol_fract=0.7,
    rasterfile='PATH/TO/PILEHEIGHT/file.tif'#Provide location of raster pile height
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
