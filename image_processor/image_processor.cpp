#include "exception.h"
#include "filter.h"
#include "handler.h"
#include <iostream>
#include "io.h"
#include <memory>
#include "parser.h"
#include <stdexcept>

int main(int argc, char** argv) {
    try {
        Parser parser(argc, argv);

        ImageReader reader;
        reader.ReadFile(parser.GetInput());

        std::shared_ptr<Image> img = ApplyFilters(reader.GetImage(), parser);

        ImageWriter writer(img);
        writer.WriteFile(parser.GetOutput());
    } catch (Exception& e) {
        std::cerr << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
