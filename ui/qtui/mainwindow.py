import os
import sys
from typing import List

from PyQt5 import QtGui
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QFont
from PyQt5.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, QApplication, QPushButton, QFileDialog,
                             QMessageBox, QTableWidget)

from qtui.parsing.petri import read_net
from qtui.widgets import show_message, create_label

sep: str = os.path.sep


def error_handling(_, exc_value, __):
    show_message(QMessageBox.Critical, "An error has occurred, the program will now exit", str(exc_value))
    exit(1)


class MainWindow(QWidget):

    @property
    def net_loaded(self):
        return self._net_loaded

    @net_loaded.setter
    def net_loaded(self, value: bool):
        for el in self.togglable_elements:
            el.setEnabled(value)
        if value:
            self.labels = sorted(self.net['place_id'].items(), key=lambda x: x[1])
            for table in (self.first_table, self.second_table):
                table.setColumnCount(len(self.labels))
                table.setHorizontalHeaderLabels([tup[0] for tup in self.labels])
        self._net_loaded = value

    def __init__(self, path: str):
        super().__init__()
        self.base_path = path
        self.outer = QHBoxLayout()
        self.net: dict[str, dict] = {}
        self.place_num = 0
        self.first_state = []
        self.second_state = []
        self.net_path: str = ""
        self._net_loaded = False
        self.togglable_elements: List[QWidget] = []
        sys.excepthook = error_handling
        self.init_ui()

    def init_ui(self):
        """
        Initializes UI
        """
        QtGui.QFontDatabase.addApplicationFont(self.base_path + sep + "Poppins" + sep + "Poppins-Regular.ttf")
        self.setFont(QFont("Poppins-Regular", 13))
        self.init_layout()
        self.setLayout(self.outer)
        rec = QApplication.desktop().screenGeometry()
        self.resize(rec.width() * 2 // 3, rec.height() * 2 // 3)
        self.move(300, 150)
        self.setWindowTitle('Bisimilarity Checker')
        self.setWindowIcon(QtGui.QIcon(self.base_path + sep + 'icon/icon.png'))
        self.net_loaded = False
        self.show()

    def init_layout(self):
        """
        Initializes the layout of the main window
        """
        left_layout = QVBoxLayout()
        left_layout.setContentsMargins(10, 10, 10, 10)
        left_layout.addWidget(create_label("Bisimilarity checker", font_size=20))
        # Importing layout
        import_button = QPushButton("Import a Petri net")
        import_button.clicked.connect(self.select_net)
        left_layout.addWidget(import_button)
        # State layouts
        left_layout.addWidget(create_label("First state:", alignment=Qt.AlignCenter))
        self.first_table = QTableWidget(1, 0)
        self.first_table.setMaximumHeight(self.first_table.rowHeight(0) + 47)
        self.first_table.verticalHeader().hide()
        left_layout.addWidget(self.first_table)
        left_layout.addWidget(create_label("Second state:", alignment=Qt.AlignCenter))
        self.second_table = QTableWidget(1, 0)
        self.second_table.setMaximumHeight(self.second_table.rowHeight(0) + 47)
        self.second_table.verticalHeader().hide()
        left_layout.addWidget(self.second_table)
        self.togglable_elements.extend([self.first_table, self.second_table])
        # Import state layout

        # Tree viewer layout
        right_layout = QVBoxLayout()
        right_layout.addWidget(create_label("Tree viewer: ", font_size=15))

        self.outer.addLayout(left_layout)
        self.outer.addLayout(right_layout)

    def select_net(self):
        """
        Method for handling net selection
        """
        dlg = QFileDialog()
        dlg.resize(self.width() * 3 // 4, self.height() * 3 // 4)
        dlg.setFileMode(QFileDialog.ExistingFile)
        dlg.setAcceptMode(QFileDialog.AcceptOpen)
        dlg.setNameFilters(["PNML Files (*.pnml)"])
        dlg.selectNameFilter("PNML Files (*.pnml)")
        dlg.setDirectory(self.base_path[:-2])
        dlg.exec_()
        self.net_path = dlg.selectedFiles()[0] if len(dlg.selectedFiles()) > 0 else ""
        if self.net_path == "":
            return
        try:
            self.net = read_net(self.net_path)
        except KeyError as err:
            show_message(QMessageBox.Warning, "There was an error while reading the Petri net:", str(err))
        self.net_loaded = True
