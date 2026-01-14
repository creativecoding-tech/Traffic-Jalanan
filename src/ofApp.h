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

    // Helper to update this track
    void setup(ofRectangle bounds, int numCars, int spacing, float maxV,
               float probSlow, int maxCells, RoadType roadType);
    void update();
    void draw(float curveIntensity, int numLinesPerCar, ofPoint (bezierHelper)(float, ofPoint, ofPoint, ofPoint, ofPoint));
    void regenerateRoad(RoadType roadType);  // Switch road type
  };

  // Tracks
  std::vector<TrackInstance> tracks;

  // Current road type
  RoadType currentRoadType = CIRCLE;  // Default: CircleRoad

  // Jumlah mobil per track
  int numCarsOuter = 10;   // Track luar
  int numCarsMiddle = 8;   // Track tengah
  int numCarsInner = 6;    // Track dalam

  // Ukuran track dalam cells per track (semakin besar track, semakin banyak cells)
  int maxCellsOuter = 800;   // Track luar - lebih panjang
  int maxCellsMiddle = 600;  // Track tengah
  int maxCellsInner = 400;   // Track dalam - lebih pendek

  // Kecepatan maksimal per track (track lebih besar = lebih cepat)
  float maxVOuter = 20.0f;    // Track luar - paling cepat
  float maxVMiddle = 15.0f;   // Track tengah - sedang
  float maxVInner = 10.0f;    // Track dalam - paling lambat

  // Probabilitas random braking per track (semakin kecil, semakin lancar)
  float probSlowOuter = 0.2f;   // Track luar
  float probSlowMiddle = 0.15f;  // Track tengah
  float probSlowInner = 0.25f;   // Track dalam

  // Bezier curve helper (dari TestModelNaSch)
  static ofPoint getBezierPoint(float t, ofPoint p0, ofPoint p1, ofPoint p2, ofPoint p3);

  // Parameter untuk intensitas kurva garis radial
  float curveIntensity = 0.f;

  // Jumlah garis radial per mobil (default 5)
  int numLinesPerCar = 3;

  // Simulation control
  bool simulationStarted = false;  // Simulasi belum mulai sampai tekan 's' atau 'S'

  // Road switching controls
  // '1' = CircleRoad (default, lingkaran sempurna)
  // '2' = CurvedRoad (oval dengan straight sections)
};
