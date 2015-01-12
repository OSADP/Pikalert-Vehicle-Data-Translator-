
# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: plots.py,v $                                           #
#       Version: $Revision: 1.5 $  Dated: $Date: 2014/07/03 17:02:03 $           #
#                                                                                #
# ============================================================================== #

import math
import backend_sys_path
import sys
import os
import json
import cgi
import string
import time
from netCDF4 import Dataset
import data_time_index
import numpy
from numpy import ma
import base64
import StringIO
import pytz
import matplotlib
matplotlib.use('Agg')  # force the antigrain backend
from matplotlib import rc
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.cbook import iterable
import datetime
from numpy import arange
from matplotlib.dates import DayLocator, HourLocator, DateFormatter, drange, num2date
import alerts as alert_mod
import processed_file
import sites as sitedict # for plot site label
import matplotlib.pyplot as plt # for changing plot layout


def get_obs_stats(cf, site_num):

    start_time = time.time()
    date_string = time.strftime("%Y%m%d%H%M", time.gmtime(start_time))
    (obs_stats_json, obs_stats_json_time) = data_time_index.get_latest_file_using_time(backend_sys_path.File_time_delta, date_string, cf.obs_stats_json_dir)
    obs_stats_dict = {}
    if obs_stats_json != "":
        if os.path.exists(obs_stats_json):
            obs_stats_dict = json.load(open(obs_stats_json))

    site_string = str(site_num)
    if site_string in obs_stats_dict:
        return obs_stats_dict[site_string]
    else:
        return {}


def get_time_alerts_tmts(cf, site_num):
    """ Retrieves time alerts and treatment recommendations.

    Parameters
    ----------
    cf : dict
        A state dictionary such as backend_sys_path.State_dictionary[state]
    hour_offset : int
        Hour offset from GMT
    site_num : int
        Desired site number to plot

    Returns
    -------
    lists
    """

    start_time = time.time()
    date_string = time.strftime("%Y%m%d%H%M", time.gmtime(start_time))
    (plots_json, plots_json_time) = data_time_index.get_latest_file_using_time(backend_sys_path.File_time_delta, date_string, cf.plots_dir)
    print "directory, plots_json file: ", cf.plots_dir, plots_json
    plots_dict = {}
    if plots_json != "":
        if os.path.exists(plots_json):
            plots_dict = json.load(open(plots_json))
    site_string = str(site_num)
    
    if "rdwx_alerts" in plots_dict and site_string in plots_dict["rdwx_alerts"]:
        time_alerts = plots_dict["rdwx_alerts"][site_string]
    else:
        time_alerts = []
    if "tmt_alerts" in plots_dict and site_string in plots_dict["tmt_alerts"]:
        time_tmts = plots_dict["tmt_alerts"][site_string]
    else:
        time_tmts = []
        
    if len(time_alerts) > 72:
        time_alerts = time_alerts[0:72]
    if len(time_tmts) > 72:
        time_tmts = time_tmts[0:72]
          
    return time_alerts, time_tmts

def get_site_index(nc_file, site_num):
    """ Gets site index from nc file.

    Parameters
    ----------
    nc_file : str
        NetCDF file to get site index from (i.e. road weather or treatments file)
    site_num : int
        The site number of desired site to plot.

    Returns
    -------
    site_index
    """
    
    sites = nc_file.variables["site_list"][:]
    site_index = numpy.where(sites == site_num)
    
    if len(site_index[0]) == 0:
        return ([],)

    return site_index

def get_time_info(nc_file, current_utc_hour, tz_info):
    """ Retrieves time frame of plots starting at the current UTC hour and ending 3 days after the current hour. 

    Parameters
    ----------
    nc_file : object
        The netCDF dataset that provides the start time of data.
    current_utc_hour : float
        The current UTC hour.

    Returns
    -------
    dates : list
        A list of Gregorian ordinals (floats) from the current UTC time to 3 days later
    """

    #init_time = nc_file.variables["forc_time"][:][0]
    init_time = nc_file.variables["forc_time"][:]
    print "init_time: ", init_time
    init_time = init_time + current_utc_hour * 3600
    #begin_time = datetime.datetime.fromtimestamp(init_time, tz=tz_info)
    #end_time = datetime.datetime.fromtimestamp(init_time + (3*24*3600), tz=tz_info) # only plot 3 days
    begin_time = datetime.datetime.fromtimestamp(init_time)
    end_time = datetime.datetime.fromtimestamp(init_time + (3*24*3600)) # only plot 3 days
    print "tz_info: ", tz_info
    print "begin_time: ", begin_time
    print "end_time: ", end_time
    delta = datetime.timedelta(hours=1) # Move time interval before all var arrays are set
    dates = drange(begin_time, end_time, delta)
    return dates

