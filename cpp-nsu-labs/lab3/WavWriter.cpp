#include <memory>
#include "WavWriter.h"
#include "CustomExceptions.h"
const uint32_t sizeWithoutData = 36;

WavWriter::WavWriter(const std::filesystem::path &outName) {
  out = std::fstream(outName, std::fstream::out | std::fstream::binary);
  if (out.fail()) {
	throw IOException("out file can`t opened");
  }
}

void WavWriter::writeWavHeader(const ParsedWavInfo &info) {
  write(info.riffHeader);
  write(info.fmtChunkInfo);
  write(info.fmtHeader);
}

void WavWriter::writeWavDataChunkInfo(uint32_t dataSize) {
  auto dataChunk = ChunkInfo{};
  dataChunk.chunkSize = dataSize;
  dataChunk.chunkId = CT_data;
  out.write(reinterpret_cast<const char *>(&dataChunk), sizeof(dataChunk));
}

void WavWriter::writeSample(int16_t sample) {
  out.write(reinterpret_cast<const char *>(&sample), 2);
}
void WavWriter::seekToDataChunk(std::streampos pos) {
  out.seekg(pos, std::fstream::beg);
}

void WavWriter::writeHeader(ParsedWavInfo info) {
  info.riffHeader.riff.chunkSize = static_cast<uint32_t>(info.samplesCount*info.bytesPerSample + sizeWithoutData);
  writeWavHeader(info);
  writeWavDataChunkInfo(info.dataSize);
  seekToDataChunk(info.dataPosition);
}



