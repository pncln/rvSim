#ifndef TRANSFORM2D_H
#define TRANSFORM2D_H

#include <array>

class TRANSFORM2D {
private:
    static std::array<double, 3> crossProduct(const std::array<double, 3>& a,
                                              const std::array<double, 3>& b);
    static double vectorNorm(const std::array<double, 3>& vec);
    static std::array<double, 3> normalizeVector(const std::array<double, 3>& vec);

public:
    static std::array<double, 2> transformTo2D(
        const std::array<double, 3>& pos1,
        const std::array<double, 3>& pos2,
        const std::array<double, 3>& point);
};

#endif // COORDINATE_TRANSFORM_H
