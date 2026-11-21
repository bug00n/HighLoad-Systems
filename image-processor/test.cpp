#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include "exception.h"
#include "fabrics.h"
#include "filter.h"
#include "image.h"
#include "io.h"
#include "matrix.h"
#include "parser.h"
#include <random>
#include <string>
#include <vector>

// argc == len(argv) doesn't need in checking, so ...

// FABRICS TEST CASES

TEST_CASE("CropFabric corner cases") {
    REQUIRE_THROWS_AS(CropFabric::Make("0", "1"), ParserException);
    REQUIRE_NOTHROW(CropFabric::Make("1", "1"));
    REQUIRE_THROWS_AS(CropFabric::Make("-1", "1"), ParserException);
    REQUIRE_THROWS_AS(CropFabric::Make("a", "1"), ParserException);
    REQUIRE_THROWS_AS(CropFabric::Make("0.1", "1"), ParserException);
    REQUIRE_THROWS_AS(CropFabric::Make("1000000000000", "1"), ParserException);
}

TEST_CASE("EdgeFabric corner cases") {
    REQUIRE_NOTHROW(EdgeFabric::Make("0"));
    REQUIRE_NOTHROW(EdgeFabric::Make("1"));
    REQUIRE_THROWS_AS(EdgeFabric::Make("-1"), ParserException);
    REQUIRE_THROWS_AS(EdgeFabric::Make("a"), ParserException);
    REQUIRE_THROWS_AS(EdgeFabric::Make("0.0.1"), ParserException);
    REQUIRE_NOTHROW(EdgeFabric::Make("0.1"));
    REQUIRE_THROWS_AS(EdgeFabric::Make("101000100101"), ParserException);
}

TEST_CASE("BlurFabric corner cases") {
    REQUIRE_NOTHROW(BlurFabric::Make("0"));
    REQUIRE_NOTHROW(BlurFabric::Make("1"));
    REQUIRE_THROWS_AS(BlurFabric::Make("-1"), ParserException);
    REQUIRE_THROWS_AS(BlurFabric::Make("a"), ParserException);
    REQUIRE_THROWS_AS(BlurFabric::Make("0.0.1"), ParserException);
    REQUIRE_NOTHROW(BlurFabric::Make("0.1"));
    REQUIRE_THROWS_AS(BlurFabric::Make("100001.00001"), ParserException);
}

TEST_CASE("TunnelFabric corner cases") {
    REQUIRE_NOTHROW(TunnelFabric::Make("0", "0", "0", "0"));
    REQUIRE_NOTHROW(TunnelFabric::Make("1", "1", "1", "1"));
    REQUIRE_THROWS_AS(TunnelFabric::Make("-1", "1", "1", "1"), ParserException);
    REQUIRE_THROWS_AS(TunnelFabric::Make("a", "1", "1", "1"), ParserException);
    REQUIRE_NOTHROW(TunnelFabric::Make("1", "1", "0.1", "0.1"));
    REQUIRE_THROWS_AS(TunnelFabric::Make("1", "1", "-1", "1"), ParserException);
    REQUIRE_THROWS_AS(TunnelFabric::Make("0.1", "1", "1", "1"), ParserException);
    REQUIRE_THROWS_AS(TunnelFabric::Make("1", "0.1", "1", "1"), ParserException);
    REQUIRE_THROWS_AS(TunnelFabric::Make("1", "1", "0..1", "1"), ParserException);
    REQUIRE_THROWS_AS(TunnelFabric::Make("1", "1", "0.1", "1.01.1"), ParserException);
    REQUIRE_THROWS_AS(TunnelFabric::Make("10000000000000000", "1", "0.111111111111111111111", "1"), ParserException);
}

// all fabrics are correct

// PARSER TEST CASES (PATHS MAY BE INVALID)

int rnd() {
    return static_cast<int> (rand()) % 100;
}

void TestBuilder(const std::vector <std::string>& args) {
    int32_t argc = static_cast<int32_t>(args.size());
    char** argv = (char**) malloc((argc + 1) * sizeof(char*));
    for (int32_t i = 0; i < argc; ++i) {
        argv[i] = strdup(args[i].c_str());
    }
    argv[argc] = nullptr;
    Parser parser(argc, argv);
}   

TEST_CASE("Parser basic test cases") {
    // REQUIRE_NOTHROW(TestBuilder({"exe"})); -> HELP
    REQUIRE_THROWS_AS(TestBuilder({"exe", "nothing.bmp"}), ParserException);
    REQUIRE_THROWS_AS(TestBuilder({"exe", "incorrect.bmp", "/tmp/res.bmp"}), ParserException);

    REQUIRE_NOTHROW(TestBuilder({"exe", "../lenna.bmp", "/rmp/res.bmp"}));
    REQUIRE_NOTHROW(TestBuilder({"exe", "../lenna.bmp", "/rmp/res.bmp", "-crop", "1", "1"}));
    REQUIRE_NOTHROW(TestBuilder({"exe", "../lenna.bmp", "/rmp/res.bmp", "-gs"}));  
    REQUIRE_NOTHROW(TestBuilder({"exe", "../lenna.bmp", "/rmp/res.bmp", "-neg"}));
    REQUIRE_NOTHROW(TestBuilder({"exe", "../lenna.bmp", "/rmp/res.bmp", "-sharp"}));
    REQUIRE_NOTHROW(TestBuilder({"exe", "../lenna.bmp", "/rmp/res.bmp", "-edge", "1"}));
    REQUIRE_NOTHROW(TestBuilder({"exe", "../lenna.bmp", "/rmp/res.bmp", "-blur", "3"}));
    REQUIRE_NOTHROW(TestBuilder({"exe", "../lenna.bmp", "/rmp/res.bmp", "-tunnel", "0", "0", "1", "1"}));
}