class RdwxNetcdfFileData(object):

    def __init__(self, rdwx_fpath, site_num, current_utc_hour, tz_info):
        """
        Parameters
        ----------
        rdwx_fpath : str
            A string containing the file path to the road weather nc file
        tmt_fpath : str
            A string containing the file path to the treatments nc file
        site_num : int
            The site number for the site to plot
        current_utc_hour: int
            The current hour in utc time
        """

        self.rdwx_error = False
        self.tz_info = tz_info
        self.current_utc_hour = current_utc_hour
        
        print "rdwx_fpath: ", rdwx_fpath

        if not os.path.exists(rdwx_fpath):
            self.rdwx_error = True
            print "rdwx_fpath does not exist"

        if self.rdwx_error:
            self.rdwx_file = rdwx_fpath
            self.site_num = site_num
            self.dates = []
            self.rdwx_nc = ""
            self.rdwx_site_index = ([],)
            self.prob_rain = []
            self.prob_snow = []
            self.prob_ice = []
            self.precip_accum_snow = []
            self.precip_accum_rain = []
            self.precip_accum_ice = []
            self.snow_accum = []
            self.temps = []
            self.dewps = []
            self.temp_units = "deg F"
            self.dewp_units = "deg F"
            self.wind_spd = []
            self.wind_spd_units = "mph"
            self.wind_u_dir = []
            self.wind_v_dir = []
        else:
            self.rdwx_file = rdwx_fpath
            self.site_num = site_num
            print "trying to open dataset: ", self.rdwx_file

            self.rdwx_nc = Dataset(self.rdwx_file, "r")
            print "rdwx_nc: ", self.rdwx_nc
            self.rdwx_site_index = get_site_index(self.rdwx_nc, site_num)
            self.dates = get_time_info(self.rdwx_nc, current_utc_hour, tz_info)
            self.prob_snow = None
            self.prob_rain = None
            self.prob_ice = None
            self.precip_accum_snow = None
            self.precip_accum_rain = None
            self.precip_accum_ice = None
            self.temps = None
            self.dewps = None
            self.temp_units = None
            self.dewp_units = None
            self.snow_accum = None
            self.wind_spd = None
            self.wind_spd_units = None
            self.wind_u_dir = None
            self.wind_v_dir = None
            
    def extract_precip_info(self):
        """ Retrieves the probabilities for rain, snow, and ice and the corresponding accumulation of each precipitation type

        Returns
        -------
        lists
        """

        nan = float('nan')
        if self.prob_rain == None and self.prob_snow == None and self.prob_ice == None and self.precip_accum_snow == None and self.precip_accum_rain == None and self.precip_accum_ice == None:
            if self.rdwx_site_index != ():
                print "getting prob_precip"
                prob_precip = ma.filled(self.rdwx_nc.variables["prob_precip01"][self.rdwx_site_index], 0).flatten()
                print "got precip"
                prob_rain = ma.filled(self.rdwx_nc.variables["cprob_rain"][self.rdwx_site_index], 0).flatten()
                print "got rain"
                prob_snow = ma.filled(self.rdwx_nc.variables["cprob_snow"][self.rdwx_site_index], 0).flatten()
                print "got snow"
                prob_ice = ma.filled(self.rdwx_nc.variables["cprob_ice"][self.rdwx_site_index], 0).flatten()
                print "got prob ice"

                #prob_precip = numpy.copy(cdf_prob_precip)
                prob_precip[prob_precip==9.9692099683868690e+36] = 0
                prob_rain[prob_rain==9.9692099683868690e+36] = 0
                prob_snow[prob_snow==9.9692099683868690e+36] = 0
                prob_ice[prob_ice==9.9692099683868690e+36] = 0

                prob_precip = prob_precip[self.current_utc_hour:]
                print "reset prob precip"
                prob_rain = prob_rain[self.current_utc_hour:]
                print "reset prob rain", prob_rain
                prob_snow = prob_snow[self.current_utc_hour:]
                print "reset prob snow"
                prob_ice = prob_ice[self.current_utc_hour:]
                print "reset prob ice"


                """
                precip_accum_var = self.rdwx_nc.variables[backend_sys_path.Met_vars.rdwx_precip_accum_var]
                precip_accum = ma.filled(precip_accum_var[self.rdwx_site_index], nan).flatten()
                precip_accum = precip_accum[self.current_utc_hour:]
                # get different types of precipitation by multiplying by the probability of each type
                precip_accum_snow = precip_accum * prob_snow # get snow accum amt
                precip_accum_rain = precip_accum * prob_rain # get rain accum amt
                precip_accum_ice = precip_accum * prob_ice # get ice accum amt
                """

                print "prob_rain shape: ", prob_rain.shape
                print "prob_snow shape: ", prob_snow.shape
                print "prob_precip shape: ", prob_precip.shape
                print "prob_ice shape: ", prob_ice.shape
                print "prob_rain: ", prob_rain
                print "prob_precip: ", prob_precip
                self.prob_rain=(prob_rain * prob_precip)
                print "mult prob_rain"
                self.prob_snow=(prob_snow * prob_precip)
                print "mult prob_snow"
                self.prob_ice=(prob_ice * prob_precip)
                print self.prob_ice
                """
                self.precip_accum_snow = precip_accum_snow
                self.precip_accum_rain = precip_accum_rain
                self.precip_accum_ice = precip_accum_ice
                """
            else:
                self.prob_rain = []
                self.prob_snow = []
                self.prob_ice = []
                """
                self.precip_accum_snow = []
                self.precip_accum_rain = []
                self.precip_accum_ice = []
                """

        #return self.prob_rain, self.prob_snow, self.prob_ice, self.precip_accum_snow, self.precip_accum_rain, self.precip_accum_ice
        print "returning"
        return self.precip_accum_snow, self.precip_accum_rain, self.precip_accum_ice

    def extract_snow_accum(self):
        """ Retrieves snow accumulation.

        Returns
        -------
        list
        """
        
        nan = float('nan')
        if self.snow_accum == None:
            if self.rdwx_site_index != ():
                snow_accum_var = self.rdwx_nc.variables["snow_accum_total_inches"]
                snow_accum = ma.filled(snow_accum_var[self.rdwx_site_index], 0).flatten()
                snow_accum[snow_accum==9.9692099683868690e+36] = 0
                snow_accum = snow_accum[self.current_utc_hour:]
                self.snow_accum = snow_accum
            else:
                self.snow_accum = []

        return self.snow_accum

    def extract_temp_dewp(self):
        """ Retrieves temperature and dewpoint information and makes necessary unit conversions.

        Returns
        -------
        lists
        """

        nan = float('nan')
        if self.temps == None and self.dewps == None and self.temp_units == None and self.dewp_units == None:
            if self.rdwx_site_index != ():
                temp_var = self.rdwx_nc.variables[backend_sys_path.Met_vars.rdwx_temp_var]
                dewp_var = self.rdwx_nc.variables[backend_sys_path.Met_vars.rdwx_dewp_var]
                temp_var = self.rdwx_nc.variables[backend_sys_path.Met_vars.rdwx_temp_var]
                dewp_var = self.rdwx_nc.variables[backend_sys_path.Met_vars.rdwx_dewp_var]

                temps = ma.filled(temp_var[self.rdwx_site_index], nan).flatten()
                dewps = ma.filled(dewp_var[self.rdwx_site_index], nan).flatten()
                temps[temps==9.9692099683868690e+36] = nan
                dewps[temps==9.9692099683868690e+36] = nan

                temps = temps[self.current_utc_hour:]
                dewps = dewps[self.current_utc_hour:]

                temp_units = temp_var.units
                dewp_units = dewp_var.units # Grab dewp units just to make sure they match temp

                # convert temp if not in deg F and to change label if in fact in deg F
                if temp_units == 'degrees Celsius':
                    temps = (9.0/5.0) * (temps) + 32          
                    temp_units = 'deg F'        
                if temp_units == 'kelvin' and temps: 
                    temps = (9.0/5.0) * (temps-273.15) + 32          
                    temp_units = 'deg F'
                if temp_units == 'degrees Fahrenheit':
                    temp_units = 'deg F'        
                if dewp_units == 'degrees Celsius':
                    dewps = (9.0/5.0) * (dewps) + 32          
                if dewp_units == 'kelvin' and dewps: 
                    dewps = (9.0/5.0) * (dewps-273.15 ) + 32

                self.temps = temps
                self.dewps = dewps
                self.temp_units = temp_units
                self.dewp_units = dewp_units
            else:
                self.temps = []
                self.dewps = []
                self.temp_units = "deg F"
                self.dewp_units = "deg F"

        print "temps: ", self.temps
        return self.temps, self.dewps, self.temp_units, self.dewp_units

    def extract_wind_info(self):
        """ Retrieves wind speed and wind direction and makes necessary unit conversions.

        Returns
        -------
        lists
        """

        nan = float('nan')
        if self.wind_spd == None and self.wind_u_dir == None and self.wind_v_dir == None and self.wind_spd_units == None:
            if self.rdwx_site_index != ():
                wind_spd_var = self.rdwx_nc.variables[backend_sys_path.Met_vars.rdwx_wind_spd_var]
                wind_spd_units = wind_spd_var.units
                wind_spd = ma.filled(wind_spd_var[self.rdwx_site_index], nan).flatten()
                wind_spd[wind_spd==9.9692099683868690e+36] = nan
                wind_spd = wind_spd[self.current_utc_hour:]

                # convert wind speed if not in mph and to change label if in fact in mph
                if wind_spd_units == 'meters per second':
                    wind_spd = 2.23694 * wind_spd
                    wind_spd_units = 'mph'
                if wind_spd_units == 'miles per hour':
                    wind_spd_units = 'mph'

                wind_dir = ma.filled(self.rdwx_nc.variables["wind_dir"][self.rdwx_site_index], nan).flatten()
                wind_dir = wind_dir[self.current_utc_hour:]
                wind_dir[wind_dir==9.9692099683868690e+36] = nan
                u = ma.filled(-(10) * numpy.sin(wind_dir*numpy.pi/180.), nan)
                v = ma.filled(-(10) * numpy.cos(wind_dir*numpy.pi/180.), nan)

                self.wind_spd = wind_spd
                self.wind_u_dir = u
                self.wind_v_dir = v
                self.wind_spd_units = wind_spd_units
            else:
                self.wind_spd = []
                self.wind_u_dir = []
                self.wind_v_dir = []
                self.wind_spd_units = "mph"

        return self.wind_spd, self.wind_u_dir, self.wind_v_dir, self.wind_spd_units

