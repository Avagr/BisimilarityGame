import os

from PyQt5 import QtGui
from PyQt5.QtWidgets import (QWidget, QHBoxLayout, QFormLayout, QApplication, QPushButton, QFileDialog)

from qtui.parsing.petri import read_net


class MainWindow(QWidget):

    def __init__(self):
        super().__init__()
        self.outer = QHBoxLayout()
        self.net = None
        self.net_path = None;
        self.init_ui()

    def init_ui(self):
        self.init_layout()
        self.setLayout(self.outer)
        rec = QApplication.desktop().screenGeometry()
        self.resize(rec.width() * 2 // 3, rec.height() * 2 // 3)
        self.move(300, 150)
        self.setWindowTitle('Bisimilarity Checker')
        self.setWindowIcon(QtGui.QIcon(os.path.dirname(os.path.realpath(__file__)) + os.path.sep + 'icon/icon.png'))
        self.show()

    def init_layout(self):
        control_layout = QFormLayout()
        import_button = QPushButton("Select file")
        import_button.clicked.connect(self.select_net)
        control_layout.addRow("Import a Petri Net: ", import_button)
        self.outer.addLayout(control_layout)

    def select_net(self):
        dlg = QFileDialog()
        dlg.resize(self.width() * 3 // 4, self.height() * 3 // 4)
        dlg.setFileMode(QFileDialog.ExistingFile)
        dlg.setAcceptMode(QFileDialog.AcceptOpen)
        dlg.setNameFilters(["PNML Files (*.pnml)"])
        dlg.selectNameFilter("PNML Files (*.pnml)")
        dlg.exec_()
        self.net_path = dlg.selectedFiles()[0]
        print(self.net_path)
        self.net = read_net(self.net_path)
