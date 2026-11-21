#include "filter.h"
#include <memory>

constexpr int MaxIntegerLength = 9;

void IsInteger(const char* value);

void IsFloat(const char* value);

void IsNotNull(const char* value);

class Fabric {};

class CropFabric : public Fabric {
public:
    static std::unique_ptr<Filter> Make(const char* width, const char* height);
};

class GrayScaleFabric : public Fabric {
public:
    static std::unique_ptr<Filter> Make();
};

class NegativeFabric : public Fabric {
public:
    static std::unique_ptr<Filter> Make();
};

class SharpFabric : public Fabric {
public:
    static std::unique_ptr<Filter> Make();
};

class EdgeFabric : public Fabric {
public:
    static std::unique_ptr<Filter> Make(const char* value);
};

class BlurFabric : public Fabric {
public:
    static std::unique_ptr<Filter> Make(const char* value);
};

class TunnelFabric : public Fabric {
public:
    static std::unique_ptr<Filter> Make(const char* pixel_h, const char* pixel_w, const char* strongth,
                                        const char* radius);
};
