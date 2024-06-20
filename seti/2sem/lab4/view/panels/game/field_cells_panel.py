from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPixmap, QImage, QColor
from PyQt5.QtWidgets import QWidget, QLabel, QGridLayout


class FieldCellsPanel(QWidget):
    def __init__(self, cell_size, width_field, height_field):
        super().__init__()

        self.width = cell_size * width_field
        self.height = cell_size * height_field
        self.field_width = width_field
        self.field_height = height_field
        self.cell_size = cell_size

        self.setFocusPolicy(Qt.StrongFocus)
        self.setFixedSize(self.width, self.height)
        self.move(20, 20)

        self.alive_other_snake = self.get_image_icon("resources/AliveOtherSnake.png", QColor(Qt.gray))
        self.alive_node_snake = self.get_image_icon("resources/AliveNodeSnake.png", QColor(Qt.green))
        self.near_snake = self.get_image_icon("resources/NearSnake.png", QColor(Qt.yellow))
        self.food = self.get_image_icon("resources/Food.png", QColor(Qt.blue))
        self.empty = self.get_image_icon("resources/EmptyField.png", QColor(Qt.white))

        self.cells = [[CellPanel(cell_size, self.empty) for x in range(width_field)] for y in range(height_field)]

        for y in range(height_field):
            for x in range(width_field):
                self.cells[y][x].setParent(self)
                self.cells[y][x].move(x*self.cell_size, y*self.cell_size)

    def update_field(self, field_points):
        for fp in field_points:
            coordinate = fp.coordinate
            sym = fp.sym
            if sym == '-' or sym == '.':
                self.cells[coordinate.y][coordinate.x].change_background(self.empty)
            elif sym == '#':
                self.cells[coordinate.y][coordinate.x].change_background(self.alive_other_snake)
            elif sym == '&':
                self.cells[coordinate.y][coordinate.x].change_background(self.alive_node_snake)
            elif sym == '*':
                self.cells[coordinate.y][coordinate.x].change_background(self.food)

    def get_image_icon(self, file_directory, color_for_button):
        pixmap = QPixmap(file_directory)

        if pixmap.isNull():
            default_background = QImage(self.cell_size, self.cell_size, QImage.Format_RGB32)
            default_background.fill(QColor(color_for_button.red(), color_for_button.green(), color_for_button.blue()))
            return QPixmap.fromImage(default_background)
        else:
            pixmap = pixmap.scaled(self.cell_size, self.cell_size, Qt.KeepAspectRatio, Qt.SmoothTransformation)
            return pixmap


class CellPanel(QLabel):
    def __init__(self, cell_size, default_icon):
        super().__init__()

        self.setFixedSize(cell_size, cell_size)
        self.setPixmap(default_icon)

    def change_background(self, icon):
        self.setPixmap(icon)
