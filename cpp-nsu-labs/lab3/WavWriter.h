#pragma once
#include <filesystem>
#include "WavParser.h"
#include "OutputStream.h"

class WavWriter : public OutputStream {
 public:
  explicit WavWriter(const std::filesystem::path &outName);
  void writeWavHeader(const ParsedWavInfo &info);
  void writeWavDataChunkInfo(uint32_t dataSize);
  void seekToDataChunk(std::streampos pos);
  void writeHeader(ParsedWavInfo info);
  void writeSample(int16_t sample) override;
 private:
  template<typename T>
  void write(T value) {
	out.write(reinterpret_cast<const char *>(&value), sizeof(T));
  }
  std::fstream out;
};
