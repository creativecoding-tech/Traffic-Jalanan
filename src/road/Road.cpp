#include "Road.h"

void Road::draw(float offsetX, float offsetY) {
    ofPushMatrix();
    ofTranslate(offsetX, offsetY);

    // Main road
    ofSetColor(0, 0, 0);
    ofSetLineWidth(10.0f);
    polyline.draw();

    ofPopMatrix();
}