#include <iostream>
#include <QVector>
#include <vector>
#include <filesystem>
#include <memory>
#include <QSound>
#include <QFileDialog>
#include <QMessageBox>
#include "MainWindow.h"
#include "UIMainWindow.h"
#include "ConfigParser.h"
#include "helpers/LineHelper.h"
#include "WavInput.h"
#include "converters/ConverterFactory.h"
#include "CustomExceptions.h"

const inline std::string_view CORRECT_EXTENSION = ".wav";

[[nodiscard]] std::vector<std::string> QStringList_to_VectorString(const QList<QString> &qlist) {
  std::vector<std::string> result(qlist.size());
  for (int i = 0; i < qlist.size(); i++) {
	result[i] = qlist.at(i).toUtf8().data();
  }
  return result;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(ui->filesPushButton, &QPushButton::clicked, this, &MainWindow::on_pushButtonFiles_clicked);
  connect(ui->configPushButton, &QPushButton::clicked, this, &MainWindow::on_pushButtonConfig_clicked);
  connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::on_pushButtonFiles_clicked() {
  QStringList files = QFileDialog::getOpenFileNames(
	  this,
	  "Select one or more files to open");
  ui->files->setText(QString::fromStdString("Files choose: " + std::to_string(files.size())));
  QMessageBox::information(this, tr("File names"), files.join("\n"));
  inputFileNames = QStringList_to_VectorString(files);

}

void MainWindow::on_pushButtonConfig_clicked() {
  QString filename = QFileDialog::getOpenFileName(this, tr("Open file"));
  QMessageBox::information(this, tr("File name"), filename);
  ui->config->setText(filename);
  configFileName = filename.toStdString();

}
void MainWindow::on_pushButton_clicked() {
  try {
	auto outName = ui->outName->text();
	std::filesystem::path outPath(outName.toStdString());
	if(outPath.extension() != CORRECT_EXTENSION){
	  throw InvalidInput("Invalid extension for out file");
	}
	if (inputFileNames.empty() || configFileName.empty()) {
	  QMessageBox::information(this, tr("File name"), "inputFileNames or configFileName size");
	  return;
	}

	inputFileNames.insert(inputFileNames.begin(), outName.toStdString());

	auto configParser = ConfigParser(configFileName);
	auto parsedLines = configParser.parseConfig(inputFileNames);
	auto converterFactory = Converters::ConverterFactory();
	auto convertersPipeline = converterFactory.createAllConvertersFromConfig(parsedLines);

	if (inputFileNames.size() < 2) {
	  throw InvalidInput("Files count must be >= 2, (output and minimum 1 input file)");
	}

	QMessageBox::information(this, tr("Message"), "Процесс начался");
	std::string previousName = inputFileNames[1];
	std::string currentName;

	for (size_t i = 0; i < convertersPipeline.size(); i++) {
	  currentName = Helpers::generateUID() + ".wav";
	  auto wavInput = WavInput(previousName);
	  auto wavWriter = WavWriter(currentName);
	  auto inputInfo = wavInput.getInfo();

	  inputInfo.dataSize = convertersPipeline[i].get()->getNewSampleSize(wavInput);
	  wavWriter.writeHeader(inputInfo);
	  convertersPipeline[i]->transformSound(wavInput,
											wavWriter);
	  if (i > 0) {
		std::filesystem::remove(previousName);
	  }
	  previousName = currentName;
	}

	std::filesystem::rename(currentName, inputFileNames[0]);
	QMessageBox::information(this, tr("Success"), "Success converters operations!!! Please wait a little. Your New Wav file will play :)");
	QSound::play(QString::fromStdString(inputFileNames[0]));
  }
  catch (const std::exception &e) {
	std::cerr << e.what() << '\n';
	QMessageBox::information(this, tr("Error"), e.what());
  }
}

