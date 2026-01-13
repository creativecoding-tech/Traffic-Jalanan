#include "Road.h"

void Road::draw(float offsetX, float offsetY) {
    ofPushMatrix();
    ofTranslate(offsetX, offsetY);

    // Main road
    ofSetColor(100, 100, 150);
    ofSetLineWidth(40.0f);
    polyline.draw();

    ofPopMatrix();
}