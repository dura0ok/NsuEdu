#pragma once
#include <cstdint>

constexpr size_t UINT32_SIZE = sizeof(uint32_t);
inline size_t DECLARED_BYTES_PER_SAMPLE = 2;

#pragma pack(push, 1)

typedef enum ChunkTypes {
  CT_fmt = 0x20746d66,
  CT_WAVE = 0x45564157,
  CT_RIFF = 0x46464952,
  CT_data = 0x61746164,
} ChunkTypes;

struct ChunkInfo {
  uint32_t chunkId;
  uint32_t chunkSize;
};

/*
The RIFF header!
*/
struct WavRiffHeader {
  ChunkInfo riff;
  char format[4]; // "WAV"
};

/*
The FMT header!
*/
struct WavFmtHeader {
  //"fmt "
  //16 (PCM)
  uint16_t audioFormat; // 1 'cause PCM
  uint16_t numChannels; // mono = 1; stereo = 2
  uint32_t sampleRate; // 8000, 44100, etc.
  uint32_t byteRate; //== SampleRate * NumChannels * byte
  uint16_t blockAlign; //== NumChannels * bytePerSample
  uint16_t bitsPerSample; //8 byte = 8, 16 byte = 16, etc.
};

#pragma pack(pop)