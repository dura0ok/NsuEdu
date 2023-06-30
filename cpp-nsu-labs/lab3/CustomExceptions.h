#pragma once
#include <utility>

enum ErrorCode {
  NO_ERROR,
  UNHANDLED_ERROR,
  IO_ERROR,
  INVALID_INPUT,
  INVALID_PARSE_FILE,
  NOT_IMPLEMENTED_ERROR
};

class SoundProcessorException : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
  [[nodiscard]] virtual int getCode() const = 0;
};

class IOException : public SoundProcessorException {
 public:
  using SoundProcessorException::SoundProcessorException;
  [[nodiscard]] int getCode() const override {
	return IO_ERROR;
  }
};

class InvalidFile : public SoundProcessorException {
 public:
  using SoundProcessorException::SoundProcessorException;
  [[nodiscard]] int getCode() const override {
	return INVALID_PARSE_FILE;
  }
};

class InvalidInput : public SoundProcessorException {
 public:
  using SoundProcessorException::SoundProcessorException;
  [[nodiscard]] int getCode() const override {
	return INVALID_INPUT;
  }
};

class NotImplemented : public SoundProcessorException {
 public:
  using SoundProcessorException::SoundProcessorException;
  [[nodiscard]] int getCode() const override {
	return NOT_IMPLEMENTED_ERROR;
  }
};