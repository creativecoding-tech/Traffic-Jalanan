#pragma once
#include "Road.h"

/**
 * CircleRoad - Jalan berbentuk lingkaran sempurna
 *
 * Seperti di TestModelNaSch, jalan berbentuk circle penuh.
 * Mobil bergerak searah jarum jam di sekeliling lingkaran.
 */
class CircleRoad : public Road {
public:
  CircleRoad() = default;
  ~CircleRoad() = default;

  /**
   * Generate Path berbentuk lingkaran sempurna
   * @param bounds Rectangle yang defines area untuk lingkaran
   */
  void generatePath(ofRectangle bounds) override;

  /**
   * Get Point at Distance
   * Mapping distance ke posisi di lingkaran
   */
  vec2 getPointAtDistance(float dist) override;

  /**
   * Get Tangent at Distance
   * Menghitung arah tangent pada lingkaran
   */
  vec2 getTangentAtDistance(float dist) override;

private:
  float centerX, centerY;  // Pusat lingkaran
  float radius;            // Radius lingkaran
};
