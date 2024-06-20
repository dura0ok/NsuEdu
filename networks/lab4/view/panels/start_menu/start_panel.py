from view.panels.window_panel import WindowPanel
from view.utils import get_part
from view.view_utils import ViewUtils
from PyQt5.QtWidgets import QSpacerItem, QSizePolicy

class StartPanel(WindowPanel):
    def __init__(self, listener, width, height):
        super().__init__("resources/Start.png", width, height)

        self.add_button("", get_part(width, 0.29), get_part(height, 0.1555), get_part(width, 0.35555),
                        get_part(height, 0.373), listener.create_new_game)

        self.add_button("", get_part(width, 0.29), get_part(height, 0.1555), get_part(width, 0.35555),
                        get_part(height, 0.607), listener.find_games)

        self.add_button("", get_part(width, 0.0929), get_part(height, 0.1041), get_part(width, 0.01570),
                        get_part(height, 0.0277), listener.close_the_game)

    def add_button(self, text, width, height, pos_x, pos_y, callback):
        button = ViewUtils.init_button(width, height, pos_x, pos_y, callback)
        button.setText(text)
        button.setParent(self)
