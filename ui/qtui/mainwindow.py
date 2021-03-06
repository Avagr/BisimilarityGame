import os
import sys
from typing import List

from PyQt5 import QtGui
from PyQt5.QtCore import Qt, QThread
from PyQt5.QtGui import QFont
from PyQt5.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, QApplication, QPushButton, QMessageBox, QCheckBox,
                             QTableWidgetItem, QTreeWidget, QHeaderView)
from qt_material import apply_stylesheet

from qtui.infowindow import InfoWindow
from qtui.io.petri import read_net, read_resources, write_resources
from qtui.io.tree import read_tree, read_basis
from qtui.widgetutils import show_message, create_label, QVLine, QHLine, create_table, get_file, Checker, \
    populate_tree_view


class MainWindow(QWidget):

    @property
    def net_loaded(self):
        return self._net_loaded

    @net_loaded.setter
    def net_loaded(self, value: bool):
        for el in self.togglable_elements:
            el.setEnabled(value)
        if value:
            self.labels = [x[0] for x in sorted(self.net['place_id'].items(), key=lambda x: x[1])]
            self.place_num = len(self.labels)
            for table in (self.first_table, self.second_table):
                table.clear()
                table.setColumnCount(self.place_num)
                table.setHorizontalHeaderLabels(self.labels)
                for i in range(self.place_num):
                    table.setItem(0, i, QTableWidgetItem(str(0)))
        self._net_loaded = value

    def r_table(self) -> str:
        for i in range(self.place_num):
            yield self.first_table.item(0, i).text()

    def s_table(self) -> str:
        for i in range(self.place_num):
            yield self.second_table.item(0, i).text()

    def __init__(self, path: str):
        super().__init__()
        self.base_path = path
        self.outer = QHBoxLayout()
        self.net: dict[str, dict] = {}
        self.place_num = 0
        self._net_loaded = False
        self.togglable_elements: List[QWidget] = []
        self.light_theme = True
        sys.excepthook = error_handling
        apply_stylesheet(self, os.path.join(path, 'stylesheets', 'colors_light.xml'), invert_secondary=True)
        self.init_ui()

    def init_ui(self):
        """
        Initializes UI
        """
        with open(os.path.join(self.base_path, "stylesheets", "table_light.qml"), 'r') as theme:
            self.theme_light_table = "".join(theme.readlines())
        with open(os.path.join(self.base_path, "stylesheets", "table_dark.qml"), 'r') as theme:
            self.theme_dark_table = "".join(theme.readlines())
        self.init_layout()
        self.setLayout(self.outer)
        rec = QApplication.desktop().screenGeometry()
        self.resize(rec.width() * 2 // 3, rec.height() * 2 // 3)
        self.move(300, 150)
        self.setWindowTitle('Bisimilarity Checker')
        self.setWindowIcon(QtGui.QIcon(os.path.join(self.base_path, 'icons', 'icon.png')))
        self.net_loaded = False
        self.show()

    def init_layout(self):
        """
        Initializes the layout of the main window
        """
        left_layout = QVBoxLayout()
        left_layout.setContentsMargins(0, 0, 0, 0)
        title_label = create_label("Bisimilarity checker", font_size=20)
        left_layout.addWidget(title_label, 3)
        left_layout.addWidget(create_label("Import a Petri net to begin", font_size=10))

        # Importing layout
        import_layout = QHBoxLayout()
        import_net = QPushButton("Import a Petri net")
        import_net.clicked.connect(self.select_net)
        import_layout.addWidget(import_net)
        import_res = QPushButton("Import resources")
        import_res.clicked.connect(self.select_resources)
        import_layout.addWidget(import_res)
        self.togglable_elements.append(import_res)
        left_layout.addLayout(import_layout, 20)
        left_layout.addWidget(QHLine(), 10)

        # State layouts
        s_label = create_label("First resource:", alignment=Qt.AlignCenter)
        s_hint_label = create_label("Double-click on a cell to edit the corresponding resource", font_size=10)
        left_layout.addWidget(s_label, 3)
        left_layout.addWidget(s_hint_label, 1)
        self.first_table = create_table()
        left_layout.addWidget(self.first_table, 100)
        r_label = create_label("Second resource:", alignment=Qt.AlignCenter)
        r_hint_label = create_label("Double-click on a cell to edit the corresponding resource", font_size=10)
        left_layout.addWidget(r_label, 3)
        left_layout.addWidget(r_hint_label, 1)
        self.second_table = create_table()
        left_layout.addWidget(self.second_table, 100)
        self.togglable_elements.extend(
            [self.first_table, self.second_table, s_label, r_label, s_hint_label, r_hint_label])

        # Export resources button
        export_res_layout = QVBoxLayout()
        export_res_layout.setContentsMargins(100, 10, 100, 10)
        export_res_button = QPushButton("Export resources")
        export_res_button.clicked.connect(self.export_resources)
        self.togglable_elements.append(export_res_button)
        export_res_layout.addWidget(export_res_button)
        left_layout.addLayout(export_res_layout, 100)
        left_layout.addWidget(QHLine(), 10)

        # Basis checkbox
        self.basis_box = QCheckBox("Calculate an approximation of the bisimilarity basis")
        self.basis_box.setFont(QFont("Poppins-Regular", 12))
        self.togglable_elements.append(self.basis_box)
        left_layout.addWidget(self.basis_box, alignment=Qt.AlignCenter)

        # Start button
        start_layout = QVBoxLayout()
        start_layout.setContentsMargins(100, 10, 100, 10)
        check_hint_label = create_label("You will need to select the file for results", font_size=10)
        self.togglable_elements.append(check_hint_label)
        start_button = QPushButton("CHECK BISIMILARITY")
        start_button.clicked.connect(self.run_algorithm)
        self.togglable_elements.append(start_button)
        start_layout.addWidget(start_button)
        left_layout.addWidget(check_hint_label)
        left_layout.addLayout(start_layout, 100)
        left_layout.addWidget(QHLine(), 3)

        # Bottom buttons
        info_layout = QHBoxLayout()
        info_button = QPushButton("Information")
        info_button.clicked.connect(self.view_info)
        info_layout.addWidget(info_button)
        theme_button = QPushButton("Switch theme")
        theme_button.clicked.connect(self.switch_theme)
        info_layout.addWidget(theme_button)
        quit_button = QPushButton("Quit")
        quit_button.clicked.connect(sys.exit)
        info_layout.addWidget(quit_button)
        left_layout.addLayout(info_layout, 20)

        # Status label layout
        right_layout = QVBoxLayout()
        self.status_label = create_label("", font_size=20)
        right_layout.addWidget(self.status_label)

        # Tree viewer layout
        right_layout.addWidget(create_label("Result tree viewer: ", font_size=15))
        self.tree_viewer = QTreeWidget()
        self.tree_viewer.setHeaderLabels(["ID", "FIRST", "SECOND", "STEP"])
        self.tree_viewer.setAlternatingRowColors(True)
        self.tree_viewer.header().setSectionResizeMode(QHeaderView.ResizeToContents)
        self.tree_viewer.setStyleSheet(self.theme_light_table)
        right_layout.addWidget(self.tree_viewer)

        self.outer.addLayout(left_layout)
        self.outer.addWidget(QVLine())
        self.outer.addLayout(right_layout)

    def view_info(self):
        window = InfoWindow(self)
        window.exec_()

    def select_net(self):
        """
        Method for handling net selection and importing
        """
        net_path = get_file("Petri Net Markup Language files (*.pnml)", self.base_path, read=True)
        if not net_path:
            return
        try:
            self.net = read_net(net_path)
            self.net_loaded = True
        except KeyError as err:
            show_message(QMessageBox.Warning, "There was an error while reading the Petri net:", str(err))

    def select_resources(self):
        """
        Method for handling resource importing
        """
        resource_path = get_file("CSV table (*.csv)", self.base_path, read=True)
        if not resource_path:
            return
        try:
            res = read_resources(resource_path, self.labels)
            for key, (r, s) in res.items():
                self.first_table.setItem(0, self.net['place_id'][key], QTableWidgetItem(r))
                self.second_table.setItem(0, self.net['place_id'][key], QTableWidgetItem(s))
        except (KeyError, ValueError) as err:
            show_message(QMessageBox.Warning, "There was an error while reading the resources:", str(err))

    def export_resources(self):
        """
        Method for handling resource export to .csv
        """
        resource_path = get_file("CSV table (*.csv)", self.base_path, read=False)
        if not resource_path:
            return
        try:
            write_resources(resource_path, self.labels, self.r_table(), self.s_table())
        except (OSError, IOError, FileExistsError, FileNotFoundError) as err:
            show_message(QMessageBox.Warning, "There was an error while exporting the resources:", str(err))

    def run_algorithm(self):
        """
        Runs the algorithm itself after getting a path to save the tree to
        """
        tree_path = get_file("GraphML file (*.graphml)", self.base_path, read=False)
        if not tree_path:
            return
        if len(tree_path) < 9 or tree_path[-8:] != '.graphml':
            tree_path += '.graphml'
        transitions = [(key, *val) for key, val in self.net['transitions'].items()]
        self.status_label.setText("Running...")
        self.status_label.setStyleSheet("QLabel { color : black; }")
        self.setDisabled(True)

        # Initializing a thread
        self.checker = Checker(list(map(int, self.s_table())), list(map(int, self.r_table())),
                               transitions, self.basis_box.isChecked(), tree_path)
        self.check_thread = QThread()
        self.checker.moveToThread(self.check_thread)
        self.checker.finished.connect(self.check_thread.quit)
        self.check_thread.started.connect(self.checker.run_algorithm)
        self.check_thread.finished.connect(self.show_tree)
        self.check_thread.start()

    def show_tree(self):
        """
        Handles the ending of the algorithm runtime and subsequent tree loading
        """
        self.setDisabled(False)
        if self.checker.result:
            self.status_label.setText("Bisimilarity found")
            self.status_label.setStyleSheet("QLabel { color : green; }")
        else:
            self.status_label.setText("Bisimilarity not found")
            self.status_label.setStyleSheet("QLabel { color : red; }")
        basis = None
        if self.checker.check_basis:
            basis = read_basis(self.checker.path)
        populate_tree_view(read_tree(self.checker.path), self.tree_viewer, basis)

    def switch_theme(self):
        """
        Switches application theme from light to dark and vise-versa
        """
        if self.light_theme:
            apply_stylesheet(self, os.path.join(self.base_path, 'stylesheets', 'colors_dark.xml'))
            self.tree_viewer.setStyleSheet(self.theme_dark_table)
        else:
            apply_stylesheet(self, os.path.join(self.base_path, 'stylesheets', 'colors_light.xml'),
                             invert_secondary=True)
            self.tree_viewer.setStyleSheet(self.theme_light_table)
        self.light_theme = not self.light_theme


def error_handling(_, exc_value, __):
    show_message(QMessageBox.Critical, "An error has occurred, the program will now exit", str(exc_value))
    exit(1)