class TmtNetcdfFileData(object):

    def __init__(self, tmt_fpath, site_num, current_utc_hour, tz_info):
        """
        Parameters
        ----------
        tmt_fpath : str
            A string containing the file path to the treatments nc file
        site_num : int
            The site number for the site to plot
        current_utc_hour: int
            The current hour in utc time
        """
        
        self.tmt_error = False
        self.tz_info = tz_info
        self.current_utc_hour = current_utc_hour
        
        if not os.path.exists(tmt_fpath):
            self.tmt_error = True
        if self.tmt_error:
            self.tmt_file = tmt_fpath
            self.site_num = site_num
            self.dates = []
            self.tmt_nc = ""
            self.tmt_site_index = ([],)
            self.road_temps = []
            self.snow_accum = []
        else:
            self.tmt_file = tmt_fpath
            self.site_num = site_num
            self.tmt_nc = Dataset(self.tmt_file, "r")
            self.tmt_site_index = get_site_index(self.tmt_nc, site_num)
            self.dates = get_time_info(self.tmt_nc, current_utc_hour, tz_info)
            self.road_temps = None
            self.snow_accum = None

            if self.tmt_site_index == ([],):
                self.dates = []
                self.road_temps = []
                self.snow_accum = []

    def extract_road_temps(self):
        """ Retrieves road temperatures.

        Returns
        -------
        list
        """

        nan = float('nan')
        if self.road_temps == None:
            if self.tmt_site_index != ([],):
                road_temp_var = self.tmt_nc.variables[backend_sys_path.Met_vars.tmt_road_temp_var]
                road_temps = ma.filled(road_temp_var[self.tmt_site_index], nan).flatten()
                road_temps[road_temps==9.9692099683868690e+36] = nan
                road_temps = road_temps[self.current_utc_hour:]
                self.road_temps = road_temps
            else:
                self.road_temps = []

        return self.road_temps

    def extract_snow_accum(self):
        """ Retrieves snow accumulation.

        Returns
        -------
        list
        """
        
        nan = float('nan')
        if self.snow_accum == None:
            if self.tmt_site_index != ([],):
                snow_accum_var = self.tmt_nc.variables["snow_depth_inches"]
                snow_accum = ma.filled(snow_accum_var[self.tmt_site_index], nan).flatten()
                snow_accum[snow_accum==9.9692099683868690e+36] = nan
                snow_accum = snow_accum[self.current_utc_hour:]
                self.snow_accum = snow_accum
            else:
                self.snow_accum = []

        return self.snow_accum

