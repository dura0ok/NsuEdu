from PyQt5.QtCore import QSize
from PyQt5.QtGui import QPainter
from PyQt5.QtWidgets import QWidget, QLabel, QVBoxLayout


class CellPanel(QWidget):
    def __init__(self, cell_size, default_icon):
        super().__init__()

        self.image_label = QLabel(self)
        self.image_icon = default_icon

        layout = QVBoxLayout(self)
        layout.addWidget(self.image_label)

        self.setFixedSize(cell_size, cell_size)
        self.change_background(default_icon)

    def change_background(self, icon):
        self.image_icon = icon
        self.repaint()

    def paintEvent(self, event):
        super().paintEvent(event)
        painter = QPainter(self)
        painter.drawPixmap(0, 0, self.image_icon.scaled(QSize(self.width(), self.height())))
