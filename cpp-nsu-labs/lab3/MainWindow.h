#pragma once
#include <vector>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
 Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

 private:
  Ui::MainWindow *ui;
  void on_pushButtonFiles_clicked();
  void on_pushButtonConfig_clicked();
  void on_pushButton_clicked();
  std::vector<std::string> inputFileNames;
  std::string configFileName;
};
