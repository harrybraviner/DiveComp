#! /usr/bin/python

"""
Dive simulation software.
Intended to pass depths over serial connection to Arduino Uno
running dive computer software.
"""

import argparse
import time
import math

import matplotlib.pyplot as plt
import numpy as np

# Decompression models
from dummyModel import dummyModel

# End of decompression models

# Start of main code

parser = argparse.ArgumentParser(
    description='dive simulation program')
parser.add_argument("--plot-profile", action="store_true")
parser.add_argument("--plot-update-time",
    help="time interval, in minutes, when we update the t-axis of the profile plot",
    dest="plot_update_time", type=float, default=5.0)
parser.add_argument("--plot-update-depth",
    help="depth interval, in meters, at which we update the depth-axis of the profile plot",
    dest="plot_update_depth", type=float, default=5.0)
parser.add_argument("--dummy-model",
    help="a dummy, single-compartment model, with a tissue half-time of 15 seconds. DO NOT USE FOR DIVE PLANNING OR EXECUTION - THIS MODEL HAS NO BASIS IN REALITY!!!",
    action="store_true")
args = parser.parse_args()
plot_profile=args.plot_profile
plot_update_time=args.plot_update_time
plot_update_depth=args.plot_update_depth

# Initialise interactive mode
quit = False
current_depth = target_depth = 0
max_depth = 0
descent_rate = 0
prev_time = now_time = dive_start_time = time.time()
dive_time = 0.0

# Build the list of active models
model_list = []
if args.dummy_model:
    model_list.append(dummyModel())

# FIXME - just here for debugging
target_depth = 5.0
descent_rate = 8.0/60.0

# Initialise the plot, if we've been asked for one
if plot_profile:
    t_plot = [0]
    depth_plot = [0]
    plt.ion()
    profilePlot = plt.figure()
    ax = profilePlot.add_subplot(111)
    # plot time in units of minutes
    profileLine, = ax.plot([t/60. for t in t_plot], depth_plot, 'r-')
    ax.set_xlim(0, plot_update_time)
    ax.set_ylim(plot_update_depth + 0.01, -0.01)
    profilePlot.canvas.draw()

#for model in model_list:
#    model("plot")

while(not quit):
    # Interactive mode
    # Allow the user to specify depths and rates of descent on a prompt
    
    # Continue moving the diver if the target_depth is not equal to the current_depth
    now_time = time.time()
    if(current_depth != target_depth):
        # Can we hit our depth within this time step?
        if(abs(current_depth - target_depth) <= abs(descent_rate)*(now_time - prev_time)):
            current_depth = target_depth
        # If not, update by correct amount, ignoring sign of descent_rate
        else:
            current_depth +=  abs(descent_rate)*(now_time - prev_time)*(target_depth - current_depth)/abs(target_depth - current_depth)
    #FIXME - output depth over serial to the Arduino Uno

    # Update time and depth variables
    prev_time = now_time
    dive_time = now_time - dive_start_time
    if current_depth > max_depth:
        max_depth = current_depth

    if(plot_profile):
        if(now_time - 0.25 >= t_plot[-1]): # Update the plot at quarter second intervals
            t_plot.append(dive_time)
            depth_plot.append(current_depth)
            profileLine.set_xdata([t/60. for t in t_plot])
            profileLine.set_ydata(depth_plot)
            ax.set_xlim(0, plot_update_time*math.ceil((t_plot[-1]/60.)/plot_update_time))
            ax.set_ylim(plot_update_depth*math.ceil(max(max_depth/plot_update_depth, 1)) + 0.01, -0.01)
        profilePlot.canvas.draw() # Redraw the plot

    # If we're running any models, update them and then plot them
    for model in model_list:
        model.update()
        model.update_plot()

