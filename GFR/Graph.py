import itertools
import pyqtgraph
import time

class Graph:

    def __init__(self):
        self.graph = pyqtgraph.PlotWidget()
        self.timeStamp = "00:00:00"
        self.x = []
        self.y = []
        self.renderGraph()

    def updateData(self, newX, newY):
        self.x.append(newX)
        self.y.append(newY)

    def updateTimeStamp(self, newTime):
        self.timeStamp = newTime

    def renderGraph(self):
        self.graph.plot(self.x, self.y)




