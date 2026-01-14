#pragma once

#include "entities/SedanCar.h"
#include "entities/Vehicle.h"
#include "ofMain.h"
#include "road/CircleRoad.h"
#include "road/CurvedRoad.h"
#include "road/Road.h"
#include <memory>
#include <vector>

using glm::vec2;
using glm::vec3;

class ofApp : public ofBaseApp {

public:
  // Road type enum
  enum RoadType {
    CIRCLE,   // Lingkaran sempurna (default)
    CURVED    // Oval dengan straight sections
  };
  void setup();
  void update();
  void draw();

  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void mouseEntered(int x, int y);
  void mouseExited(int x, int y);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);

private:
  // Struct to hold simulation instance
  struct TrackInstance {
    std::shared_ptr<Road> road;  // Gunakan Road base class (bukan CurvedRoad)
    ofRectangle bounds;          // Simpan bounds untuk regenerate road
    std::vector<std::shared_ptr<Vehicle>> traffic;
    std::vector<int> grid;
    int maxCells;
    int numLinesPerCar;  // Jumlah garis per mobil untuk track ini
    float curveIntensity; // Intensitas kelengkungan garis radial
    float curveAngle1;    // Angle offset untuk P1 (dalam radian)
    float curveAngle2;    // Angle offset untuk P2 (dalam radian)

    // Helper to update this track
    void setup(ofRectangle bounds, int numCars, int spacing, float maxV,
               float probSlow, int maxCells, RoadType roadType, int numLinesPerCar, float curveIntensity,
               float curveAngle1, float curveAngle2);
    void update();
    void draw(ofPoint (bezierHelper)(float, ofPoint, ofPoint, ofPoint, ofPoint));
    void regenerateRoad(RoadType roadType);  // Switch road type
  };

  // Tracks
  std::vector<TrackInstance> tracks;

  // Current road type
  RoadType currentRoadType = CIRCLE;  // Default: CircleRoad

  // Jumlah mobil per track
  int numCarsOuter = 20;   // Track luar
  int numCarsMiddle = 9;   // Track tengah
  int numCarsInner = 6;    // Track dalam

  // Ukuran track dalam cells per track (semakin besar track, semakin banyak cells)
  int maxCellsOuter = 1500;   // Track luar - lebih panjang
  int maxCellsMiddle = 600;  // Track tengah
  int maxCellsInner = 400;   // Track dalam - lebih pendek

  // Kecepatan maksimal per track (track lebih besar = lebih cepat)
  float maxVOuter = 20.0f;    // Track luar - paling cepat
  float maxVMiddle = 6.0f;   // Track tengah - sedang
  float maxVInner = 5.0f;    // Track dalam - paling lambat

  // Jumlah garis radial per mobil (per track)
  int numLinesPerCarOuter = 10;    // Track luar - lebih banyak garis
  int numLinesPerCarMiddle = 10;   // Track tengah
  int numLinesPerCarInner = 10;    // Track dalam - lebih sedikit garis

  // Intensitas kelengkungan garis radial per track
  float curveIntensityOuter = .0f;   // Track luar - lurus
  float curveIntensityMiddle = 0.f;  // Track tengah - sedang melengkung
  float curveIntensityInner = 0.f;   // Track dalam - lebih melengkung

  // Angle offset untuk P1 per track (dalam radian)
  float curveAngle1Outer = HALF_PI / 2;      // Track luar - 45 derajat
  float curveAngle1Middle = HALF_PI / 4;     // Track tengah
  float curveAngle1Inner = HALF_PI;      // Track dalam

  // Angle offset untuk P2 per track (dalam radian)
  float curveAngle2Outer = -HALF_PI;         // Track luar - -90 derajat
  float curveAngle2Middle = -HALF_PI / 4;        // Track tengah
  float curveAngle2Inner = -HALF_PI / 2;         // Track dalam

  // Probabilitas random braking per track (semakin kecil, semakin lancar)
  float probSlowOuter = 0.03f;   // Track luar
  float probSlowMiddle = 0.3f;  // Track tengah
  float probSlowInner = 0.25f;   // Track dalam

  // Bezier curve helper (dari TestModelNaSch)
  static ofPoint getBezierPoint(float t, ofPoint p0, ofPoint p1, ofPoint p2, ofPoint p3);

  // Simulation control
  bool simulationStarted = false;  // Simulasi belum mulai sampai tekan 's' atau 'S'

  // Road switching controls
  // '1' = CircleRoad (default, lingkaran sempurna)
  // '2' = CurvedRoad (oval dengan straight sections)
};
