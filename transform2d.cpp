#include "transform2d.h"
// #include <iostream>

#include <cmath>

std::array<double, 3> TRANSFORM2D::crossProduct(
    const std::array<double, 3>& a,
    const std::array<double, 3>& b) {
    return {
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    };
}

double TRANSFORM2D::vectorNorm(const std::array<double, 3>& vec) {
    return std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

std::array<double, 3> TRANSFORM2D::normalizeVector(
    const std::array<double, 3>& vec) {
    double norm = vectorNorm(vec);
    return {vec[0] / norm, vec[1] / norm, vec[2] / norm};
}

std::array<double, 2> TRANSFORM2D::transformTo2D(
    const std::array<double, 3>& pos1,
    const std::array<double, 3>& pos2,
    const std::array<double, 3>& point) {

    // Print input vectors


    // Calculate normal vector of the orbital plane
    auto normal = normalizeVector(crossProduct(pos1, pos2));

    // Choose reference vector for creating coordinate system
    std::array<double, 3> temp = {
        std::abs(normal[2]) < std::abs(normal[0]) ? 0.0 : 1.0,
        0.0,
        std::abs(normal[2]) < std::abs(normal[0]) ? 1.0 : 0.0
    };

    // Create orthonormal basis
    auto x_axis = normalizeVector(crossProduct(normal, temp));
    auto y_axis = crossProduct(normal, x_axis);


    // Project point onto new coordinate system
    return {
        point[0] * x_axis[0] + point[1] * x_axis[1] + point[2] * x_axis[2],
        point[0] * y_axis[0] + point[1] * y_axis[1] + point[2] * y_axis[2]
    };
}
