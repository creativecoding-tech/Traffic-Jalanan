#include "SpiralRoad.h"

//--------------------------------------------------------------
SpiralRoad::SpiralRoad()
    : centerX(0)
    , centerY(0)
    , outerRadius(0)
    , orbitRotations(10)   // 1 putaran orbit sebelum spiral IN (cepat!)
    , numLoops(8)         // 3 rotasi in, 3 rotasi out (dramatic!)
    , numPoints(720)      // Total titik untuk smoothness
    , lengthIn(0)
    , lengthOut(0)
    , gapThreshold(0.15f) // 15% dari outerRadius = GAP (kecil tapi visible)
{
}

//--------------------------------------------------------------
void SpiralRoad::generatePath(ofRectangle bounds) {
    // Bersihkan semua path yang sudah ada
    polylineIn.clear();
    polylineOut.clear();
    polyline.clear();
    path.clear();

    // Hitung titik pusat dari bounds
    centerX = bounds.x + bounds.width / 2.0f;
    centerY = bounds.y + bounds.height / 2.0f;

    // Hitung outer radius dari bounds
    outerRadius = std::min(bounds.width, bounds.height) / 2.0f;

    // Radius threshold untuk GAP (5% dari outer radius)
    float gapRadius = outerRadius * gapThreshold;

    // ===== POLYLINE IN: Orbit IN + Spiral IN =====

    // Phase 1: Orbit IN (circular di outer edge)
    int orbitPoints = orbitRotations * 60;
    for (int i = 0; i < orbitPoints; i++) {
        float progress = i / (float)orbitPoints;
        float angle = progress * TWO_PI * orbitRotations;

        float x = centerX + cos(angle) * outerRadius;
        float y = centerY + sin(angle) * outerRadius;

        polylineIn.addVertex(x, y);
    }

    // Phase 2: Spiral IN (outer → gapRadius)
    int spiralInPoints = (numPoints - orbitPoints) / 2;
    for (int i = 0; i < spiralInPoints; i++) {
        float progress = i / (float)spiralInPoints;  // 0.0 → 1.0

        // Radius: outer → gapRadius (berhenti sebelum center!)
        float r = outerRadius - (outerRadius - gapRadius) * progress;

        // Angle: berput saat masuk ke dalam
        float startAngle = TWO_PI * orbitRotations;
        float angle = startAngle + (progress * TWO_PI * numLoops);

        float x = centerX + cos(angle) * r;
        float y = centerY + sin(angle) * r;

        polylineIn.addVertex(x, y);
    }

    // ===== POLYLINE OUT: KOSONG =====
    // TIDAK ADA Spiral OUT! TIDAK ADA Orbit OUT! TIDAK ADA Ghost Phase!
    // Track berakhir di akhir Spiral IN (gapRadius)
    // Track KOSONG karena mobil akan di-destroy saat masuk black hole

    // Tambah SATU titik di gapRadius supaya polyline valid (minimal)
    float ghostRadius = gapRadius * 0.5f;
    vec2 lastPoint(centerX + cos(TWO_PI * (orbitRotations + numLoops)) * ghostRadius,
                     centerY + sin(TWO_PI * (orbitRotations + numLoops)) * ghostRadius);
    polylineOut.addVertex(ofPoint(lastPoint.x, lastPoint.y));

    // Hitung panjang masing-masing polyline
    lengthIn = polylineIn.getPerimeter();
    lengthOut = polylineOut.getPerimeter();

    // Total length = sum untuk wrapping logic
    totalLength = lengthIn + lengthOut;

    // Also add vertices to base polyline for compatibility
    // (meskipun kita akan pakai polylineIn/polylineOut untuk draw dan getPoint)
    for (auto& pt : polylineIn.getVertices()) {
        polyline.addVertex(pt);
        path.push_back(vec2(pt.x, pt.y));
    }
    for (auto& pt : polylineOut.getVertices()) {
        polyline.addVertex(pt);
        path.push_back(vec2(pt.x, pt.y));
    }
}

//--------------------------------------------------------------
vec2 SpiralRoad::getPointAtDistance(float dist) {
    // Normalisasi distance untuk handle wrapping
    dist = fmod(dist, totalLength);
    if (dist < 0) {
        dist += totalLength;
    }

    // Switch antar polyline berdasarkan distance
    if (dist < lengthIn) {
        // Masih di polyline IN (orbit + spiral IN)
        ofPoint pt = polylineIn.getPointAtLength(dist);
        return vec2(pt.x, pt.y);
    } else {
        // Di polyline OUT (spiral OUT + orbit)
        float distInOut = dist - lengthIn;  // Offset untuk polyline OUT
        ofPoint pt = polylineOut.getPointAtLength(distInOut);
        return vec2(pt.x, pt.y);
    }
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

    // Draw polyline IN (Orbit + Spiral IN)
    // Color: Gradient dari biru ke merah (menunjukkan approach ke black hole)
    ofSetColor(0, 0, 0);  // Light blue untuk orbit IN
    polylineIn.draw();

    // Draw polyline OUT (Spiral OUT + Orbit)
    // Color: Gradient dari merah ke biru (menunjukkan exit dari black hole)
    ofSetColor(255, 150, 100);  // Orange untuk spiral OUT
    polylineOut.draw();

    ofPopMatrix();
}
