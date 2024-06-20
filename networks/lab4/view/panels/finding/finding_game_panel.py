from PyQt5.QtCore import Qt, QSize
from PyQt5.QtGui import QPixmap, QColor, QImage, QIcon
from PyQt5.QtWidgets import QWidget, QPushButton, QLineEdit, QVBoxLayout, QMessageBox
from view.panels.window_panel import WindowPanel
from view.view_utils import ViewUtils

from lib import PlayerType
from view.utils import get_part


class FindingGamePanel(WindowPanel):
    def __init__(self, listener, width, height):
        super().__init__('resources/FindingAGame.png', width, height)

        self.listener = listener
        self.port = 10245
        self.name = "Player"
        self.player_type = PlayerType.HUMAN
        self.isViewer = False;

        self.setFixedSize(width, height)

        self.create_button("", self.listener.launch_finding_game_module, get_part(width, 0.64), get_part(height, 0.745),
                           get_part(width, 0.255), get_part(height, 0.135))

        self.create_button("", self.listener.close_the_game, get_part(width, 0.01570), get_part(height, 0.0277),
                           get_part(width, 0.0929), get_part(height, 0.1041))

        self.create_button("", self.listener.back_to_start_menu, get_part(width, 0.009), get_part(height, 0.16),
                            get_part(width, 0.11), get_part(height, 0.082))

        self.create_text_field("Player", Qt.white, 15, get_part(width, 0.013), get_part(width, 0.188), get_part(height, 0.32), get_part(width, 0.197), get_part(height, 0.055)).setParent(self)

        self.create_text_field(str(self.port), Qt.white, 15,
                                get_part(width, 0.013), get_part(width, 0.188), get_part(height, 0.53), get_part(width, 0.197), get_part(height, 0.055)).setParent(self)

        self.player_type_width = get_part(width, 0.03125)
        self.player_type_height = get_part(height, 0.0555)

        self.empty_icon = self.get_image_button_icon("resources/Empty.png", QColor(Qt.green))
        self.tick_icon = self.get_image_button_icon("resources/Tick.png", QColor(Qt.blue))

        self.human_type_button = self.init_button_for_player_type(self.tick_icon, get_part(width, 0.645), get_part(height, 0.377))

        self.computer_type_button = self.init_button_for_player_type(self.empty_icon, get_part(width, 0.645), get_part(height, 0.435))

        self.viewer_type_button = self.init_button_for_player_type(self.empty_icon, get_part(width, 0.645), get_part(height, 0.492))

    def create_button(self, text, function, pos_x, pos_y, width, height):
        button = ViewUtils.init_button(width, height, pos_x, pos_y, function)
        button.setText(text)
        button.setParent(self)

    def create_text_field(self, default_text, color, font_size, columns, pos_x, pos_y, width, height):
        text_field = QLineEdit(default_text, self)
        text_field.setStyleSheet(f"color: green; font-size: {font_size}px;")
        text_field.setMaxLength(columns)
        text_field.setGeometry(pos_x, pos_y, width, height)

        text_field.textChanged.connect(lambda text: self.on_text_changed(text, text_field, default_text))

        return text_field

    def on_text_changed(self, text, text_field, default_text):
        if not text:
            text_field.setText(default_text)
        elif text.isdigit():
            self.update_port(int(text))

    def update_port(self, parsed_int):
        if 1024 <= parsed_int <= 49151:
            self.port = parsed_int
        else:
            self.port_field.setText(f"Wrong port {parsed_int}")
            self.show_port_warning()

    def show_port_warning(self):
        QMessageBox.critical(self, "Invalid Port", "Please, type a port only from the interval (1024, 49151)")

    def init_button_for_player_type(self, icon, pos_x, pos_y):
        button = QPushButton(self)
        button.setIcon(QIcon(icon))
        button.setIconSize(QSize(self.player_type_width, self.player_type_height))
        button.setGeometry(pos_x, pos_y, self.player_type_width, self.player_type_height)
        button.clicked.connect(lambda: self.change_chosen_box(button))
        button.setParent(self)
        return button

    def change_chosen_box(self, button):
        if button == self.human_type_button:
            self.player_type = PlayerType.HUMAN
            self.is_viewer = False
            self.human_type_button.setIcon(QIcon(self.tick_icon))
            self.computer_type_button.setIcon(QIcon(self.empty_icon))
            self.viewer_type_button.setIcon(QIcon(self.empty_icon))
        elif button == self.computer_type_button:
            self.player_type = PlayerType.ROBOT
            self.is_viewer = False
            self.human_type_button.setIcon(QIcon(self.empty_icon))
            self.computer_type_button.setIcon(QIcon(self.tick_icon))
            self.viewer_type_button.setIcon(QIcon(self.empty_icon))
        elif button == self.viewer_type_button:
            self.player_type = PlayerType.HUMAN
            self.is_viewer = True
            self.human_type_button.setIcon(QIcon(self.empty_icon))
            self.computer_type_button.setIcon(QIcon(self.empty_icon))
            self.viewer_type_button.setIcon(QIcon(self.tick_icon))

    def get_image_button_icon(self, file_directory, color_for_button):
        pixmap = QPixmap(file_directory)

        if pixmap.isNull():
            default_background = QImage(50, 50, QImage.Format_RGB32)
            default_background.fill(
                QColor(color_for_button.red(), color_for_button.green(), color_for_button.blue()))
            return QPixmap.fromImage(default_background)
        else:
            pixmap = pixmap.scaled(self.player_type_width, self.player_type_height, Qt.KeepAspectRatio,
                                   Qt.SmoothTransformation)
            return pixmap
