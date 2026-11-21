#include "handler.h"
#include <memory>

std::shared_ptr<Image> ApplyFilters(std::shared_ptr<Image> img, Parser& parser) {
    for (auto filter = parser.Begin(); filter < parser.End(); ++filter) {
        img = (*filter)->Apply(img);
    }
    return img;
}
