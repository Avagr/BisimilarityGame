import os

from PyQt5.QtWidgets import QVBoxLayout, QTextEdit, QApplication, QDialog, QWidget

from qtui.widgetutils import create_label


class InfoWindow(QDialog):

    def __init__(self, parent: QWidget):
        super().__init__()
        self.parent = parent
        self.init_ui()
        rec = QApplication.desktop().screenGeometry()
        self.resize(rec.width() // 2, rec.height() // 2)
        self.move(400, 250)
        self.setWindowTitle('')
        self.setStyleSheet(parent.styleSheet())
        self.show()

    def init_ui(self):
        layout = QVBoxLayout()
        layout.addWidget(create_label("Information", font_size=20))
        text = QTextEdit()
        text.setReadOnly(True)
        with open(os.path.join(self.parent.base_path, 'instructions.md'), 'r') as file:
            markdown = ''.join(file.readlines())
        text.setMarkdown(markdown)
        text.setStyleSheet("background-color: #ffffff;")
        layout.addWidget(text)

        self.setLayout(layout)
