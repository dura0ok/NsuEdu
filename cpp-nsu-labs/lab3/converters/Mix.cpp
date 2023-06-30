#include "Mix.h"
#include "../WavInput.h"
#include "../CustomExceptions.h"

void Converters::Mix::setConfig(const std::vector<std::string> &argsVector) {
  auto args = argsVector;

  if (args.empty()) {
	throw InvalidInput("Invalid arguments getSamplesCount for mix converter. It should be minimum 1");
  }

  if (args.size() < 2) {
	args.emplace_back("0");
  }

  mixingFile = createInputStreamFromString(args[0]);
  fromPoint = std::stoi(args[1]);
}

void Converters::Mix::transformSound(InputStream &input, OutputStream &output) {
  fromPoint *= input.getCountSamplesInOneSecond();
  for (size_t i = 0; i < input.getSamplesCount(); i++) {
	int16_t sample = input.readSample();

	if (i >= fromPoint) {
	  try {
		auto mixSample = mixingFile->readSample();
		sample = static_cast<int16_t>((sample + mixSample)/2);
	  }
	  catch (const InvalidFile &e) {}
	  catch (const std::exception &e) {
		throw InvalidFile(e.what());
	  }

	}

	output.writeSample(sample);
  }
}

size_t Converters::Mix::getNewSampleSize(InputStream &input) {
  return input.getSamplesCount() * 2;
}

