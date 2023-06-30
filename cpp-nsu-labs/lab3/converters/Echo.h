#pragma once
#include <cstring>
#include "Converter.h"
#include "../WavWriter.h"

namespace Converters {

class Echo : public Converter {

  [[nodiscard]] size_t getNewSampleSize(InputStream &input) override;

  void setConfig(const std::vector<std::string> &argsVector) override;

  [[nodiscard]] std::string_view getConverterDescription() const override {
	return "Echo converter.\nIt accepts 3 parameters(number file to mix, start, stop seconds)\nExample how to use: echo 10 20 0.2";
  }

  void transformSound(InputStream &input, OutputStream &output) override;

 private:
  float delaySecs;
  size_t from;
  size_t to;
};

}