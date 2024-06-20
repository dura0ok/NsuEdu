import traceback

from PyQt5.QtCore import Qt, QSize
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QGridLayout
from PyQt5.QtCore import QTimer

from view.panels.game.field_cells_panel import FieldCellsPanel
from view.panels.game.score_line_panel import ScoreLinePanel
from view.panels.window_panel import WindowPanel
from view.view_utils import ViewUtils
from view.utils import *
from lib.snakes import NodeRole


class GamePanel(WindowPanel):
    def __init__(self, listener, width_panel, height_panel, width_field, height_field):
        try:
            super().__init__('resources/Field.png', width_panel, height_panel)
            self.listener = listener
            self.node_role = NodeRole.NORMAL
            self.width = width_panel
            self.height = height_panel
            self.field_width = width_field
            self.field_height = height_field
            self.score_line_width = get_part(width_panel, 0.2)
            self.score_line_height = get_part(height_panel, 0.1)
            self.scores_table = []
            self.change_role_on_screen = False
            self.uptimer = QTimer()
            self.uptimer.timeout.connect(self.update_scores_table)

            cell_size = min(int(self.width * 0.6), int(self.height*0.8)) // max(width_field, height_field)
            self.field_cells_panel = FieldCellsPanel(cell_size, self.field_width, self.field_height)
            self.field_cells_panel.setParent(self)

            self.node_role_label = ViewUtils.init_label(
                text=str(self.node_role),
                font_size=get_part(height_panel, 0.018),
                width=get_part(width_panel, 0.1),
                height=get_part(height_panel, 0.05),
                pos_x=get_part(width_panel, 0.93),
                pos_y=get_part(height_panel, 0.0001)
            )
            self.node_role_label.setParent(self)

            button_width = get_part(width_panel, 0.19)
            button_height = get_part(height_panel, 0.1)
            self.change_role_on_viewer = ViewUtils.init_button(
                width=button_width,
                height=button_height,
                pos_x=get_part(width_panel, 0.8),
                pos_y=get_part(height_panel, 0.75),
                listener=self.listener.change_role_on_viewer
            )
            self.change_role_on_viewer.setIcon(ViewUtils.get_image_button_icon(
                file_directory="resources/BecomeAViewer.png",
                color_for_button=Qt.green,
                width=button_width,
                height=button_height
            ))
            self.change_role_on_viewer.setIconSize(QSize(button_width, button_height))
            self.change_role_on_viewer.setParent(self)

            self.leave_the_game = ViewUtils.init_button(
                width=button_width,
                height=button_height,
                pos_x=get_part(width_panel, 0.797),
                pos_y=get_part(height_panel, 0.885),
                listener=self.listener.leave_the_game
            )
            self.leave_the_game.setParent(self)
            self.change_role_on_screen = False

            self.scroll_pane = ViewUtils.init_scroll_area(
                width=int(self.score_line_width * 1.025),
                height=self.score_line_height * 4,
                pos_x=get_part(width_panel, 0.7),
                pos_y=get_part(height_panel, 0.10)
            )
            self.scroll_pane.setParent(self)
            self.scroll_pane_w = QWidget()
            self.lines = []
            self.scroll_pane.repaint()
            self.uptimer.start(2000)
        except Exception as ex: print(ex, 'game_pan 84')

    def update_scores_table(self):
        games_panel = QWidget()
        games_panel.setFixedSize(self.score_line_width, self.score_line_height * len(self.scores_table))

        layout = QVBoxLayout(games_panel)
        layout.setContentsMargins(0, 0, 0, 0)

        for number, data in enumerate(self.scores_table):
            score_line_panel = ScoreLinePanel(
                filename="GameLine.png",
                width=self.score_line_width,
                height=self.score_line_height,
                pos_x=0,
                pos_y=self.score_line_height * number,
                data=str(data)
            )
            layout.addWidget(score_line_panel)
        self.scroll_pane.setWidget(games_panel)
        self.scroll_pane.repaint()



    def update_role(self, node_role):
        self.node_role_label.setText(str(node_role)[9:])
        print('here')
        if node_role != NodeRole.VIEWER:
            if not self.change_role_on_screen:
                self.change_role_on_viewer.setParent(self)
                self.change_role_on_screen = True
        elif self.change_role_on_screen:
            self.change_role_on_viewer.setParent(None)
            self.change_role_on_screen = False

    def update_game_panel(self, field, scores_table, node_role):
        if self.node_role != node_role:
            self.node_role = node_role
            self.update_role(node_role)
        self.field_cells_panel.update_field(field)
        self.scores_table = scores_table
