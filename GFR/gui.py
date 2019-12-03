from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QGroupBox, QSlider, QLabel, QVBoxLayout, QHBoxLayout, \
    QPushButton, QFormLayout, QGridLayout, QTabWidget, QMessageBox, QInputDialog, QLineEdit, QLabel
from PyQt5.QtCore import Qt, QSize
from PyQt5.QtGui import QPainter, QBrush, QPen, QFont, QColor
from random import random
import numpy as np
from numpy import sin, cos, pi
from enum import Enum
import sys
import pyqtgraph
from pyqtgraph import PlotWidget, plot
from Tab import Tab


class popupWindow(QMainWindow):

    main = None

    def __init__(self, parent=None):
        super(popupWindow, self).__init__(parent)

        self.setWindowTitle("Add New Tab")
        centralWidget = QWidget()
        self.setCentralWidget(centralWidget)

        self.name = QLineEdit()
        self.type = QLineEdit()

        tabForm = QFormLayout()
        tabForm.addRow(QLabel("Name:"), self.name)
        tabForm.addRow(QLabel("Type:"), self.type)

        submit = QPushButton("Submit")
        submit.clicked.connect(self.generate_tab)

        cancel = QPushButton("Cancel")
        cancel.clicked.connect(self.close)

        tabForm.addRow(submit, cancel)
        centralWidget.setLayout(tabForm)

    def generate_tab(self):
        newTab = Tab(self.name.text())
        self.main.tabList.append(newTab)
        print(newTab.title)
        self.main.add_tab(newTab)
        self.close()



class main(QMainWindow):
    def __init__(self, parent=None):
        super(main, self).__init__(parent)

        # Apply title and staring size and location
        # for window on startup
        self.setWindowTitle("GFR Live Data GUI")
        self.setGeometry(0, 0, 1250, 750)

        # centralWidget houses layout features
        # and smaller widgets within the app
        centralWidget = QWidget(self)
        self.setCentralWidget(centralWidget)

        ################################################################
        # Static tool bar
        ################################################################

        # Tool bar setup
        self.statusBar()
        mainMenu = self.menuBar()

        # File drop down
        fileMenu = mainMenu.addMenu("&File")

        # File drop down - exit
        extractAction = QtGui.QAction("&Exit", self)
        extractAction.setShortcut("Ctrl+Q")
        extractAction.setStatusTip("&Leave the App")
        extractAction.triggered.connect(self.close_application)
        fileMenu.addAction(extractAction)  # add to file drop down

        # View drop down
        viewMenu = mainMenu.addMenu("&View")

        # View drop down - add tab
        addTab = QtGui.QAction("&Add Tab", self)
        addTab.setShortcut("Ctrl+T")
        addTab.setStatusTip("&Add tab with custom graph display")
        addTab.triggered.connect(self.tab_window)
        viewMenu.addAction(addTab) # add to view drop down

        ################################################################
        # Central Widget Layout
        ################################################################

        # Create horizontal format for subsections
        horizontalSections = QHBoxLayout()

        # Left subsection - not assigned
        programDescription = QLabel("This tells you what the program does which is currently precisely nothing...")
        horizontalSections.addWidget(programDescription)

        # Right subsection - Tabbed graphic data
        self.tabList = []
        self.tabs = QTabWidget()
        horizontalSections.addWidget(self.tabs)

        # Add horizontal subsections to central widget
        centralWidget.setLayout(horizontalSections)

        # holds instance up add tab popup so that it data
        # is not cleaned up immediately upon close
        self.popup = None

    def tab_window(self):
        self.popup = popupWindow()
        self.popup.main = self
        self.popup.show()

    def add_tab(self, item):
        #item.updateData(self.hour, self.temperature)
        self.tabs.addTab(item.tab, item.title)

    def close_application(self):
        print("Good-Bye!")
        sys.exit()


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    mainWin = main()
    mainWin.show()
    sys.exit( app.exec_() )
