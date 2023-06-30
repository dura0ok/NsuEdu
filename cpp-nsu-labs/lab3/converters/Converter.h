#pragma once
#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include "../WavParser.h"
#include "../WavWriter.h"
#include "../InputStream.h"
#include "../OutputStream.h"

namespace Converters {

class Converter {
 public:
  [[nodiscard]] virtual size_t getNewSampleSize(InputStream &input) = 0;

  virtual void transformSound(InputStream &input, OutputStream &output) = 0;
  [[nodiscard]] virtual std::string_view getConverterDescription() const = 0;
  virtual void setConfig(const std::vector<std::string> &argsVector) = 0;
  virtual ~Converter() = default;

};

}

