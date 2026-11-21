#pragma once

#include <stdexcept>
#include <string>

class Exception : public std::exception {
public:
    Exception() = default;
    explicit Exception(const std::string& message) noexcept : message_(message){};

    const char* what() const noexcept final;

public:
    std::string message_;
};

class ParserException : public Exception {
public:
    ParserException();
    explicit ParserException(const std::string& message) noexcept : Exception(message){};
};

class ReadImageException : public Exception {
public:
    ReadImageException();
    explicit ReadImageException(const std::string& message) noexcept : Exception(message){};
};

class WriteImageException : public Exception {
public:
    WriteImageException();
    explicit WriteImageException(const std::string& message) noexcept : Exception(message){};
};

class ApplyFilterException : public Exception {
public:
    ApplyFilterException();
    explicit ApplyFilterException(const std::string& message) noexcept : Exception(message){};
};
