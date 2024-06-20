import sys
import time
import traceback

from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtCore import Qt

from lib import NodeRole
from model import presenter
from view.panels.creating.configuration.configuration_settings_panel import ConfigurationSettingsPanel
from view.panels.creating.creating_game_panel import CreatingGamePanel
from view.panels.finding.finding_game_panel import FindingGamePanel
from view.panels.finding.joining_game_panel import JoiningGamePanel
from view.panels.game.game_panel import GamePanel
from view.panels.start_menu.start_panel import StartPanel


class MainGuiWindow(QMainWindow):
    def __init__(self):

        super().__init__()
        self.NAME = "Snakes"
        self.MENU = "Menu"
        self.widthWindow = 860
        self.heightWindow = self.widthWindow // 16 * 9

        self.widthField = 0
        self.heightField = 0

        self.presenter = None

        self.setCentralWidget(StartPanel(self, self.widthWindow, self.heightWindow))
        self.setFocusPolicy(Qt.StrongFocus)
        self.setFixedSize(self.widthWindow, self.heightWindow)
        self.settings = {'width_game_field': '40', 'height_game_field': '30', 'food_static_field': '1', 'food_per_player_field': '1', 'state_delay_field': '200', 'dead_food_prob_field': '0.1', 'ping_delay_field': '100', 'node_timeout_field': '800'}

    def set_content_on_frame(self, widget):
        self.setCentralWidget(widget)
        self.repaint()
        self.show()

    def launch_finding_game_module(self):
        try:
            self.joiningGamePanel = JoiningGamePanel(self, self.widthWindow, self.heightWindow)
            self.set_content_on_frame(self.joiningGamePanel)
            self.presenter.find_the_game(self.findingGamePanel.name, self.findingGamePanel.port,
                                       self.findingGamePanel.player_type)
        except Exception as ex: print(traceback.format_exc(), 'main_window 55')

    def joining_to_game(self, gameKey):
        # if self.findingGamePanel.is_viewer:
        #     self.presenter.joinTheGame(NodeRole.VIEWER, gameKey)
        # else:
        self.presenter.join_the_game(NodeRole.NORMAL, gameKey)

    def open_field(self, widthField, heightField):
        self.gamePanel = GamePanel(self, self.widthWindow, self.heightWindow, widthField, heightField)
        self.set_content_on_frame(self.gamePanel)
        self.widthField = widthField
        self.heightField = heightField

    def close_the_game(self):
        self.presenter.endTheSession()
        sys.exit(0)

    def back_to_creating_menu(self):
        self.creatingGamePanel = CreatingGamePanel(self, self.widthWindow, self.heightWindow)
        self.set_content_on_frame(self.creatingGamePanel)

    def back_to_start_menu(self):
        self.set_content_on_frame(StartPanel(self, self.widthWindow, self.heightWindow))

    def create_new_game(self):
        self.creatingGamePanel = CreatingGamePanel(self, self.widthWindow, self.heightWindow)
        self.set_content_on_frame(self.creatingGamePanel)

    def find_games(self):
        self.findingGamePanel = FindingGamePanel(self, self.widthWindow, self.heightWindow)
        self.set_content_on_frame(self.findingGamePanel)

    def attach_presenter(self, presenter):
        self.presenter = presenter

    def change_visible(self, var):
        self.setVisible(var)

    def update_game_view(self, fieldString, scoresTable, nodeRole):
        self.gamePanel.update_game_panel(fieldString, scoresTable, nodeRole)

    def update_find_game_list(self, games):
        self.joiningGamePanel.update_user_list(games)

    def start_the_game(self):
        self.open_field(int(self.settings['width_game_field']), int(self.settings['height_game_field']))
        self.presenter.start_the_game(
            self.settings['name_field'],
            int(self.settings['port_field']),
            self.settings['player_type']
    )

    def open_config_settings(self):
        self.configurationSettingsPanel = ConfigurationSettingsPanel(self, self.widthWindow, self.heightWindow)
        self.set_content_on_frame(self.configurationSettingsPanel)

    def change_role_on_viewer(self):
        self.presenter.change_role_on_viewer()

    def leave_the_game(self):
        self.presenter.leave_the_game()
        self.back_to_start_menu()

    def save_settings(self, settings):
        self.settings = settings

    def keyPressEvent(self, e):
        if e.key() == Qt.Key_W:
            self.presenter.make_up_move()
        elif e.key() == Qt.Key_A:
            self.presenter.make_left_move()
        elif e.key() == Qt.Key_S:
            self.presenter.make_down_move()
        elif e.key() == Qt.Key_D:
            self.presenter.make_right_move()
