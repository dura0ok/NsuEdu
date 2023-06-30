#pragma once
#include <cstring>
#include <memory>
#include "Converter.h"
#include "../WavWriter.h"

namespace Converters {

class Mix : public Converter {
 public:
  [[nodiscard]] size_t getNewSampleSize(InputStream &input) override;

  void setConfig(const std::vector<std::string> &argsVector) override;

  [[nodiscard]] std::string_view getConverterDescription() const override {
	return "Mix converter.\nIt accepts 3 parameters(number file to mix, start, stop seconds)\nExample how to use: mix $2 10";
  }

  void transformSound(InputStream &input, OutputStream &output) override;
 private:
  std::unique_ptr<InputStream> mixingFile;
  size_t fromPoint;
};

}