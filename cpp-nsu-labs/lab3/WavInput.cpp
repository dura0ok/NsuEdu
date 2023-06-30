#include <istream>
#include <utility>
#include "WavInput.h"
#include "WavFormat.h"
#include "WavParser.h"
#include "CustomExceptions.h"
#include "ConfigParser.h"

int16_t WavInput::readSample() {
  if (in.eof()) {
	throw InvalidFile("Samples count incorrect");
  }

  int16_t sample;

  in.read(reinterpret_cast<char *>(&sample), static_cast<std::streamsize>(DECLARED_BYTES_PER_SAMPLE));

  if (static_cast<size_t>(in.gcount()) < DECLARED_BYTES_PER_SAMPLE) {
	throw InvalidFile("Something went wrong. Trying to read sample, but sample size < declaredBytesPerSample");
  }

  return sample;
}

WavInput::WavInput(const std::filesystem::path &inputFileName) {
  info = parseWavFile(inputFileName);
  in.open(inputFileName, std::fstream::binary);
  if (in.fail()) {
	throw IOException("file cant`t opened: " + inputFileName.string());
  }
}
size_t WavInput::getSamplesCount() const {
  return info.samplesCount;
}

size_t WavInput::getCountSamplesInOneSecond() const {
  size_t secondsLengthFile = info.samplesCount/info.fmtHeader.sampleRate;
  return info.samplesCount/secondsLengthFile;
}

size_t WavInput::getSampleRate() const {
  return info.fmtHeader.sampleRate;
}
void WavInput::prepare() {
  in.seekg(info.dataPosition, std::fstream::beg);
}

const ParsedWavInfo &WavInput::getInfo() const {
  return info;
}

std::string parseDashedArg(std::string arg) {
  if (arg[0] != FILE_SIGN) {
	throw InvalidInput("Dashed arg must be start with $.");
  }
  arg.erase(0, 1);
  return arg;
}

std::unique_ptr<InputStream> createInputStreamFromString(std::string arg) {
  auto fileName = parseDashedArg(std::move(arg));
  return std::make_unique<WavInput>(fileName);
}