from typing import Tuple

import bisimilarity_checker
from PyQt5 import QtCore
from PyQt5.QtCore import Qt, pyqtSignal, QObject, pyqtSlot
from PyQt5.QtGui import QFont, QIntValidator, QBrush, QColor
from PyQt5.QtWidgets import QMessageBox, QLabel, QFrame, QTableWidget, QItemDelegate, QWidget, QStyleOptionViewItem, \
    QLineEdit, QFileDialog, QTreeWidget, QTreeWidgetItem

from qtui.io.tree import Node


def show_message(icon: QMessageBox.Icon, title: str, text: str) -> None:
    """
    Shows a message in a small dialog box
    :param icon: icons to set, signals the importance of message
    :param title: top level message to display
    :param text: additional info message to display
    """
    dialog = QMessageBox()
    dialog.setIcon(icon)
    dialog.setText(title)
    dialog.setInformativeText(text)
    dialog.setStandardButtons(QMessageBox.Close)
    dialog.resize(400, 300)
    dialog.exec_()


def create_label(text: str, font_size: int = 15,
                 alignment: str = Qt.AlignCenter) -> QLabel:
    """
    Creates a lable in a single line with fixed params
    :param text: text of the label
    :param font_size: font size
    :param alignment: label alignment
    :return: created label
    """
    label = QLabel(text)
    label.setAlignment(alignment)
    label.setFont(QFont("Poppins", font_size))
    return label


def create_table() -> QTableWidget:
    """
    Creates a table with one row and no columns
    :return: created table
    """
    table = QTableWidget(1, 0)
    table.setMaximumHeight(table.rowHeight(0) + 37)
    table.setMinimumHeight(table.rowHeight(0) + 37)
    table.verticalHeader().hide()
    table.setItemDelegate(NumberDelegate())
    return table


def get_file(file_filter: str, base_path: str, read: bool) -> str:
    """
    Shows a file dialog and returns the first chosen file
    :param read: true if the file id for reading, false if for writing into
    :param file_filter: file type filter
    :param base_path: base path to show to the user
    :return: None if no file was selected, path to the file otherwise
    """
    dlg = QFileDialog()
    dlg.resize(800, 500)
    dlg.setFileMode(QFileDialog.ExistingFile if read else QFileDialog.AnyFile)
    dlg.setAcceptMode(QFileDialog.AcceptOpen if read else QFileDialog.AcceptSave)
    dlg.setNameFilters([file_filter])
    dlg.selectNameFilter(file_filter)
    dlg.setDirectory(base_path[:-3])
    dlg.exec_()
    return dlg.selectedFiles()[0] if len(dlg.selectedFiles()) > 0 and dlg.selectedFiles()[
        0] != dlg.directory().path() else None


class Checker(QObject):
    """
    Helper for async bisimilarity checking
    """
    finished = pyqtSignal()

    def __init__(self, r, s, trans, basis, path):
        super().__init__()
        self.r_res = r
        self.s_res = s
        self.transitions = trans
        self.check_basis = basis
        self.path = path

    @pyqtSlot()
    def run_algorithm(self):
        self.result = bisimilarity_checker.check_bisimilarity(self.r_res, self.s_res, self.transitions,
                                                              self.check_basis,
                                                              self.path)
        # noinspection PyUnresolvedReferences
        self.finished.emit()


def populate_tree_view(root: Node, tree: QTreeWidget, basis=None) -> None:
    """
    Populates a given tree view with a tree and optionally a basis
    :param root: root of a tree
    :param tree: tree view to populate
    :param basis: basis of the
    """
    root_item = QTreeWidgetItem(root.to_list())
    for i in range(2):
        root_item.setTextAlignment(i + 1, Qt.AlignCenter)
    stack: list[Tuple[Node, QTreeWidgetItem]] = [(root, root_item)]

    # Initializing brushes
    success = QBrush()
    fail = QBrush()
    success.setColor(QColor(Qt.green))
    fail.setColor(QColor(Qt.red))

    while len(stack) > 0:
        cur, cur_item = stack.pop()
        for child in cur.children:
            item = QTreeWidgetItem(child.to_list())
            for i in range(2):
                item.setTextAlignment(i + 1, Qt.AlignCenter)
            cur_item.addChild(item)
            stack.append((child, item))
        if cur.terminal == "SUCCESS":
            cur_item.setBackground(0, success)
        elif cur.terminal == "FAIL":
            cur_item.setBackground(0, fail)

    tree.clear()
    tree.addTopLevelItem(root_item)

    if basis:
        basis_root = QTreeWidgetItem(["Basis"])
        for i in range(2):
            basis_root.setTextAlignment(i + 1, Qt.AlignCenter)
        for num, (first, second) in enumerate(basis):
            child = QTreeWidgetItem(
                ['#' + str(num), '(' + ", ".join(first) + ')', '(' + ", ".join(second) + ')', 'BASIS'])
            for i in range(2):
                child.setTextAlignment(i + 1, Qt.AlignCenter)
            basis_root.addChild(child)
        tree.addTopLevelItem(basis_root)


class NumberDelegate(QItemDelegate):
    def createEditor(self, parent: QWidget, option: 'QStyleOptionViewItem', index: QtCore.QModelIndex) -> QWidget:
        editor = QLineEdit(parent)
        editor.setValidator(QIntValidator(0, 2_147_483_647))
        editor.setAlignment(Qt.AlignCenter)
        return editor


class QHLine(QFrame):
    """
    Simple horizontal line
    """

    def __init__(self):
        super(QHLine, self).__init__()
        self.setFrameShape(QFrame.HLine)
        self.setFrameShadow(QFrame.Sunken)


class QVLine(QFrame):
    """
    Simple vertical line
    """

    def __init__(self):
        super(QVLine, self).__init__()
        self.setFrameShape(QFrame.VLine)
        self.setFrameShadow(QFrame.Sunken)
