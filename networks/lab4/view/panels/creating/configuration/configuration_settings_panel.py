from PyQt5.QtWidgets import QWidget, QPushButton, QLineEdit, QVBoxLayout, QLabel, QMessageBox
from PyQt5.QtGui import QPixmap, QColor
from PyQt5.QtCore import Qt, QRect, QSize
from view.panels.window_panel import WindowPanel
from view.view_utils import ViewUtils
from view.utils import get_part


class ConfigurationSettingsPanel(WindowPanel):
    def __init__(self, listener, width, height):
        super().__init__('resources/ConfigurationSettings.png', width, height)

        self.listener = listener
        self.width = width
        self.height = height
        self.setting_width = int(width * 0.08)
        self.setting_height = int(width * 0.03)
        self.font_size = int(width * 0.013)
        self.is_saved = False

        self.width_game = 40
        self.height_game = 30
        self.food_static = 1
        self.food_per_player = 1
        self.state_delay = 200
        self.dead_food_prob = 0.1
        self.ping_delay = 100
        self.node_timeout = 800

        self.create_ui()

    def create_ui(self):
        self.create_button("", self.listener.close_the_game, self.width*0.01570, self.height*0.0277, self.width*0.0929, self.height*0.1041)
        self.create_button("", self.listener.back_to_creating_menu, self.width*0.009, self.height*0.16, self.width*0.11, self.height*0.082)
        self.create_button("", self.save_config, self.width*0.622, self.height*0.77, self.width*0.35, self.height*0.175)
        self.create_button("", self.reset_config, self.width*0.52, self.height*0.67, self.width*0.098, self.height*0.086)

        self.width_game_field = self.create_text_field((str(self.width_game) if 'width_game_field' not in self.listener.settings.keys() else self.listener.settings['width_game_field']), Qt.black, 3, self.width*0.325, self.height*0.299, self.width*0.04, self.height*0.05)
        self.height_game_field = self.create_text_field((str(self.height_game) if 'height_game_field' not in self.listener.settings.keys() else self.listener.settings['height_game_field']), Qt.black, 3, self.width*0.325, self.height*0.399, self.width*0.04, self.height*0.051)
        self.food_static_field = self.create_text_field((str(self.food_static) if 'food_static_field' not in self.listener.settings.keys() else self.listener.settings['food_static_field']), Qt.black, 3, self.width*0.795, self.height*0.289, self.width*0.043, self.height*0.05)
        self.food_per_player_field = self.create_text_field((str(self.food_per_player) if 'food_per_player_field' not in self.listener.settings.keys() else self.listener.settings['food_per_player_field']), Qt.black, 3, self.width*0.795, self.height*0.389, self.width*0.043, self.height*0.051)
        self.state_delay_field = self.create_text_field((str(self.state_delay) if 'state_delay_field' not in self.listener.settings.keys() else self.listener.settings['state_delay_field']), Qt.black, 4, self.width*0.325, self.height*0.728, self.width*0.045, self.height*0.05)
        self.dead_food_prob_field = self.create_text_field((str(self.dead_food_prob) if 'dead_food_prob_field' not in self.listener.settings.keys() else self.listener.settings['dead_food_prob_field']), Qt.black, 3, self.width*0.795, self.height*0.546, self.width*0.043, self.height*0.051)
        self.ping_delay_field = self.create_text_field((str(self.ping_delay) if 'ping_delay_field' not in self.listener.settings.keys() else self.listener.settings['ping_delay_field']), Qt.black, 4, self.width*0.325, self.height*0.857, self.width*0.045, self.height*0.05)
        self.node_timeout_field = self.create_text_field((str(self.node_timeout) if 'node_timeout_field' not in self.listener.settings.keys() else self.listener.settings['node_timeout_field']), Qt.black, 4, self.width*0.325, self.height*0.614, self.width*0.045, self.height*0.05)
        self.local_save = {
            self.width_game_field: {'name': 'width_game_field', 'pr_val': str(self.width_game), 'type': int},
            self.height_game_field: {'name': 'height_game_field', 'pr_val': str(self.height_game), 'type': int},
            self.food_static_field: {'name': 'food_static_field', 'pr_val': str(self.food_static), 'type': int},
            self.food_per_player_field: {'name': 'food_per_player_field', 'pr_val': str(self.food_per_player), 'type': float},
            self.state_delay_field: {'name': 'state_delay_field', 'pr_val': str(self.state_delay), 'type': int},
            self.dead_food_prob_field: {'name': 'dead_food_prob_field', 'pr_val': str(self.dead_food_prob), 'type': float},
            self.ping_delay_field: {'name': 'ping_delay_field', 'pr_val': str(self.ping_delay), 'type': int},
            self.node_timeout_field: {'name': 'node_timeout_field', 'pr_val': str(self.node_timeout), 'type': int},
        }

    def create_button(self, text, function, pos_x, pos_y, width, height):
        button = ViewUtils.init_button(int(width), int(height), int(pos_x), int(pos_y), function)
        button.setText(text)
        button.setParent(self)

    def create_text_field(self, default_text, color, columns, pos_x, pos_y, width, height):
        text_field = QLineEdit(default_text, self)
        text_field.setStyleSheet(f"color:  blue; font-size: {self.font_size}px;")
        text_field.setMaxLength(columns)
        text_field.setGeometry(QRect(int(pos_x), int(pos_y), int(width), int(height)))

        text_field.textChanged.connect(lambda text: self.on_text_changed(text, text_field, default_text))

        return text_field

    def on_text_changed(self, text: str, text_field, default_text):
        try:
            if text:
                data = self.local_save[text_field]
                def chDic(x: str):
                    return x.count('.') < 2 and x.replace('.', '').isdigit()
                if data['type'] == float:
                    if chDic(text):
                        text = float(text)
                    else:
                        text_field.setText(data['pr_val'])
                        message = f'Please, this text field takes only {data["type"]} type'
                        QMessageBox.critical(self, "Invalid Value", message)
                        return
                elif data['type'] == int:
                    if text.isdigit():
                        text = int(text)
                    else:
                        text_field.setText(data['pr_val'])
                        message = f'Please, this text field takes only {data["type"]} type'
                        QMessageBox.critical(self, "Invalid Value", message)
                        return
                elif data['type'] == str:
                    pass
                data['pr_val'] = str(text)
                self.update_value(text, text_field)
        except Exception as ex: print(ex, 'sett_pan 98')


    def update_value(self, parsed_value, text_field):
        attribute_name = text_field.objectName()
        if not self.is_valid_value(parsed_value, attribute_name):
            text_field.setText(str(getattr(self, attribute_name)))
        else:
            setattr(self, attribute_name, parsed_value)
            print(f"{attribute_name} = {parsed_value}")

    def is_valid_value(self, value, attribute_name):
        limits = {
            "width_game": (10, 100),
            "height_game": (10, 100),
            "food_static": (0, 100),
            "food_per_player": (0, 100),
            "state_delay": (1, 10000),
            "dead_food_prob": (0, 1),
            "ping_delay": (1, 10000),
            "node_timeout": (1, 10000),
        }
        left_limit, right_limit = limits.get(attribute_name, (None, None))
        if left_limit is not None and right_limit is not None:
            if not (left_limit <= value <= right_limit):
                message = f"Please, type value only from interval ({left_limit}, {right_limit})"
                QMessageBox.critical(self, "Invalid Value", message)
                return False
        return True

    def save_config(self):
        for key in self.local_save.keys():
            self.listener.settings[self.local_save[key]['name']] = key.text()
        print(self.listener.settings)
        self.listener.back_to_creating_menu()

    def reset_config(self):
        for key in self.local_save.keys():
            self.local_save[key]['pr_val'] = key.text()

        self.width_game_field.setText('40')
        self.height_game_field.setText('30')
        self.food_static_field.setText('1')
        self.food_per_player_field.setText('1')
        self.state_delay_field.setText('1000')
        self.dead_food_prob_field.setText('0.1')
        self.ping_delay_field.setText('100')
        self.node_timeout_field.setText('800')
