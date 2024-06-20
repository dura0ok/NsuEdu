import traceback

from PyQt5.QtWidgets import QWidget, QPushButton, QLineEdit, QMessageBox
from PyQt5.QtGui import QPixmap, QColor, QIcon
from PyQt5.QtCore import Qt, QSize
from view.panels.window_panel import WindowPanel
from view.view_utils import ViewUtils
from view.utils import get_part
from lib.snakes import PlayerType

class CreatingGamePanel(WindowPanel):
    def __init__(self, listener, width, height):
        super().__init__('resources/CreatingNewGame.png', width, height)

        self.listener = listener
        self.port = 1024
        self.name = "Player"
        self.player_type = PlayerType.HUMAN

        self.player_type_width = get_part(width, 0.03125)
        self.player_type_height = get_part(height, 0.0555)


        self.create_ui(width, height)

    def create_ui(self, width, height):
        self.name_field = self.create_text_field("Player", Qt.white, 15, width * 0.188, height * 0.32, width * 0.198, height * 0.053, self.update_name)
        self.port_field = self.create_text_field(str(self.port), Qt.white, 15, width * 0.188, height * 0.53, width * 0.198, height * 0.054, self.update_port)

        self.human_type = self.init_button_for_player_type("resources/Tick.png", width * 0.645, height * 0.377, self.change_chosen_box)
        self.computer_type = self.init_button_for_player_type("resources/Empty.png", width * 0.645, height * 0.435, self.change_chosen_box)

        self.start_game_button = self.create_button("", self.create_game, width*0.64, height*0.7343, width*0.2539, height*0.146)
        self.config_settings_button = self.create_button("", self.listener.open_config_settings, width * 0.186, height * 0.705, width * 0.315, height * 0.12)
        self.close_game_button = self.create_button("", self.listener.close_the_game, width * 0.01570, height * 0.0277, width * 0.0929, height * 0.1041)
        self.back_to_menu_button = self.create_button("", self.listener.back_to_start_menu, width * 0.009, height * 0.16, width * 0.11, height * 0.082)

    def create_button(self, text, function, pos_x, pos_y, width, height):
        button = ViewUtils.init_button(int(width), int(height), int(pos_x), int(pos_y), function)
        button.setText(text)
        button.setParent(self)

    def create_text_field(self, default_text, color, columns, pos_x, pos_y, width, height, update_function):
        text_field = QLineEdit(default_text, self)
        text_field.setStyleSheet(f"color: red; font-size: {columns}px;")
        text_field.setMaxLength(columns)
        text_field.setGeometry(int(pos_x), int(pos_y), int(width), int(height))

        text_field.textChanged.connect(lambda text: update_function(text, text_field, default_text))
        return text_field

    def init_button_for_player_type(self, icon_file, pos_x, pos_y, function):
        button = QPushButton(self)
        button.setIcon(QIcon(icon_file))
        button.setIconSize(QSize(self.player_type_width, self.player_type_height))
        button.setGeometry(int(pos_x), int(pos_y), self.player_type_width, self.player_type_height)
        button.setFocusPolicy(Qt.NoFocus)
        button.clicked.connect(lambda: function(button))
        return button

    def change_chosen_box(self, button):
        if button == self.human_type:
            self.player_type = PlayerType.HUMAN
            self.human_type.setIcon(QIcon("resources/Tick.png"))
            self.computer_type.setIcon(QIcon("resources/Empty.png"))
        elif button == self.computer_type:
            self.player_type = PlayerType.ROBOT
            self.human_type.setIcon(QIcon("resources/Empty.png"))
            self.computer_type.setIcon(QIcon("resources/Tick.png"))

    def update_name(self, text, text_field, default_text):
        if not text:
            self.name = default_text
        else:
            self.name = text

    def update_port(self, text, text_field, default_text):
        try:
            parsed_int = int(text)
            if 1024 <= parsed_int <= 49151:
                self.port = parsed_int
            else:
                text_field.setText(f"Wrong port {parsed_int}")
                QMessageBox.critical(self, "Invalid Port", "Please, type port only from interval (1024, 49151)")
                text_field.setText(str(self.port))
        except ValueError:
            text_field.setText(default_text)

    def create_game(self):
        self.listener.settings['port_field'] = self.port
        self.listener.settings['name_field'] = self.name
        self.listener.settings['player_type'] = self.player_type
        self.listener.start_the_game()