def get_summary_plot(cf, rdwx_fpath, tmt_fpath, site_num, current_utc_hour, state_timezone, outfile=""):
    """ Makes a plot summary of road conditions.

    Parameters
    ----------
    cf : dict
        State dictionary such as backend_sys_path.State_dictionary[state]
    rdwx_fpath : str
        A string containing the file path for the road weather nc file
    tmt_fpath : str
        A string containing the file path for the treatments nc file
    site_num : int
        The site number to be plotted
    current_utc_hour : int
        The current utc hour
    state_timezone : tz
        The state timezone given by pytz.timezone()
    outfile : str
        Optional argument for user to save plots to an outfile, left empty returns a base 64 encoded plot
    """

    print "in get_summary_plot"
    all_sites = sitedict.get_rdwx_sites(cf)
    print all_sites
    try:
        site_num_test = all_sites[site_num]
    except:
        return
    
    # Retrieve data from netcdf files
    print "getting rdwx data"
    rdwx_nc_object = RdwxNetcdfFileData(rdwx_fpath, site_num, current_utc_hour, state_timezone)

    print "getting tmt data"
    tmt_nc_object = TmtNetcdfFileData(tmt_fpath, site_num, current_utc_hour, state_timezone)

    print "extracting data precip"
    rdwx_nc_object.extract_precip_info()
    print "extracting temp dewp"
    rdwx_nc_object.extract_temp_dewp()

    print "extracting wind info"
    rdwx_nc_object.extract_wind_info()
    print "extracting snow accum"
    rdwx_nc_object.extract_snow_accum()
    print "extracting road temps"
    tmt_nc_object.extract_road_temps()
    print "finished extracting data"

    # Retrieve time alerts and recommended treatments
    time_alerts, time_tmts = get_time_alerts_tmts(cf, site_num)
    print "getting alerts"
    
    fig=plt.figure(figsize=(7,8), dpi=150)

    # Create plots and create a consolidated list of these plots
    axs = []
    axs = make_alert_bar(axs, time_alerts, rdwx_nc_object, all_sites, site_num)
    axs, snow_accum_plot_bool = make_snow_accum_plot(axs, rdwx_nc_object)
    axs = make_precip_graph(axs, rdwx_nc_object, snow_accum_plot_bool)
    axs = make_temp_dewp_graph(axs, rdwx_nc_object, tmt_nc_object, snow_accum_plot_bool)
    axs = make_wind_graph(axs, rdwx_nc_object, snow_accum_plot_bool)
    axs = make_rec_treatments_bar(axs, time_tmts, time_alerts, tmt_nc_object, snow_accum_plot_bool)
    print "made plots"
    
    # fix missing major tick issue caused by autofmt_axdate
    fig.autofmt_xdate(rotation=0, ha='left') # horizontal rotation and left alignment of date
    fig.subplots_adjust(hspace=1, bottom=0.1) # add padding to make x axis visible

    # Set up axes labels and legends
    if not rdwx_nc_object.rdwx_error:
        for ax in axs:
            ax.xaxis.set_major_locator(DayLocator())
            ax.xaxis.set_minor_locator(HourLocator(arange(0,24,6), tz=state_timezone)) # Label every 6 hours

            ax.xaxis.set_major_formatter(DateFormatter("%Y%m%d")) # Only display date on major x ticks
            ax.xaxis.set_minor_formatter(DateFormatter("%H", tz=state_timezone))
            ax.fmt_xdata = DateFormatter('%Y-%m-%d %H:%M:%S')
            ax.legend(loc='upper left',bbox_to_anchor=(1,1.06),prop={'size':9}) # Move legend to outside of plot
            box = ax.get_position() # Grab plot sizes
            ax.set_position([box.x0, box.y0, box.width * 0.87, box.height]) # Shrink width to make legend room
            for tick in ax.xaxis.get_minor_ticks():
                tick.label.set_fontsize(8)
            for tick in ax.xaxis.get_major_ticks():
                tick.set_pad(-10) # Set date label lower on axis
                tick.label.set_fontsize(10)

    plt.tick_params(axis='x', which='major', reset=True, labelsize=10, pad=15)
    plt.xticks(ha='left')
    plt.tick_params(axis='x', which='minor', labelsize=8)

    # add all plots to the figure
    for ax in axs:
        fig.add_subplot(ax)

    if outfile != "":
        fig.savefig(outfile)
    else:
        f = StringIO.StringIO()
        fig_canvas = FigureCanvas(fig)
        fig_canvas.print_figure(f)
        return base64.b64encode(f.getvalue())
    
