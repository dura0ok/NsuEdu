from PyQt5.QtWidgets import QWidget, QVBoxLayout
from PyQt5.QtGui import QPixmap, QPainter, QColor, QImage
from PyQt5.QtCore import Qt, QSize, QRect, QFile, QIODevice, QUrl

class WindowPanel(QWidget):
    def __init__(self, file_name, width, height):
        super().__init__()
        self.width = width
        self.height = height
        self.image_icon = self.set_image_icon(file_name)
        self.setFocusPolicy(Qt.StrongFocus)


    def set_image_icon(self, file_name):
        file = QFile(file_name)
        if not file.exists():
            default_background = QImage(self.width, self.height, QImage.Format_RGB32)
            default_background.fill(QColor(46, 101, 217))
            return QPixmap.fromImage(default_background)
        else:
            background = QPixmap(file_name)
            resized_background = background.scaled(QSize(self.width, self.height), Qt.AspectRatioMode.KeepAspectRatio)
            return resized_background

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.drawImage(QRect(0, 0, self.width, self.height), self.image_icon.toImage())


