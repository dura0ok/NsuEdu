#include <iostream>
#include <cstring>
#include <QApplication>
#include "argparse/argparse.hpp"
#include "ConfigParser.h"
#include "helpers/LineHelper.h"
#include "converters/ConverterFactory.h"
#include "WavInput.h"
#include "CustomExceptions.h"
#include "MainWindow.h"

std::string convertersInfoToString(const Converters::ConverterFactory &factory) {
  std::string res;
  auto infoMap = factory.getElementsInfo();
  for (auto const &key : infoMap) {
	res += std::string(key.first) + "\n" + std::string("Description: ") + key.second + "\n\n";
  }
  return res;
}

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("SoundProcessor");
  auto tempFactory = Converters::ConverterFactory();
  auto info = convertersInfoToString(tempFactory);

  program.add_argument("-cmd", "--nogui")
	  .help("no gui mode")
	  .default_value(false)
	  .implicit_value(true);

  program.add_argument("-c", "--config")
	  .help("config converters file");

  program.add_argument("files")
	  .nargs(argparse::nargs_pattern::at_least_one)
	  .remaining();

  program.add_description("Possible converters:\n\n" + info);

  try {
	program.parse_args(argc, argv);
	if (program.get<bool>("-cmd")) {

	  auto const &configFileName = program.get<std::string>("--config");
	  auto files = program.get<std::vector<std::string>>("files");

	  auto configParser = ConfigParser(configFileName);
	  auto parsedLines = configParser.parseConfig(files);
	  auto converterFactory = Converters::ConverterFactory();
	  auto convertersPipeline = converterFactory.createAllConvertersFromConfig(parsedLines);

	  if (files.size() < 2) {
		throw InvalidInput("Files count must be >= 2, (output and minimum 1 input file)");
	  }

	  std::string previousName = files[1];
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

	  std::filesystem::rename(currentName, files[0]);
	}else{
	  QApplication a(argc, argv);

	  MainWindow w;
	  w.show();
	  return QApplication::exec();
	}
  }

  catch (const SoundProcessorException &e) {
	std::cerr << e.what() << '\n';
	return e.getCode();
  }
  catch (const std::exception &e) {
	std::cerr << e.what() << '\n';
	return UNHANDLED_ERROR;
  }

  return NO_ERROR;
}