def make_alert_bar(ax_list, time_alerts, rdwx_nc_object, all_sites, site_num):
    """ Creates the alert bar plot and adds it to a list of axes/plots

    Parameters
    ----------
    ax_list : list
        A list of axes/plots
    time_alerts : list
        A list of time alerts
    rdwx_nc_object : NetcdfFileData object
        An object with information from a road weather nc file

    Returns
    -------
    ax_list
    """
    
    alert_bar_ax = plt.subplot2grid((14,1),(0,0))
    array_len = 72
    if len(time_alerts) < array_len:
        array_len = len(time_alerts)
        
    # Create a height array with entries for each hour so that each alert bar segment is of a uniform height that fills the alert bar fully

    if rdwx_nc_object.rdwx_error:
        height_array = []
    else:
        height_array = []
        for i in range(array_len):
            height_array.append(100)

    print "length time alerts, height array, dates: %d %d %d" % (len(time_alerts), len(height_array), len(rdwx_nc_object.dates))
    dates = rdwx_nc_object.dates[0:array_len]
    print "dates: ", dates
    alert_bar_chart = alert_bar_ax.bar(dates, height_array, linewidth = 0)

    # Set each alert bar segment to the correct alert color.
    # Time alerts len is 72, and is a number between 0-3 to represent warn, alert, clear, and missing.
    # Iterate through all values of time alerts, and grab the corresponding color in RGBA (i.e. Warn is Yellow, (255,255,0,1))
    # Divide all of these colors by 255 and store into an RGB array for matplotlib to use when plotting the bar. Each bar represents 1 hour out of 72.

    if len(time_alerts) != 0:
        for ibar in range(len(alert_bar_chart)):
            bar_color = alert_mod.alert_to_color[time_alerts[ibar]]
            alert_bar_color = [bar_color[0]/255., bar_color[1]/255., bar_color[2]/255.]
            alert_bar_chart[ibar].set_color(alert_bar_color)
        alert_bar_ax.set_title(str(all_sites[site_num]['desc']), fontsize=12, va='bottom') # add site ID to plot and set fontsize
    alert_bar_ax.set_yticks([])
    alert_bar_ax.set_ylabel("Alert\n Summary", fontsize=8, fontweight='bold', rotation=0)
    if not rdwx_nc_object.rdwx_error:
        alert_bar_ax.set_xlim([dates[0], dates[len(dates)-1]])
    ax_list.append(alert_bar_ax)
    
    return ax_list

def make_snow_accum_plot(ax_list, rdwx_nc_object):
    """ Creates a snow accum plot (72hr time frame) and adds it to a list of axes/plots

    Parameters
    ----------
    ax_list : list
        A list of axes/plots
    nc_object : NetcdfFileData object
        An object with information from a treatment nc file

    Returns
    -------
    ax_list
    snow_accum_plot_bool
    """

    snow_accum_plot_bool = True
    if rdwx_nc_object.rdwx_site_index == ([],):
        snow_accum_plot_bool = False
        return ax_list, snow_accum_plot_bool
    
    snow_accum_ax = plt.subplot2grid((14,1), (1,0), rowspan=3) # Change subplot layout for alert bar
    snow_accum_ax.grid(True)
    snow_accum = rdwx_nc_object.snow_accum
    snow_accum_ax.tick_params(axis='y',which='major',labelsize=8) # Set y tick font size
    snow_accum_ax.set_ylabel("Snow (in)",fontsize=10)
    snow_accum_ax.plot_date(rdwx_nc_object.dates, snow_accum[0:len(rdwx_nc_object.dates)], "-", label="Snow Accum")
    #snow_accum_ax.bar(rdwx_nc_object.dates, snow_accum[0:len(rdwx_nc_object.dates)], width=0.04, label="Snow Accum", color='b')
    if not rdwx_nc_object.rdwx_error:
        snow_accum_ax.set_xlim([rdwx_nc_object.dates[0], rdwx_nc_object.dates[len(rdwx_nc_object.dates)-1]]) # force axis to encompass all times even if first values are missing
    ax_list.append(snow_accum_ax)

    return ax_list, snow_accum_plot_bool

