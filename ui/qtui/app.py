import sys

from PyQt5.QtWidgets import QApplication

from .mainwindow import MainWindow


def run(path: str):
    sys_argv = sys.argv
    app = QApplication(sys_argv)
    _ = MainWindow(path)
    sys.exit(app.exec_())

