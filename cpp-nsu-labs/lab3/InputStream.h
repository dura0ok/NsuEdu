#pragma once
#include <iostream>
#include "CustomExceptions.h"

class InputStream {
 public:
  [[nodiscard]] virtual size_t getSamplesCount() const = 0;
  [[nodiscard]] virtual int16_t readSample() = 0;

  virtual void prepare() {
	throw NotImplemented("Not implemented.");
  }

  [[nodiscard]] virtual size_t getCountSamplesInOneSecond() const {
	throw NotImplemented("Not implemented.");
  }

  [[nodiscard]] virtual size_t getSampleRate() const {
	throw NotImplemented("Not implemented.");
  }

  virtual ~InputStream() = default;
};