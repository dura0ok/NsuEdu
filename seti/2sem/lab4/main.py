import sys
from PyQt5.QtWidgets import QApplication
from model.presenter import Presenter
import view.windows.main_window as t
import faulthandler


def main():
    app = QApplication(sys.argv)
    view = t.MainGuiWindow()
    presenter = Presenter(view)
    presenter.launch_ui()
    app.exec()


if __name__ == "__main__":
    def my_exception_hook(exctype, value, traceback):
        print(exctype, value, traceback)
        sys._excepthook(exctype, value, traceback)
        sys.exit(1)
    sys._excepthook = sys.excepthook
    sys.excepthook = my_exception_hook
    faulthandler.enable()


    main()