TEST_CASE("Parser stress testing") {
    std::vector <std::vector <std::string>> correct_args = {
        {"-crop", "1", "1"},
        {"-crop", "2", "2"},
        {"-gs"},
        {"-neg"},
        {"-sharp"},
        {"-edge", "0.5"},
        {"-blur", "7.5"},
        {"-tunnel", "0", "0", "1.5", "35.5"},
        {"-tunnel", "50", "50", "1.5", "35.5"},
        {"-tunnel", "100", "100", "1.5", "35.5"}
    };
    std::vector <std::vector <std::string>> incorrect_args = {
        {"-crop", "0", "1"},
        {"-crop", "2", "-2"},
        {"-crop", "2"},
        {"-edge", "-0.5"},
        {"-blur", "-17"},
        {"-tunnel", "0", "0.7", "1.5", "35.5"},
        {"-tunnel", "-50", "50", "1.5", "35.5"},
        {"-tunnel", "100", "1.5", "35.5"}
    };
    srand(time(0)); // seed
    for (int _ = 0; _ < 1000; ++_) {
        std::vector <std::string> args = {"exe"};
        bool correct = true;
        if (rnd() < 5) {
            correct = false;
            args.push_back("incorrect.bmp");
        } else {
            args.push_back("../lenna.bmp");
        }
        args.push_back("/tmp/res.bmp");
        for (int i = 0; i < 3; ++i) {
            if (rnd() < 20) {
                correct = false;
                int id = rnd() % static_cast<int>(incorrect_args.size());
                for (auto &s : incorrect_args[id]) {
                    args.push_back(s);
                }
            } else {
                int id = rnd() % static_cast<int>(correct_args.size());
                for (auto &s : correct_args[id]) {
                    args.push_back(s);
                }
            }
        }
        if (correct) {
            REQUIRE_NOTHROW(TestBuilder(args));
        } else {
            REQUIRE_THROWS_AS(TestBuilder(args), ParserException);
        }
    }
    // 0.95 * 0.8^3 = 0.5 btw
}

// parser is correct

void test1() {
    std::unique_ptr<Filter> crop = CropFabric::Make("1", "1");
    std::unique_ptr<Filter> crop2 = CropFabric::Make("1000", "1000");
    std::unique_ptr<Filter> blur = BlurFabric::Make("7.5");
    std::unique_ptr<Filter> sharp = SharpFabric::Make();
    std::unique_ptr<Filter> edge = EdgeFabric::Make("7.5");
    std::unique_ptr<Filter> tunnel = TunnelFabric::Make("0", "0", "7.5", "100");
    std::unique_ptr<Filter> tunnel2 = TunnelFabric::Make("1000", "1000", "7.5", "5000");
    {
        ImageReader reader;
        reader.ReadFile("../lenna.bmp");
        auto img = reader.GetImage();
        img = crop->Apply(img);
        img = blur->Apply(img);
        img = edge->Apply(img);
        img = sharp->Apply(img);
        img = tunnel->Apply(img);
        img = crop2->Apply(img);
        img = tunnel2->Apply(img);
    }
}
void test(std::unique_ptr<Filter> filter) {
    std::unique_ptr<Filter> crop = CropFabric::Make("3", "3");
    ImageReader reader;
    reader.ReadFile("../lenna.bmp");
    auto img = reader.GetImage();
    img = crop->Apply(img);
    img = filter->Apply(img);
}

TEST_CASE("Image corner cases") {
    REQUIRE_NOTHROW(test1);

    REQUIRE_NOTHROW(test(GrayScaleFabric::Make()));
    REQUIRE_NOTHROW(test(NegativeFabric::Make()));
    REQUIRE_NOTHROW(test(SharpFabric::Make()));

    REQUIRE_NOTHROW(test(EdgeFabric::Make("1")));
    REQUIRE_NOTHROW(test(EdgeFabric::Make("0")));
    REQUIRE_NOTHROW(test(EdgeFabric::Make("1000")));

    REQUIRE_NOTHROW(test(BlurFabric::Make("1")));
    REQUIRE_THROWS_AS(test(BlurFabric::Make("0")), ApplyFilterException);
    REQUIRE_NOTHROW(test(BlurFabric::Make("1000")));

    REQUIRE_NOTHROW(test(TunnelFabric::Make("1", "1", "0.5", "1")));
    REQUIRE_THROWS_AS(test(TunnelFabric::Make("1000", "1000", "0.5", "1")), ApplyFilterException);
    REQUIRE_THROWS_AS(test(TunnelFabric::Make("0", "1000", "0.5", "1")), ApplyFilterException);
    REQUIRE_THROWS_AS(test(TunnelFabric::Make("1000", "0", "0.5", "1")), ApplyFilterException);
    REQUIRE_NOTHROW(test(TunnelFabric::Make("0", "0", "0.5", "1")));
}

// filters are correct

