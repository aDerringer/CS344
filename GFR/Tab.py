import itertools
import pyqtgraph
import time
from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QGroupBox, QSlider, QLabel, QVBoxLayout, QHBoxLayout, \
    QPushButton, QFormLayout, QGridLayout, QTabWidget


class Tab:

    id_iter = itertools.count()

    def __init__(self, measure):
        self.id = next(Tab.id_iter)
        self.title = measure
        self.tab = QWidget()

        self.graph = pyqtgraph.PlotWidget()
        self.timeStamp = "00:00:00"
        self.x = []
        self.y = []

        layout = QVBoxLayout()
        layout.addWidget(self.graph)
        self.graph.plot(self.x, self.y)
        self.tab.setLayout(layout)

    def updateData(self, newX, newY):
        self.x.extend(newX)
        self.y.extend(newY)
        self.graph.plot(self.x, self.y)

    def updateTimeStamp(self, newTime):
        self.timeStamp = newTime


