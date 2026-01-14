#include "CircleRoad.h"
#include "ofMain.h"

void CircleRoad::generatePath(ofRectangle bounds) {
  // Clear polyline
  polyline.clear();

  // Pusat lingkaran di tengah bounds
  centerX = bounds.x + bounds.width / 2;
  centerY = bounds.y + bounds.height / 2;

  // Radius lingkaran (setengah dari lebar/tinggi bounds, ambil yang lebih kecil)
  float radiusX = bounds.width / 2;
  float radiusY = bounds.height / 2;
  radius = std::min(radiusX, radiusY);

  // Buat lingkaran penuh dengan arc
  // Arc parameters: x, y, radiusX, radiusY, angleStart, angleEnd
  polyline.arc(centerX, centerY, radius, radius, 0, 360);

  // Hitung total length (keliling lingkaran = 2 * PI * radius)
  totalLength = polyline.getPerimeter();
}

vec2 CircleRoad::getPointAtDistance(float dist) {
  // Normalize distance (handle wrapping)
  while (dist >= totalLength)
    dist -= totalLength;
  while (dist < 0)
    dist += totalLength;

  // Mapping distance ke angle di lingkaran
  // distance 0 → angle 0
  // distance totalLength → angle 360
  float angle = ofMap(dist, 0, totalLength, 0, TWO_PI);

  // Hitung posisi x, y dari angle dan radius
  float x = centerX + cos(angle) * radius;
  float y = centerY + sin(angle) * radius;

  return vec2(x, y);
}

vec2 CircleRoad::getTangentAtDistance(float dist) {
  // Untuk lingkaran, tangent selalu tegak lurus dengan radius
  // Kita bisa hitung langsung tanpa sampling

  // Normalize distance
  while (dist >= totalLength)
    dist -= totalLength;
  while (dist < 0)
    dist += totalLength;

  // Mapping distance ke angle
  float angle = ofMap(dist, 0, totalLength, 0, TWO_PI);

  // Tangent pada lingkaran: perpendicular ke radius
  // Radius vector: (cos(angle), sin(angle))
  // Tangent vector: (-sin(angle), cos(angle)) → 90 derajat dari radius
  float tx = -sin(angle);
  float ty = cos(angle);

  return normalize(vec2(tx, ty));
}
