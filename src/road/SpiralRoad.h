#pragma once

#include "Road.h"
#include "ofMain.h"
#include <glm/glm.hpp>

using glm::vec2;

/**
 * SpiralRoad - Track spiral dengan aliran in-out dan GAP di center
 *
 * Menghasilkan track spiral di mana vehicles spiral masuk ke arah center,
 * menghilang di black hole (GAP), lalu muncul lagi spiral keluar.
 *
 * Fitur:
 * - Orbit IN: putar normal di outer edge (default: 3 rotasi)
 * - Spiral IN: spiral masuk ke arah center (default: 4 rotasi)
 * - GAP: track putus di center (black hole)
 * - Spiral OUT: spiral keluar dari center (default: 4 rotasi)
 * - Orbit OUT: putar normal di outer edge (default: 3 rotasi)
 * - True black hole effect: vehicles menghilang dan muncul lagi
 */
class SpiralRoad : public Road {
private:
    float centerX;        // Koordinat X pusat spiral
    float centerY;        // Koordinat Y pusat spiral
    float outerRadius;    // Radius di outer edge
    int orbitRotations;   // Jumlah putaran normal sebelum spiral (default: 3)
    int numLoops;         // Jumlah rotasi spiral (default: 4)
    int numPoints;        // Jumlah titik untuk smoothness (default: 720)

    // Two polyline system untuk GAP effect
    ofPolyline polylineIn;   // Orbit IN + Spiral IN (outer → center)
    ofPolyline polylineOut;  // Spiral OUT + Orbit OUT (center → outer)
    float lengthIn;          // Panjang polyline IN
    float lengthOut;         // Panjang polyline OUT
    float gapThreshold;      // Radius threshold untuk GAP (default: 5% dari outerRadius)

public:
    // Constructor dengan default parameters
    SpiralRoad();
    ~SpiralRoad() = default;

    // Generate jalur spiral in-out
    void generatePath(ofRectangle bounds) override;

    // Override draw() untuk render 2 polylines dengan GAP
    void draw(float offsetX = 0.0f, float offsetY = 0.0f) override;

    // Dapatkan posisi pada jarak tertentu sepanjang track
    vec2 getPointAtDistance(float dist) override;

    // Dapatkan vektor tangent (arah) pada jarak tertentu
    vec2 getTangentAtDistance(float dist) override;
};
