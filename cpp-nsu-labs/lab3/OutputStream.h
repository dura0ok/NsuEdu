#pragma once
#include "WavWriter.h"

class OutputStream {
 public:
  virtual void writeSample(int16_t sample) = 0;
};