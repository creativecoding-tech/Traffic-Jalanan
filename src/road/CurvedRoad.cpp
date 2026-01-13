#include "CurvedRoad.h"
#include "ofMain.h"

void CurvedRoad::generatePath() {
    // Clear polyline
    polyline.clear();

    float margin = 100;
    float width = ofGetWidth() - 2 * margin;
    float height = ofGetHeight() - 2 * margin;

    // Start point
    polyline.lineTo(margin, margin + height * 0.5f);

    // Bezier curve 1: Wave naik
    polyline.bezierTo(
        margin + width * 0.25f, margin,      // CP1
        margin + width * 0.25f, margin,      // CP2
        margin + width * 0.5f, margin + height * 0.5f  // Anchor
    );

    // Bezier curve 2: Wave turun
    polyline.bezierTo(
        margin + width * 0.75f, margin + height,  // CP1
        margin + width * 0.75f, margin + height,  // CP2
        margin + width, margin + height * 0.5f    // Anchor
    );

    // Hitung total length
    totalLength = polyline.getPerimeter();
}

vec2 CurvedRoad::getPointAtDistance(float dist) {
    // Normalize distance (handle wrapping)
    while (dist >= totalLength) dist -= totalLength;
    while (dist < 0) dist += totalLength;

    ofPoint pt = polyline.getPointAtLength(dist);
    return vec2(pt.x, pt.y);
}

vec2 CurvedRoad::getTangentAtDistance(float dist) {
    // Sampling 2 titik untuk hitung arah
    float delta = 1.0f;  // Small offset

    vec2 p1 = getPointAtDistance(dist);
    vec2 p2 = getPointAtDistance(dist + delta);

    vec2 dir = p2 - p1;
    return normalize(dir);  // glm::normalize
}
