from PyQt5.QtCore import Qt
from PyQt5.QtGui import QFont
from PyQt5.QtWidgets import QMessageBox, QLabel


def show_message(icon: QMessageBox.Icon, title: str, text: str) -> None:
    """
    Shows a message in a small dialog box
    :param icon: icon to set, signals the importance of message
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


def create_label(text: str, font_size: int = 15, font_name: str = "Poppins-Regular",
                 alignment: str = Qt.AlignCenter) -> QLabel:
    label = QLabel(text)
    label.setAlignment(alignment)
    label.setFont(QFont(font_name, font_size))
    return label
