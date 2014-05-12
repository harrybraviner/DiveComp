import matplotlib.pyplot as plt
import numpy as np

# WARNING - the 'dummy_model' is NOT viable decompression model. NEVER use it for diving. EVER!
class dummyModel:
    """An example decompression model with wildly unrealistic parameters and only a single tissue compartment."""

    def __init__(self):
        # Initialise the plot
	plt.ion()
        self.modelPlot = plt.figure()
        self.modelAx = self.modelPlot.add_subplot(111)
        self.modelLine, = self.modelAx.plot([1,2,3], [1,1,2], 'r-')
        self.modelPlot.canvas.draw()

        # FIXME - initialise the model itself
        return

    def update_plot(self):
        self.modelPlot.canvas.draw()
        return

    def update(self):
        # FIXME - write this function
	return
