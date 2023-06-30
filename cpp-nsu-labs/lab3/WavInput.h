#pragma once
#include <iostream>
#include <filesystem>
#include <istream>
#include <fstream>
#include <memory>
#include "InputStream.h"
#include "WavParser.h"

class WavInput : public InputStream {
 public:
  explicit WavInput(const std::filesystem::path &inputFileName);
  [[nodiscard]] int16_t readSample() override;
  [[nodiscard]] size_t getSamplesCount() const override;
  void prepare() override;
  [[nodiscard]] size_t getCountSamplesInOneSecond() const override;
  [[nodiscard]] size_t getSampleRate() const override;
  const ParsedWavInfo &getInfo() const;

 private:
  std::ifstream in;
  ParsedWavInfo info{};
};

std::unique_ptr<InputStream> createInputStreamFromString(std::string arg);