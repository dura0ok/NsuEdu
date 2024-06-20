from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPixmap, QImage, QColor
from PyQt5.QtWidgets import QWidget, QPushButton, QLabel, QVBoxLayout

from view.utils import get_part


class GameLinePanel(QWidget):
    def __init__(self, joining_game_panel, filename, width, height, pos_x, pos_y, data):
        super().__init__()

        self.joining_game_panel = joining_game_panel
        self.filename = filename

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)

        self.setFixedSize(width, height)
        self.setGeometry(pos_x, pos_y, width, height)

        self.label_name = QLabel(data)
        self.label_name.setFixedSize(get_part(height, 0.32), get_part(width, 0.7))
        self.label_name.setGeometry(get_part(width, 0.1), get_part(height, 0.25),
                                    get_part(height, 0.32), get_part(width, 0.7))

        button = QPushButton(self)
        button.setFixedSize(width//2, height//2)
        button.clicked.connect(lambda: self.choose_game(data))
        # button.setGeometry(0, 0, width-3, height-3)
        button.setText(data)

        layout.addWidget(self.label_name)
        layout.addWidget(button)

    def make_chosen_background(self):
        self.set_image_icon("/" + self.filename[:self.filename.index('.')] + "Chosen.png")

    def make_default_background(self):
        self.set_image_icon("/" + self.filename)

    def choose_game(self, data):
        self.make_chosen_background()
        self.joining_game_panel.choose_game(data)

    def set_image_icon(self, file_directory):
        pixmap = QPixmap(file_directory)
        if pixmap.isNull():
            default_background = QImage(self.width(), self.height(), QImage.Format_RGB32)
            default_background.fill(QColor(255, 255, 255))
            self.label_name.setPixmap(QPixmap.fromImage(default_background))
        else:
            pixmap = pixmap.scaled(self.width(), self.height(), Qt.KeepAspectRatio, Qt.SmoothTransformation)
            self.label_name.setPixmap(pixmap)
