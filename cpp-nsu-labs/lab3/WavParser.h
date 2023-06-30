#pragma once
#include <fstream>
#include <filesystem>
#include "WavFormat.h"


#pragma pack(push, 1)
struct ParsedWavInfo {
  WavRiffHeader riffHeader;
  ChunkInfo fmtChunkInfo;
  WavFmtHeader fmtHeader;
  uint32_t dataSize; // NumSamples * NumChannels * BitsPerSample/8
  std::streampos dataPosition;
  size_t samplesCount;
  uint32_t bytesPerSample;
};

#pragma pack(pop)

[[nodiscard]] ParsedWavInfo parseWavFile(const std::filesystem::path &inputFileName);

