#include "exception.h"
#include "fabrics.h"
#include <filesystem>
#include <memory>
#include "parser.h"
#include <string>
#include <vector>

Parser::Parser(int argc, char** argv) : argc_(argc), argv_(argv) {
    filters_.reserve(argc);
    Parse();
}

void Parser::PathCheck(const std::string& s) const {
    if (!filesys::exists(s)) {
        throw ParserException("This file doesn't exist.");
    }
    if (!filesys::is_regular_file(s)) {
        throw ParserException("This path is bad");
    }
}

void Parser::Parse() {
    if (argc_ == 1) {
        std::cout << HELP_INFO;
        exit(0);
    }
    if (argc_ < 3) {
        throw ParserException("Expected 2 or more arguments for this app.\n");
    }

    input_path_ = filesys::path(argv_[1]);
    output_path_ = filesys::path(argv_[2]);

    PathCheck(input_path_);

    int cur_arg = 3;
    while (cur_arg < argc_) {
        if (argv_[cur_arg][0] != '-') {
            throw ParserException("Unknown parameter as " + std::to_string(cur_arg) + "\'s argument");
        }
        if (std::string(argv_[cur_arg]) == "-crop") {
            if (argc_ <= cur_arg + 2) {
                throw ParserException("Expected 2 arguments for -crop.");
            }
            filters_.push_back(CropFabric::Make(argv_[cur_arg + 1], argv_[cur_arg + 2]));
            cur_arg += 2;
        } else if (std::string(argv_[cur_arg]) == "-gs") {
            filters_.push_back(GrayScaleFabric::Make());
        } else if (std::string(argv_[cur_arg]) == "-neg") {
            filters_.push_back(NegativeFabric::Make());
        } else if (std::string(argv_[cur_arg]) == "-sharp") {
            filters_.push_back(SharpFabric::Make());
        } else if (std::string(argv_[cur_arg]) == "-edge") {
            if (argc_ <= cur_arg + 1) {
                throw ParserException("Expected 1 arguments for -edge.\n");
            }
            filters_.push_back(EdgeFabric::Make(argv_[++cur_arg]));
        } else if (std::string(argv_[cur_arg]) == "-blur") {
            if (argc_ <= cur_arg + 1) {
                throw ParserException("Expected 1 arguments for -blur.\n");
            }
            filters_.push_back(BlurFabric::Make(argv_[++cur_arg]));
        } else if (std::string(argv_[cur_arg]) == "-tunnel") {
            if (argc_ <= cur_arg + 4) {
                throw ParserException("Expected 4 arguments for -tunnel.\n");
            }
            filters_.push_back(
                TunnelFabric::Make(argv_[cur_arg + 1], argv_[cur_arg + 2], argv_[cur_arg + 3], argv_[cur_arg + 4]));
            cur_arg += 4;
        }
        ++cur_arg;
    }
}

std::string Parser::GetInput() const {
    return input_path_;
}

std::string Parser::GetOutput() const {
    return output_path_;
}

std::vector<std::unique_ptr<Filter>>::iterator Parser::Begin() {
    return filters_.begin();
}

std::vector<std::unique_ptr<Filter>>::iterator Parser::End() {
    return filters_.end();
}
