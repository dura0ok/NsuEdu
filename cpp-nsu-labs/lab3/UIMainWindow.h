/********************************************************************************
** Form generated from reading UI file 'asder.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ASDER_H
#define UI_ASDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow {
 public:
  QWidget *centralwidget;
  QGridLayout *gridLayout;
  QSpacerItem *verticalSpacer_2;
  QPushButton *filesPushButton;
  QSpacerItem *verticalSpacer;
  QPushButton *configPushButton;
  QSpacerItem *verticalSpacer_3;
  QLabel *config;
  QLineEdit *outName;
  QLabel *files;
  QPushButton *pushButton;
  QMenuBar *menubar;
  QMenu *menuSound_Processor;
  QStatusBar *statusbar;

  void setupUi(QMainWindow *MainWindow) {
	if (MainWindow->objectName().isEmpty())
	  MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
	MainWindow->resize(1012, 516);
	centralwidget = new QWidget(MainWindow);
	centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
	gridLayout = new QGridLayout(centralwidget);
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	gridLayout->addItem(verticalSpacer_2, 3, 0, 1, 1);

	filesPushButton = new QPushButton(centralwidget);
	filesPushButton->setObjectName(QString::fromUtf8("filesPushButton"));

	gridLayout->addWidget(filesPushButton, 0, 0, 1, 1);

	verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	gridLayout->addItem(verticalSpacer, 5, 2, 1, 1);

	configPushButton = new QPushButton(centralwidget);
	configPushButton->setObjectName(QString::fromUtf8("configPushButton"));

	gridLayout->addWidget(configPushButton, 2, 0, 1, 1);

	verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	gridLayout->addItem(verticalSpacer_3, 1, 0, 1, 1);

	config = new QLabel(centralwidget);
	config->setObjectName(QString::fromUtf8("config"));

	gridLayout->addWidget(config, 2, 1, 1, 2);

	outName = new QLineEdit(centralwidget);
	outName->setObjectName(QString::fromUtf8("outName"));

	gridLayout->addWidget(outName, 4, 0, 1, 3);

	files = new QLabel(centralwidget);
	files->setObjectName(QString::fromUtf8("files"));

	gridLayout->addWidget(files, 0, 1, 1, 2);

	pushButton = new QPushButton(centralwidget);
	pushButton->setObjectName(QString::fromUtf8("pushButton"));
	QFont font;
	font.setPointSize(15);
	pushButton->setFont(font);
	pushButton->setStyleSheet(QString::fromUtf8("background:  rgb(46,139,87);\n"
												"border: none;\n"
												"padding: 10px;\n"
												"color:  rgb(248,248,255);\n"
												"border-radius: 10px;"));

	gridLayout->addWidget(pushButton, 6, 2, 1, 1);

	MainWindow->setCentralWidget(centralwidget);
	menubar = new QMenuBar(MainWindow);
	menubar->setObjectName(QString::fromUtf8("menubar"));
	menubar->setGeometry(QRect(0, 0, 1012, 22));
	menuSound_Processor = new QMenu(menubar);
	menuSound_Processor->setObjectName(QString::fromUtf8("menuSound_Processor"));
	MainWindow->setMenuBar(menubar);
	statusbar = new QStatusBar(MainWindow);
	statusbar->setObjectName(QString::fromUtf8("statusbar"));
	MainWindow->setStatusBar(statusbar);

	menubar->addAction(menuSound_Processor->menuAction());

	retranslateUi(MainWindow);

	QMetaObject::connectSlotsByName(MainWindow);
  } // setupUi

  void retranslateUi(QMainWindow *MainWindow) {
	MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
	filesPushButton->setText(QCoreApplication::translate("MainWindow", "input files(choose)", nullptr));
	configPushButton->setText(QCoreApplication::translate("MainWindow", "choose config", nullptr));
	config->setText(QString());
	outName->setInputMask(QString());
	outName->setText(QCoreApplication::translate("MainWindow", "Input out input fileName", nullptr));
	files->setText(QString());
	pushButton->setText(QCoreApplication::translate("MainWindow", "Convert", nullptr));
	menuSound_Processor->setTitle(QCoreApplication::translate("MainWindow", "Sound Processor", nullptr));
  } // retranslateUi

};

namespace Ui {
class MainWindow : public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ASDER_H
