#include <charconv>
#include "Echo.h"
const inline uint16_t UPPER_LIMIT = 32767;
const inline int16_t DOWN_LIMIT = -32768;

void Converters::Echo::setConfig(const std::vector<std::string> &argsVector) {
  if (argsVector.size()!=3) {
	throw InvalidInput(
		"Invalid arguments getSamplesCount for echo converter. It should be 3(start, stop, delay sec)");
  }

  auto res = std::from_chars(argsVector[2].data(), argsVector[2].data() + argsVector[2].size(), delaySecs);
  if (res.ec==std::errc::invalid_argument) {
	throw InvalidInput("Invalid delay secs argument.");
  }
  from = std::stoi(argsVector[0]);
  to = std::stoi(argsVector[1]);

}

void Converters::Echo::transformSound(InputStream &input, OutputStream &output) {
  from *= input.getCountSamplesInOneSecond();
  to *= input.getCountSamplesInOneSecond();

  auto delaySamples = static_cast<size_t>(static_cast<float>(input.getSampleRate())*delaySecs)/10;

  auto ringBuffer = std::make_unique<int16_t[]>(delaySamples);
  std::memset(ringBuffer.get(), 0, sizeof(int16_t)*delaySamples);

  for (size_t i = 0; i < input.getSamplesCount(); i++) {
	auto sample = static_cast<double>(input.readSample());

	if (i >= from && i <= to) {
	  sample += static_cast<double>(ringBuffer[i%delaySamples])/2;
	  sample = (sample < DOWN_LIMIT) ? DOWN_LIMIT : (sample > UPPER_LIMIT) ? UPPER_LIMIT : sample;
	  ringBuffer[i%delaySamples] = static_cast<int16_t>(sample);
	}

	output.writeSample(static_cast<int16_t>(sample));
  }
}

size_t Converters::Echo::getNewSampleSize(InputStream &input) {
  return input.getSamplesCount() * 2;
}

