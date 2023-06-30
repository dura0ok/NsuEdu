#pragma once
#include <cstring>
#include "Converter.h"
#include "../WavWriter.h"

namespace Converters {

class Mute : public Converter {
 public:
  [[nodiscard]] size_t getNewSampleSize(InputStream &input) override;

  void setConfig(const std::vector<std::string> &argsVector) override;

  [[nodiscard]] std::string_view getConverterDescription() const override {
	return "Mute converter.\nIt accepts two parameters(start stop seconds)\nExample how to use: mute 10 20";
  }

  void transformSound(InputStream &input, OutputStream &output) override;

 private:
  size_t from;
  size_t to;

};

}