def make_precip_graph(ax_list, rdwx_nc_object, snow_accum_plot_bool):
    """ Creates precipitation (rain, snow, ice) plot (72hrs) and adds to list of axes/plots

    Parameters
    ----------
    ax_list : list
        A list of axes/plots
    rdwx_nc_object : NetcdfFileData object
        An object with information from road weather and treatment nc files
    snow_accum_plot_bool : bool
        A test condition to see if road temp plot exists

    Returns
    -------
    ax_list
    """

    # Check to see if road temp plot exists; if not, shift plots up
    if snow_accum_plot_bool:
        precip_ax = plt.subplot2grid((14,1), (4,0), rowspan=3) # Change subplot layout for alert bar
    else:
        precip_ax = plt.subplot2grid((14,1), (1,0), rowspan=3) # Change subplot layout for alert bar
        
    precip_ax.tick_params(axis='y', which='major', labelsize=8) # Set y tick font size
    precip_ax.set_ylim(0,1)
    precip_ax.grid(True)
    precip_ax.set_ylabel("Prob Precip", fontsize=10) 
    """
    precip_accum_snow = rdwx_nc_object.precip_accum_snow
    precip_accum_rain = rdwx_nc_object.precip_accum_rain
    precip_accum_ice = rdwx_nc_object.precip_accum_ice
    precip_ax.bar(rdwx_nc_object.dates, precip_accum_rain[0:len(rdwx_nc_object.dates)], width=0.04, label='Rain', color='g') 
    precip_ax.bar(rdwx_nc_object.dates, precip_accum_snow[0:len(rdwx_nc_object.dates)], width=0.04, label='Snow', color='b',bottom=precip_accum_rain[0:len(rdwx_nc_object.dates)]) 
    precip_ax.bar(rdwx_nc_object.dates, precip_accum_ice[0:len(rdwx_nc_object.dates)], width=0.04, label='Ice', color='r',bottom=precip_accum_snow[0:len(rdwx_nc_object.dates)]+precip_accum_rain[0:len(rdwx_nc_object.dates)])
    """

    prob_rain = rdwx_nc_object.prob_rain
    prob_ice = rdwx_nc_object.prob_ice
    prob_snow = rdwx_nc_object.prob_snow
    precip_ax.bar(rdwx_nc_object.dates, prob_rain[0:len(rdwx_nc_object.dates)], width=0.04, label='Rain', color='g') 
    precip_ax.bar(rdwx_nc_object.dates, prob_snow[0:len(rdwx_nc_object.dates)], width=0.04, label='Snow', color='b',bottom=prob_rain[0:len(rdwx_nc_object.dates)]) 
    precip_ax.bar(rdwx_nc_object.dates, prob_ice[0:len(rdwx_nc_object.dates)], width=0.04, label='Ice', color='r',bottom=prob_rain[0:len(rdwx_nc_object.dates)]+prob_snow[0:len(rdwx_nc_object.dates)])

    precip_ax.xaxis_date()
    
    if not rdwx_nc_object.rdwx_error:
        precip_ax.set_xlim([rdwx_nc_object.dates[0], rdwx_nc_object.dates[len(rdwx_nc_object.dates)-1]]) # force axis to encompass all times even if first values are missing
    ax_list.append(precip_ax)

    return ax_list

def make_temp_dewp_graph(ax_list, rdwx_nc_object, tmt_nc_object, snow_accum_plot_bool):
    """ Creates temperature and dewpoint graph (72hrs) and adds to list of axes/plots

    Parameters
    ----------
    ax_list : list
        A list of axes/plots
    rdwx_nc_object : NetcdfFileData object
        An object with information from a road weather file
    tmt_nc_object : NetcdfFileData object
        An object with information from a treatment file
    snow_accum_plot_bool : bool
        A test condition to see if road temp plot exists

    Returns
    -------
    ax_list
    """

    # Check to see if road temp plot exists; if not, shift plots up
    if snow_accum_plot_bool:
        temperature_ax = plt.subplot2grid((14,1), (7,0), rowspan=3) # Change subplot layout for alert bar
    else:
        temperature_ax = plt.subplot2grid((14,1), (4,0), rowspan=3) # Change subplot layout for alert bar
    temperature_ax.grid(True)
    
    temps = rdwx_nc_object.temps
    dewps = rdwx_nc_object.dewps
    road_temps = tmt_nc_object.road_temps
    temp_units = rdwx_nc_object.temp_units
    dewp_units = rdwx_nc_object.dewp_units         

    temperature_ax.tick_params(axis='y', which='major', labelsize=8) #Set y tick font size
    temperature_ax.set_ylabel(temp_units, fontsize=10)
    temperature_ax.plot_date(tmt_nc_object.dates, road_temps[0:len(tmt_nc_object.dates)], "-", color='b', label="Road Temp") # Only plot first 72 hrs
    temperature_ax.plot_date(rdwx_nc_object.dates, temps[0:len(rdwx_nc_object.dates)], "-", color='r', label="Air Temp") # Only plot first 72 hrs
    temperature_ax.plot_date(rdwx_nc_object.dates, dewps[0:len(rdwx_nc_object.dates)], "-", color='g', label="Dew Point") # Only plot first 72 hrs
    if not rdwx_nc_object.rdwx_error:
        temperature_ax.set_xlim([rdwx_nc_object.dates[0], rdwx_nc_object.dates[len(rdwx_nc_object.dates)-1]]) # force axis to encompass all times even if first values are missing
    ax_list.append(temperature_ax)

    return ax_list

