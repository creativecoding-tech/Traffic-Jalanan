#include "SpiralRoad.h"

//--------------------------------------------------------------
SpiralRoad::SpiralRoad()
    : centerX(0)
    , centerY(0)
    , outerRadius(0)
    , orbitRotations(9)   // putaran orbit sebelum spiral IN (cepat!)
    , numLoops(8)         // rotasi in
    , numPoints(720)      // Total titik untuk smoothness
    , length(0)
    , gapThreshold(0.05f)
{
}

//--------------------------------------------------------------
void SpiralRoad::generatePath(ofRectangle bounds) {
    // Bersihkan semua path yang sudah ada
    polyline.clear();
    path.clear();

    // Hitung titik pusat dari bounds
    centerX = bounds.x + bounds.width / 2.0f;
    centerY = bounds.y + bounds.height / 2.0f;

    // Hitung outer radius dari bounds
    outerRadius = std::min(bounds.width, bounds.height) / 2.0f;

    // Radius threshold untuk GAP (15% dari outer radius)
    float gapRadius = outerRadius * gapThreshold;

    // ===== POLYLINE: Orbit IN + Spiral IN =====

    // Phase 1: Orbit IN (circular di outer edge)
    int orbitPoints = orbitRotations * 60;
    for (int i = 0; i < orbitPoints; i++) {
        float progress = i / (float)orbitPoints;
        float angle = progress * TWO_PI * orbitRotations;

        float x = centerX + cos(angle) * outerRadius;
        float y = centerY + sin(angle) * outerRadius;

        polyline.addVertex(x, y);
        path.push_back(vec2(x, y));
    }

    // Phase 2: Spiral IN (outer → gapRadius)
    int spiralInPoints = (numPoints - orbitPoints) / 2;
    for (int i = 0; i < spiralInPoints; i++) {
        float progress = i / (float)spiralInPoints;  // 0.0 → 1.0

        // Radius: outer → gapRadius (berhenti sebelum center!)
        float r = outerRadius - (outerRadius - gapRadius) * progress;

        // Angle: berputar saat masuk ke dalam
        float startAngle = TWO_PI * orbitRotations;
        float angle = startAngle + (progress * TWO_PI * numLoops);

        float x = centerX + cos(angle) * r;
        float y = centerY + sin(angle) * r;

        polyline.addVertex(x, y);
        path.push_back(vec2(x, y));
    }

    // Track berakhir di sini (mobil di-destroy di black hole)

    // Hitung panjang track
    length = polyline.getPerimeter();
    totalLength = length;
}

//--------------------------------------------------------------
vec2 SpiralRoad::getPointAtDistance(float dist) {
    // Normalisasi distance untuk handle wrapping
    dist = fmod(dist, totalLength);
    if (dist < 0) {
        dist += totalLength;
    }

    // Ambil titik dari polyline (orbit + spiral IN saja)
    ofPoint pt = polyline.getPointAtLength(dist);
    return vec2(pt.x, pt.y);
}

//--------------------------------------------------------------
vec2 SpiralRoad::getTangentAtDistance(float dist) {
    // Ambil dua titik berdekatan untuk menghitung vektor arah
    float delta = 1.0f;
    vec2 p1 = getPointAtDistance(dist);
    vec2 p2 = getPointAtDistance(dist + delta);

    // Hitung arah dan normalize
    vec2 dir = p2 - p1;
    return normalize(dir);
}

//--------------------------------------------------------------
void SpiralRoad::draw(float offsetX, float offsetY) {
    // Apply offset jika ada
    ofPushMatrix();
    ofTranslate(offsetX, offsetY);

    // Draw polyline (Orbit + Spiral IN)
    ofSetColor(0, 0, 0);  // Hitam untuk track
    polyline.draw();

    ofPopMatrix();
}
