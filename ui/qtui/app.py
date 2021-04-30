import sys

from PyQt5.QtWidgets import QApplication

from .mainwindow import MainWindow


def run(path: str):
    app = QApplication([])
    _ = MainWindow(path)
    sys.exit(app.exec_())

# import bisimilarity_checker
#
# from qtui.parsing.petri import read_net
#
# dat = read_net("../nets/test.pnml")
# trans = []
# for key, val in dat['transitions'].items():
#     trans.append((key, val[2], val[0], val[1]))
# print(bisimilarity_checker.check_bisimilarity([1, 0], [0, 3], trans))