def make_wind_graph(ax_list, rdwx_nc_object, snow_accum_plot_bool):
    """ Creates a wind speed/direction plot (72hrs) and adds it to a list of axes/plots

    Parameters
    ----------
    ax_list : list
        A list of axes/plots
    rdwx_nc_object : NetcdfFileData object
        An object with information from road weather and treatment nc files
    snow_accum_plot_bool : bool
        A test condition to see if road temp plot exists

    Returns
    -------
    ax_list
    """

    # Check to see if road temp plot exists; if not, shift plots up
    if snow_accum_plot_bool:
        wind_ax = plt.subplot2grid((14,1), (10,0), rowspan=3) # Change subplot layout for alert bar
    else:
        wind_ax = plt.subplot2grid((14,1), (7,0), rowspan=3) # Change subplot layout for alert bar
    wind_ax.grid(True)

    wind_spd_units = rdwx_nc_object.wind_spd_units
    wind_spd = rdwx_nc_object.wind_spd
    u = rdwx_nc_object.wind_u_dir
    v = rdwx_nc_object.wind_v_dir

    wind_ax.set_ylabel(wind_spd_units, fontsize=10) 
    wind_ax.tick_params(axis='y', which='major', labelsize=8)
    wind_ax.plot_date(rdwx_nc_object.dates, wind_spd[0:len(rdwx_nc_object.dates)], "-", label="Wind Speed") # Only plot first 72 hrs
    if not rdwx_nc_object.rdwx_error:
        wind_ax.set_xlim([rdwx_nc_object.dates[0], rdwx_nc_object.dates[len(rdwx_nc_object.dates)-1]]) # force axis to encompass all times even if first values are missing

    # include wind barbs along top of wind speed trace
    tmp = [0]*len(rdwx_nc_object.dates)                                     
    wind_ax.plot_date(rdwx_nc_object.dates, tmp, '-', color='k', label="Wind Dir") # get wind_dir in legend 
    barb_loc = [0.8*wind_ax.get_ylim()[1]]*len(rdwx_nc_object.dates)
    wind_ax.barbs(rdwx_nc_object.dates, barb_loc, u[0:len(rdwx_nc_object.dates)], v[0:len(rdwx_nc_object.dates)], length=5, label="Wind Dir")
    ax_list.append(wind_ax)

    return ax_list

def make_rec_treatments_bar(ax_list, time_tmts, time_alerts, tmt_nc_object, snow_accum_plot_bool):
    """ Creates a recommended treatments bar and adds to list of axes/plots

    Parameters
    ----------
    ax_list : list
        A list of axes/plots
    time_tmts : list
        A list of recommended treatments (integers representing warn, alert, clear, missing)
    time_alerts : list
        A list of time alerts
    tmt_nc_object : NetcdfFileData object
        An object with information from the treatment nc file
    snow_accum_plot_bool : bool
        A test condition to see if road temp plot exists

    Returns
    -------
    ax_list
    """
    
    # Check if data was read from nc files; if not, make height array empty to plot a blank plot.
    if tmt_nc_object.tmt_error or tmt_nc_object.tmt_site_index == ([],):
        return ax_list

    # Check to see if road temp plot exists; if not, shift plots up
    if snow_accum_plot_bool:
        rec_tmt_ax = plt.subplot2grid((14,1), (13,0)) # Change subplot layout for alert bar
    else:
        rec_tmt_ax = plt.subplot2grid((14,1), (10,0), rowspan=3) # Change subplot layout for alert bar

    rec_tmt_ax.grid(True)
    rec_tmt_ax.set_ylabel('Recommended\n Treatments', fontsize=8, rotation=0)
    rec_tmt_ax.set_yticks([])

    # Check if data was read from nc files; if not, make height array empty to plot a blank plot.
    height_array = []
    alert_len = 72
    if len(time_alerts) < alert_len:
        alert_len = len(time_alerts)
    for i in range(alert_len):
        height_array.append(100)

    dates = tmt_nc_object.dates[0:alert_len]
    bar_chart = rec_tmt_ax.bar(dates, height_array, linewidth = 0)

    if time_tmts != []:
        for ibar in range(len(bar_chart)):
            bar_color = alert_mod.alert_to_color[time_tmts[ibar]]
            tmt_bar_color = [bar_color[0]/255., bar_color[1]/255., bar_color[2]/255.]
            bar_chart[ibar].set_color(tmt_bar_color)

    rec_tmt_ax.set_xlim([dates[0], dates[len(dates)-1]]) # force axis to encompass all times even if first values are missing
    ax_list.append(rec_tmt_ax)

    return ax_list

