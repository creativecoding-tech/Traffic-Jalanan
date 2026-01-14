#include "CurvedRoad.h"
#include "ofMain.h"

// Update generatePath signature in .h first? No, let's do .cpp then .h
void CurvedRoad::generatePath(ofRectangle bounds) {
  // Clear polyline
  polyline.clear();

  float margin = 20; // Internal margin inside the bounds
  float w = bounds.width - 2 * margin;
  float h = bounds.height - 2 * margin;

  // Titik pusat relatif terhadap bounds
  float cx = bounds.x + bounds.width / 2;
  float cy = bounds.y + bounds.height / 2;

  float halfW = w / 2;
  float halfH = h / 2;

  // Radius pojok
  float r = halfH;

  // Panjang bagian lurus
  float straightLen = halfW - r;
  if (straightLen < 0)
    straightLen = 0;

  // P1: Top-Left (Start of straight)
  vec2 p1(cx - straightLen, cy - r);
  // P2: Top-Right (End of straight)
  vec2 p2(cx + straightLen, cy - r);

  // P3: Bottom-Right (End of curve)
  vec2 p3(cx + straightLen, cy + r);
  // P4: Bottom-Left (Start of straight)
  vec2 p4(cx - straightLen, cy + r);

  // Drawing Order (Clockwise):
  polyline.addVertex(p1.x, p1.y);
  polyline.lineTo(p2.x, p2.y);
  polyline.arc(cx + straightLen, cy, r, r, 270, 450);
  polyline.lineTo(p4.x, p4.y);
  polyline.arc(cx - straightLen, cy, r, r, 90, 270);
  polyline.close();

  // Hitung total length
  totalLength = polyline.getPerimeter();
}

vec2 CurvedRoad::getPointAtDistance(float dist) {
  // Normalize distance (handle wrapping)
  while (dist >= totalLength)
    dist -= totalLength;
  while (dist < 0)
    dist += totalLength;

  ofPoint pt = polyline.getPointAtLength(dist);
  return vec2(pt.x, pt.y);
}

vec2 CurvedRoad::getTangentAtDistance(float dist) {
  // Sampling 2 titik untuk hitung arah
  float delta = 1.0f; // Small offset

  vec2 p1 = getPointAtDistance(dist);
  vec2 p2 = getPointAtDistance(dist + delta);

  vec2 dir = p2 - p1;
  return normalize(dir); // glm::normalize
}
