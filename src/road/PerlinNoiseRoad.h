#pragma once

#include "Road.h"
#include "ofMain.h"
#include <glm/glm.hpp>

using glm::vec2;

/**
 * PerlinNoiseRoad - Jalan lingkaran organik dengan deformasi Perlin noise
 *
 * Menghasilkan track lingkaran yang dideformasi menggunakan multi-octave Perlin noise
 * untuk menciptakan bentuk organik yang tidak sempurna. Noise diterapkan secara radial
 * untuk mendefromasi radius lingkaran dasar.
 *
 * Fitur:
 * - Multi-octave Perlin noise untuk detail yang terlihat natural
 * - Deformation configurable (20-40% variasi radius)
 * - Noise statis (tidak animasi seiring waktu)
 * - Interpolasi smooth antara titik-titik noise
 */
class PerlinNoiseRoad : public Road {
private:
    float centerX;        // Koordinat X pusat track
    float centerY;        // Koordinat Y pusat track
    float baseRadius;     // Radius dasar sebelum deformasi
    int numPoints;        // Jumlah titik yang digenerate (default: 360)
    float noiseScale;     // Kontrol frekuensi noise (default: 1.5)
    float deformAmount;   // Jumlah deformasi 0.0-1.0 (default: 0.3 = 30%)

public:
    // Constructor dengan default parameters
    PerlinNoiseRoad();
    ~PerlinNoiseRoad() = default;

    // Generate jalur lingkaran organik menggunakan Perlin noise
    void generatePath(ofRectangle bounds) override;

    // Dapatkan posisi pada jarak tertentu sepanjang track
    vec2 getPointAtDistance(float dist) override;

    // Dapatkan vektor tangent (arah) pada jarak tertentu
    vec2 getTangentAtDistance(float dist) override;
};