def get_treatment_plot(cf, fpath, site):
    num_plots = 5
    rdwx_nc = Dataset(fpath,"r")
    sites = rdwx_nc.variables["site_list"][:]
    i = numpy.where(sites == site)
    nan = float('nan')
    init_time = rdwx_nc.variables["forc_time"][:]
    fig = Figure(figsize=(7,8),dpi=150)
    app_rate_var = rdwx_nc.variables["application_rate"]
    app_rates = ma.filled(app_rate_var[i],nan).flatten()
    chem_types = ma.filled(rdwx_nc.variables["chem_type"][i],nan).flatten()
    precip_types = ma.filled(rdwx_nc.variables["precip_type"][i],nan).flatten()
    #precip_types = map(lambda x: nan if math.isnan(x) else int(nan), precip_types)
    phase_types = ma.filled(rdwx_nc.variables["phase_type"][i],nan).flatten()
    #phase_types = map(lambda x: nan if math.isnan(x) else int(nan), phase_types)
    road_temp_var = rdwx_nc.variables[cf.met_vars.tmt_road_temp_var]
    road_temps = ma.filled(road_temp_var[i],nan).flatten()
    chem_type_labels = ["None","NACL","CACL2","MGCL2","CMA","KAC","Caliber","IceSlicer","IceBan"]
    precip_type_labels = ["None","Rain","Snow","Mixed Rain/Snow","Mixed Snow/Rain","Freezing Rain"]
    phase_type_labels = ["Dry","Wet","Chemically Wet","Chemically Ice","Slush","Snow","Ice"]
    begin_time = datetime.datetime.fromtimestamp(init_time)
    end_time = datetime.datetime.fromtimestamp(init_time + (len(road_temps)*3600))
    delta = datetime.timedelta(hours=1)
    dates = drange(begin_time, end_time, delta)
    axs = []
    ax = fig.add_subplot(num_plots,1,1)
    ax.set_ylabel(app_rate_var.units)
    ax.plot_date(dates,app_rates,"-")
    ax.set_ylim(bottom=0)
    axs.append(ax)
    ax = fig.add_subplot(num_plots,1,2)
    ax.set_ylabel("chem type")
    ax.plot_date(dates,chem_types,"-")
    ax.set_ylim(bottom=0)
    axs.append(ax)
    ax.yaxis.set_ticklabels(chem_type_labels)
    ax = fig.add_subplot(num_plots,1,3)
    ax.set_ylabel("precip type")
    ax.plot_date(dates,precip_types,"-")
    ax.yaxis.set_ticklabels(precip_type_labels)
    ax.set_ylim(bottom=0)
    axs.append(ax)
    ax = fig.add_subplot(num_plots,1,4)
    ax.set_ylabel("phase type")
    ax.plot_date(dates,phase_types,"-")
    ax.set_ylim(bottom=0)
    ax.yaxis.set_ticklabels(phase_type_labels)
    axs.append(ax)
    ax = fig.add_subplot(num_plots,1,5)
    ax.set_ylabel(road_temp_var.units)
    ax.plot_date(dates,road_temps,"-",label="road temp")
    axs.append(ax)
    for ax in axs:
        ax.grid(True)
        ax.xaxis.set_major_locator(DayLocator())
        ax.xaxis.set_minor_locator(HourLocator([6,12,18]))
        ax.xaxis.set_major_formatter(DateFormatter("%Y%m%d %H:%M"))
        ax.xaxis.set_minor_formatter(DateFormatter("%H"))
        ax.fmt_xdata = DateFormatter('%Y-%m-%d %H:%M:%S')
        for tick in ax.xaxis.get_minor_ticks():
            tick.label.set_fontsize(8)
    fig.autofmt_xdate()
    f = StringIO.StringIO()
    can = FigureCanvas(fig)
    can.print_figure(f)
    can.print_figure("/home/dicast/testme_treatment.png")
    tmt_plot = base64.b64encode(f.getvalue())

    recs = []
    tmt_expls = rdwx_nc.variables["treatment_explanation"][i][0]
    (days,fc_times_per_day,max_str_len) = tmt_expls.shape
    for d in range(days):
        for h in range(fc_times_per_day):
            expl = ma.filled(tmt_expls[d][h])
            expl = string.join(expl,"")
            vtime = init_time + (d*86400) + (h*3600)
            time_tup = time.gmtime(vtime)
            date_str = time.strftime("%Y-%m-%d %H:%M:%S", time_tup)
            recs.append({"date_str":date_str, "explanation":expl})
    rdwx_nc.close()
    return (tmt_plot,recs)

def get_wx_obs(cf, site, ptime):
    alerts = alert_mod.ObsAlerts(cf, ptime)
    return alerts.get_obs(site)

def get_rwis(cf, site, ptime):
    alerts = alert_mod.RwisAlerts(cf, ptime)
    return alerts.get_obs(site)

def main():

     cf = backend_sys_path.State_dictionary["alaska"]
     get_summary_plot(cf, "/home/dicast/mdss_view/rdwx_fcst.20140507.1315.nc", "/home/dicast/mdss_view/merge_rec_tmt.20140507.1315.nc", 70261000, 13, pytz.timezone("US/Alaska"), "/scratch/new.testme.png")
    
if "__main__" == __name__:
#    get_summary_plot("/d1/dicast/rctm/rdwx_fcst/20121128/rdwx_fcst.20121128.1115.nc",70133008)
#    get_summary_plot("/d1/dicast/rctm/rdwx_fcst/20130327/rdwx_fcst.20130327.1515.nc","/d1/dicast/rctm/merge_rec_tmt/20130327/merge_rec_tmt.20130327.1515.nc",70341018)
# GW get Tom a set of netcdf files for michigan
     main()
     #cf = backend_sys_path.State_dictionary["alaska"]
     #get_summary_plot(cf, "/d2/dicast/rctm/rdwx_fcst/20130926/rdwx_fcst.20130926.0115.nc", "/d2/dicast/rctm/merge_suff_tmt/20130926/merge_suff_tmt.20130926.0119.nc", 72649007)
     #get_summary_plot(cf, "rdwx_fcst.20140507.1315.nc", "merge_rec_tmt.20140507.1315.nc", 70261000, 13, pytz.timezone("US/Alaska"), "new.testme.png")
     #get_summary_plot(cf, "/d2/dicast/rctm/rdwx_fcst/20130917/rdwx_fcst.20130917.1615.nc", "/d2/dicast/rctm/merge_suff_tmt/20130917/merge_suff_tmt.20130917.1619.nc", 72747030)
  #      get_summary_plot("alaska", "/d1/dicast/rctm/rdwx_fcst/20130516/rdwx_fcst.20130516.1915.nc",70174003)
#    rdwx_alert_images = rdwx_alerts.precip_types()
#    print rdwx_alert_images
    #get_treatment_plot("/d1/dicast/rctm/merge_rec_tmt/20130327/merge_rec_tmt.20130327.1515.nc",70341018)
    #plot = TreatmentPlot("/d2/lambi/dicast_data/merge_rec_tmt/20120531/merge_rec_tmt.20120531.2317.nc")
    #plot.save_treatment_plot(72469103,"testme.png")
    #plot.save_temp_plot(72469103,"testme.png")s
    #plot.save_precip_plot(72469103,"testme.png")
    #print plot.get_temp_plot_b64(72469103)
