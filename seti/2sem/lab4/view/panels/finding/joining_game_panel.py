from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtWidgets import QWidget, QPushButton, QVBoxLayout, QScrollArea

from view.panels.finding.game_line_panel import GameLinePanel
from view.utils import get_part


class JoiningGamePanel(QWidget):
    def __init__(self, listener, width, height):
        super().__init__()

        self.listener = listener
        self.gamePanelsMap = {}
        self.chosenGame = None
        self.gameLineWidth = get_part(width * 1.025, 0.6)
        self.gameLineHeight = get_part(height, 0.1)

        self.timer = QTimer()
        self.game_list = []
        self.timer.timeout.connect(self.update_scroll)
        self.timer.start(1000)

        layout = QVBoxLayout(self)

        layout.addWidget(self.create_button("Close", self.listener.close_the_game, get_part(width, 0.0929),
                                            get_part(height, 0.1041), get_part(width, 0.01570),
                                            get_part(height, 0.0277)))
        layout.addWidget(self.create_button("Back", self.listener.back_to_start_menu, get_part(width, 0.11),
                                            get_part(height, 0.082), get_part(width, 0.009), get_part(height, 0.16)))
        layout.addWidget(self.create_button("Join Game", self.join_the_game, get_part(width, 0.093),
                                            get_part(height, 0.15), get_part(width, 0.879), get_part(height, 0.8)))

        self.scrollPane = self.create_scroll_area(self.gameLineWidth, self.gameLineHeight * 7, get_part(width, 0.2),
                                                  get_part(height, 0.25))
        layout.addWidget(self.scrollPane)

    def create_button(self, text, function, pos_x, pos_y, width, height):
        button = QPushButton(text, self)
        button.setGeometry(pos_x, pos_y, width, height)
        button.clicked.connect(function)
        return button

    def create_scroll_area(self, width, height, pos_x, pos_y):
        scroll_area = QScrollArea(self)
        scroll_area.setGeometry(pos_x, pos_y, width, height)
        scroll_area.setWidgetResizable(True)
        scroll_area.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)

        self.content_widget = QWidget()
        self.content_widget.setFixedSize(300, 700)
        scroll_area.setWidget(self.content_widget)

        self.inner_layout = QVBoxLayout(self.content_widget)
        self.inner_layout.setAlignment(Qt.AlignTop)

        return scroll_area

    def join_the_game(self):
        if self.chosenGame is not None:
            self.listener.joining_to_game(self.chosenGame)

    def choose_game(self, name):
        print('frhgdnf')
        if self.chosenGame is not None:
            if self.chosenGame == name:
                return
            self.gamePanelsMap[self.chosenGame].make_default_background()
        self.chosenGame = name
        print(self.chosenGame)

    def update_user_list(self, game_list):
        self.game_list = game_list

    def update_scroll(self):
        is_chosen_any_dialog = False

        for child in self.content_widget.findChildren(QWidget):
            child.deleteLater()
            del child

        number = 0
        for data in self.game_list:
            data = data[:-2]
            game_line_panel = GameLinePanel(self, "resources/GameLine.png", self.gameLineWidth, self.gameLineHeight, 0,
                                            self.gameLineHeight * number, data)

            if self.chosenGame is None or self.chosenGame == data:
                self.chosenGame = data
                print(self.chosenGame)
                game_line_panel.make_chosen_background()
                is_chosen_any_dialog = True
            self.gamePanelsMap[data] = game_line_panel
            self.content_widget.layout().addWidget(game_line_panel)
            number += 1
        if not is_chosen_any_dialog and self.game_list:
            chosen_game = self.game_list[0]
            self.chosenGame = chosen_game
            self.gamePanelsMap[chosen_game].make_chosen_background()

        # self.scrollPane.verticalScrollBar().setValue(0)
