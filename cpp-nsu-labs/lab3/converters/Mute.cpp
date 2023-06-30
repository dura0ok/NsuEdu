#include "Mute.h"

void Converters::Mute::setConfig(const std::vector<std::string> &argsVector) {
  if (argsVector.size()!=2) {
	throw InvalidInput("Invalid arguments getSamplesCount for mute converter. It should be 2(from, to)");
  }

  from = std::stoi(argsVector[0]);
  to = std::stoi(argsVector[1]);
}

void Converters::Mute::transformSound(InputStream &input, OutputStream &output) {

  to *= input.getCountSamplesInOneSecond();
  from *= input.getCountSamplesInOneSecond();

  for (size_t i = 0; i < input.getSamplesCount(); i++) {
	auto sample = input.readSample();

	if (i >= from && i <= to) {
	  sample = 0;
	}

	output.writeSample(sample);
  }
}

size_t Converters::Mute::getNewSampleSize(InputStream &input) {
  return input.getSamplesCount() * 2;
}
