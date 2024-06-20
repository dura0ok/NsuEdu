from PyQt5.QtCore import Qt, QSize
from PyQt5.QtGui import QIcon, QPixmap, QImage, QColor, QFont
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QLabel, QScrollArea, QLineEdit, QVBoxLayout


class ViewUtils:
    def __init__(self):
        pass

    @staticmethod
    def init_button(width, height, pos_x, pos_y, listener):
        button = QPushButton()
        button.setFixedSize(width, height)
        button.setFocusPolicy(Qt.NoFocus)
        button.setAutoFillBackground(True)
        button.setStyleSheet("border: none")
        button.setCursor(Qt.PointingHandCursor)
        button.clicked.connect(listener)
        button.move(pos_x, pos_y)
        return button

    @staticmethod
    def get_image_button_icon(file_directory, color_for_button, width, height):
        pixmap = QPixmap(file_directory)

        if pixmap.isNull():
            default_background = QImage(width, height, QImage.Format_RGB32)
            # default_background.fill(QColor(QColor.red, QColor.green, QColor.blue))
            return QIcon(QPixmap.fromImage(default_background))
        else:
            pixmap = pixmap.scaled(QSize(width, height))
            return QIcon(pixmap)

    @staticmethod
    def init_label(text, font_size, width, height, pos_x, pos_y):
        label = QLabel(text)
        label.setFont(QFont("Roboto", font_size, QFont.Bold))
        label.setStyleSheet("color: blue;")
        label.setFixedSize(width, height)
        label.setGeometry(pos_x, pos_y, width, height)
        return label

    @staticmethod
    def init_scroll_area(width, height, pos_x, pos_y):
        scroll_area = QScrollArea()
        # scroll_area.viewport().setAutoFillBackground(False)
        # scroll_area.setFrameShape(QScrollArea.NoFrame)
        scroll_area.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        # scroll_area.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        scroll_area.setFixedSize(width, height)
        scroll_area.move(pos_x, pos_y)
        return scroll_area

    @staticmethod
    def init_text_field(default_text, color, font_size, columns, width, height, pos_x, pos_y):
        text_field = QLineEdit(default_text)
        text_field.setFont(QFont("Roboto", font_size, QFont.Bold))
        text_field.setStyleSheet("color: {}; border: none;".format(color.name()))
        text_field.setFixedSize(width, height)
        text_field.setGeometry(pos_x, pos_y, width, height)
        return text_field
