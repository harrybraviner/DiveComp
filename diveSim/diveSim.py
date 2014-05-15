#! /usr/bin/python

"""
Dive simulation software.
Intended to pass depths over serial connection to Arduino Uno
running dive computer software.
"""

import argparse
import time
import math
import serial

import curses

import matplotlib.pyplot as plt
import numpy as np

# Decompression models
from dummyModel import dummyModel

# End of decompression models

# Start of main code

##### Start of argument parser #####
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
parser.add_argument("--serial-comp",
    help="serial device on which an unoDiveComp (or compatible) is attached",
    dest="serial_device", type=str)
args = parser.parse_args()
plot_profile=args.plot_profile
plot_update_time=args.plot_update_time
plot_update_depth=args.plot_update_depth
serial_device=args.serial_device
##### End of argument parser #####

# Initialise interactive mode parameters
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

# Setup the serial connection to the Arduino
if serial_device != None:
    try:
        serComp = serial.Serial(serial_device, 9600)
        time.sleep(2)
	# Tell the computer what the time is
	ctt = time.gmtime(time.time())
	serComp.write(b"T {0:d} {1:d} {2:d} {3:d} {4:d} {5:d}".format(ctt.tm_hour, ctt.tm_min, ctt.tm_sec, ctt.tm_mday, ctt.tm_mon, ctt.tm_year))
    except:
        print "Problem opening serial connection to device ", serial_device
	serial_device = None
        time.sleep(1)

##### Start of curses interface setup #####
stdscr = curses.initscr()
curses.noecho()
curses.cbreak()
stdscr.keypad(1)

windowWidth = 90

# Data window
numDataLines = 4
dataWin = curses.newwin(numDataLines, windowWidth, 0, 0)
dataWin.addstr(0, 0, "Dive time:\t{0:02d}:{1:02d}:{2:02d}".format(int(dive_time/(60*60)), int((dive_time/60)%60), int(dive_time%60)))
dataWin.addstr(1, 0, "Current depth:\t{0:03d}.{1:1d}m".format(int(current_depth), int(round((current_depth*10)%10))))
dataWin.addstr(2, 0, "Target depth:\t{0:03d}.{1:1d}m".format(int(target_depth), int(round((target_depth*10)%10))))
dataWin.addstr(3, 0, "Rate of descent / ascent:\t{0:03d}.{1:1d}m/min".format(int(descent_rate*60.), int(round((descent_rate*60.*10)%10))))
dataWin.refresh()

# Input window
inputString = ''
inputWin = curses.newwin(1, windowWidth, numDataLines, 0)
inputWin.addstr(0, 0, ">> " + inputString)
inputWin.refresh()
inputWin.nodelay(1)

# Help window
numHelpLines = 3
helpWin = curses.newwin(numHelpLines, windowWidth, numDataLines + 1, 0)
helpWin.addstr(0, 0, "quit\t- Exit the program")
helpWin.addstr(1, 0, "set depth [depth / m]\t- Set the target depth in meters")
helpWin.addstr(2, 0, "set rate [rate / (m / min)]\t- Set the descent / ascent rate in meters per minute")
helpWin.refresh()

inputWin.move(0, 3) # Finally, place the cursor!
##### End of curses interface setup #####

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
    # Output depth over serial to the unoDiveComp
    if serial_device != None:
        serComp.write(b"D {0:d}".format(int(round(current_depth*10))))

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
    
    ##### Start of curses interface update #####
    # Update the displayed data
    dataWin.erase()
    dataWin.addstr(0, 0, "Dive time:\t{0:02d}:{1:02d}:{2:02d}".format(int(dive_time/(60*60)), int((dive_time/60)%60), int(dive_time%60)))
    dataWin.addstr(1, 0, "Current depth:\t{0:03d}.{1:01d}m".format(int(round(current_depth*10)/10), int(round((current_depth*10))%10)))
    dataWin.addstr(2, 0, "Target depth:\t{0:03d}.{1:1d}m".format(int(target_depth), int(round((target_depth*10)%10))))
    dataWin.addstr(3, 0, "Rate of descent / ascent:\t{0:03d}.{1:1d}m/min".format(int(descent_rate*60.), int(round((descent_rate*60.*10)%10))))
    dataWin.refresh()
    inputWin.move(0, len(">> " + inputString)) # Put the cursor back in the right place
    inputWin.refresh()
    time.sleep(0.01)

    # Get user input (if any)
    inch = inputWin.getch()
    if inch ==8 or inch == 127:	# Backspace or Delete?
        inputString = inputString[:-1]
	inputWin.deleteln()
	inputWin.move(0, 0)
	inputWin.addstr(0, 0, ">> " + inputString)
	inputWin.refresh()
    elif inch != -1: # Anything else?
        try:
	    inch = str(chr(inch))
	except:
	    pass
	else:
	    if inch == '\n':
	        inputWin.deleteln()
		inputWin.move(0, 0)
		inputWin.addstr(0, 0, ">> ")
		inputWin.refresh()
		# Did the user ask to quit?
		if inputString.strip().lower() == 'quit':
		    quit = True
		# Did the user ask to set the target depth?
		elif len(inputString.strip().lower().split()) >= 3 and inputString.strip().lower().split()[0:2] == ['set', 'depth']:
		    try:
		        target_depth = float(inputString.strip().lower().split()[2])
		    except:
		        pass
		# Did the user ask to set the descent rate?
		elif len(inputString.strip().lower().split()) >= 3 and inputString.strip().lower().split()[0:2] == ['set', 'rate']:
		    try:
		        descent_rate = abs(float(inputString.strip().lower().split()[2])/60.0)
		    except:
		        pass
		inputString = ''
	    else:
	        inputString += inch
	        inputWin.deleteln()
		inputWin.move(0, 0)
		inputWin.addstr(0, 0, ">> " + inputString)
		inputWin.refresh()
    elif (inch == -1):
        inputWin.move(0, len(">> " + inputString)) # Put the cursor back in the right place
    ##### End of curses interface update #####

##### Start of curses cleanup #####
curses.nocbreak()
stdscr.keypad(0)
curses.echo()
curses.endwin()
##### End of curses cleanup #####
