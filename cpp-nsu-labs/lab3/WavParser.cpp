#include <iostream>
#include <cstring>
#include "WavParser.h"
#include "CustomExceptions.h"

const inline uint32_t SUPPORT_SAMPLE_RATE = 44100;
const inline uint32_t SUPPORT_AUDIO_FORMAT = 1;
const inline uint32_t SUPPORT_NUM_CHANNELS = 1;
const inline uint32_t SUPPORT_BITS_PER_SAMPLE = 16;
const inline uint32_t BYTE_SIZE = 4;
const inline char WAVE_FORMAT[] = "WAVE";

WavRiffHeader parseRiffChunk(std::ifstream &rawWavFile);
void validateFmtChunk(WavFmtHeader header);

void read(std::istream &f, void *buffer, std::streamsize size) {
  f.read(static_cast<char *>(buffer), size);
}

void read(std::istream &f, ChunkInfo &c) {
  read(f, &c.chunkId, BYTE_SIZE);
  read(f, &(c.chunkSize), BYTE_SIZE);
}

std::string convert4ByteUIntToString(uint32_t n) {
  return {
	  char(n & 0xFF),
	  char((n >> 8) & 0xFF),
	  char((n >> 16) & 0xFF),
	  char((n >> 24) & 0xFF)
  };
}

WavRiffHeader parseRiffChunk(std::ifstream &rawWavFile) {
  auto riff = WavRiffHeader{};
  read(rawWavFile, riff.riff);

  if (riff.riff.chunkId != CT_RIFF) {
	throw InvalidFile("First chunk must be RIFF");
  }

  rawWavFile.read(static_cast<char *>(riff.format), UINT32_SIZE);

  if (std::memcmp(WAVE_FORMAT, riff.format, BYTE_SIZE) != 0) {
	throw InvalidFile("Riff format must be WAVE");
  }

  return riff;

}

ParsedWavInfo parseWavFile(const std::filesystem::path &inputFileName) {
  auto rawWavFile = std::ifstream(inputFileName, std::fstream::in | std::fstream::binary);
  if (rawWavFile.fail()) {
	throw IOException("can`t opened file: " + inputFileName.string());
  }
  auto chunk = ChunkInfo{};
  auto res = ParsedWavInfo{};

  auto fmtChunk = WavFmtHeader{};
  bool isFmtFound = false;

  res.riffHeader = parseRiffChunk(rawWavFile);

  while (rawWavFile) {
	read(rawWavFile, chunk);
	uint32_t next_chunk = static_cast<size_t>(rawWavFile.tellg()) + chunk.chunkSize + (chunk.chunkSize & 1);

	if (chunk.chunkId == CT_fmt) {
	  res.fmtChunkInfo = chunk;
	  rawWavFile.read(reinterpret_cast<char *>(&fmtChunk), sizeof(fmtChunk));
	  isFmtFound = true;
	}

	if (chunk.chunkId == CT_data) {
	  res.dataPosition = rawWavFile.tellg();
	  res.dataSize = chunk.chunkSize;
	  break;
	}

	rawWavFile.seekg(next_chunk);
  }

  if (!isFmtFound) {
	throw InvalidFile("fmt not found");
  }

  validateFmtChunk(fmtChunk);
  res.fmtHeader = fmtChunk;

  res.bytesPerSample = res.fmtHeader.bitsPerSample / 8;
  res.samplesCount = res.dataSize / (res.bytesPerSample);
  return res;
}

void validateFmtChunk(WavFmtHeader header) {
  if (header.numChannels != SUPPORT_NUM_CHANNELS) {
	throw InvalidFile(
		"Invalid num channels. We support only " + std::to_string(SUPPORT_NUM_CHANNELS) + " channels in wav file.");
  }

  if (header.audioFormat != SUPPORT_AUDIO_FORMAT) {
	throw InvalidFile("Invalid audioFormat. We support only PCM");
  }

  if (header.sampleRate != SUPPORT_SAMPLE_RATE) {
	throw InvalidFile("Invalid SampleRate. We support only " + std::to_string(SUPPORT_SAMPLE_RATE));
  }

  if (header.bitsPerSample != SUPPORT_BITS_PER_SAMPLE) {
	throw InvalidFile("Invalid bitsPerSample. We support only " + std::to_string(SUPPORT_BITS_PER_SAMPLE));
  }

}
