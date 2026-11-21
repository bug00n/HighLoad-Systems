#pragma once

#include <filesystem>
#include "filter.h"
#include <memory>
#include <string>
#include <vector>

namespace filesys = std::filesystem;

const std::string HELP_INFO =
    "Well,\n"
    "1. -crop <int> <int> : crops the photo to height as the first parameter and to width\n"
    "as the second parameter.\n"
    "2. -gs : renders the photo to black and white.\n"
    "3. -neg : makes a negative of the photo.\n"
    "4. -sharp : improves a contrast of the photo.\n"
    "5. -edge <double> : highlights borders with using the \"sensibility\" as the parameter.\n"
    "6. -blur <double> : blurs the photo with using the \"sensibility\" as the parameter.\n"
    "7. -tunnel <int> <int> <double> <double> : rotate pixels around a pixel with coords\n"
    "as the first parameter for x and the second parameter for y with strength as the third\n"
    "parameter and radius as the fourth parameter. You can write 0 0 as first ans second\n"
    "parameter it will mean center of the photo."
    "!!!ATTENTION: USE ONLY NON-NEGATIVE NUMBERS!!!";

class Parser {
private:
    int argc_;
    char** argv_;

    std::string input_path_;
    std::string output_path_;

    std::vector<std::unique_ptr<Filter>> filters_;

    void PathCheck(const std::string& s) const;

    void Parse();

public:
    Parser(int argc, char** argv);

    std::string GetInput() const;

    std::string GetOutput() const;

    std::vector<std::unique_ptr<Filter>>::iterator Begin();

    std::vector<std::unique_ptr<Filter>>::iterator End();
};
