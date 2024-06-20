from view.panels.window_panel import WindowPanel
from view.utils import get_part
from view.view_utils import ViewUtils


class ScoreLinePanel(WindowPanel):
    def __init__(self, filename, width, height, pos_x, pos_y, data):
        super().__init__(f'/{filename}', width, height)

        self.filename = filename

        size = self.sizeHint()
        self.setGeometry(pos_x, pos_y, size.width(), size.height())

        name = ViewUtils.init_label(data, get_part(height, 0.32), width,
                                    get_part(height, 0.5), get_part(height, 0.2),
                                    get_part(height, 0.25))
        name.setParent(self)