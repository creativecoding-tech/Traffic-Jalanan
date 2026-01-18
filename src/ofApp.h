#pragma once

#include "entities/SedanCar.h"
#include "entities/Vehicle.h"
#include "ofMain.h"
#include "road/CircleRoad.h"
#include "road/CurvedRoad.h"
#include "road/PerlinNoiseRoad.h"
#include "road/Road.h"
#include "road/SpiralRoad.h"
#include <memory>
#include <vector>

using glm::vec2;
using glm::vec3;

class ofApp : public ofBaseApp {

public:
  // Road type enum
  enum RoadType {
    CIRCLE,        // Lingkaran sempurna (default)
    CURVED,        // Oval dengan straight sections
    PERLIN_NOISE,  // Lingkaran organik dengan Perlin noise
    SPIRAL         // Spiral in-out continuous
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
    std::shared_ptr<Road> road;  // Gunakan Road base class
    RoadType roadType;            // Tipe road untuk cek SpiralRoad behavior
    ofRectangle bounds;          // Simpan bounds untuk regenerate road
    std::vector<std::shared_ptr<Vehicle>> traffic;
    std::vector<int> grid;
    int maxCells;
    float maxV;  // Kecepatan maksimal untuk track ini (normal mode)
    float spiralMaxV;  // Kecepatan maksimal khusus untuk SpiralRoad
    int numLinesPerCar;  // Jumlah garis per mobil untuk track ini
    float curveIntensity; // Intensitas kelengkungan garis radial
    float curveAngle1;    // Angle offset untuk P1 (dalam radian)
    float curveAngle2;    // Angle offset untuk P2 (dalam radian)
    int direction;        // Arah putaran: 1 = counter-clockwise, -1 = clockwise
    bool visible;         // Visibility toggle untuk track ini
    bool drawFromCenter;  // true: center→car, false: car→center
    bool gradientMode;    // true: white→dark gradient, hide cars

    // Untuk SpiralRoad: daftar indeks vehicle yang harus dihapus
    std::vector<int> vehiclesToRemove;

    // Helper to update this track
    void setup(ofRectangle bounds, int numCars, int spacing, float maxV, float spiralMaxV,
               float probSlow, int maxCells, RoadType roadType,
               int numLinesPerCar, float curveIntensity, float curveAngle1, float curveAngle2, int direction);
    void update();
    void draw(ofPoint (bezierHelper)(float, ofPoint, ofPoint, ofPoint, ofPoint), float wobbleTime, bool gradientMode);
    void regenerateRoad(RoadType roadType);  // Switch road type
  };

  // Tracks
  std::vector<TrackInstance> tracks;

  // Current road type
  RoadType currentRoadType = CIRCLE;  // Default: CircleRoad

  // Jumlah mobil per track
  int numCarsOuter = 20;   // Track luar
  int numCarsMiddle = 20;   // Track tengah
  int numCarsInner = 20;    // Track dalam

  // Ukuran track dalam cells per track (semakin besar track, semakin banyak cells)
  int maxCellsOuter = 1500;   // Track luar - lebih panjang
  int maxCellsMiddle = 1500;  // Track tengah
  int maxCellsInner = 1500;   // Track dalam - lebih pendek

  // Kecepatan maksimal per track (track lebih besar = lebih cepat)
  float maxVOuter = 20.0f;    // Track luar - paling cepat
  float maxVMiddle = 6.0f;   // Track tengah - sedang
  float maxVInner = 5.0f;    // Track dalam - paling lambat

  // Kecepatan maksimal khusus untuk SpiralRoad
  float spiralMaxVOuter = .7f;    
  float spiralMaxVMiddle = 1.f;   
  float spiralMaxVInner = 1.5f;    

  // Jumlah garis radial per mobil (per track)
  int numLinesPerCarOuter = 5;    // Track luar - lebih banyak garis
  int numLinesPerCarMiddle = 5;   // Track tengah
  int numLinesPerCarInner = 5;    // Track dalam - lebih sedikit garis

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
  float curveAngle2Middle = -HALF_PI /2;        // Track tengah
  float curveAngle2Inner = -HALF_PI / 2;         // Track dalam

  // Arah putaran per track: 1 = counter-clockwise, -1 = clockwise
  int directionOuter = 1;     // Track luar - counter-clockwise
  int directionMiddle = -1;   // Track tengah - clockwise (berlawanan)
  int directionInner = 1;     // Track dalam - counter-clockwise

  // Probabilitas random braking per track (semakin kecil, semakin lancar)
  float probSlowOuter = 0.03f;   // Track luar
  float probSlowMiddle = 0.03f;  // Track tengah
  float probSlowInner = 0.03f;   // Track dalam

  // Bezier curve helper
  static ofPoint getBezierPoint(float t, ofPoint p0, ofPoint p1, ofPoint p2, ofPoint p3);

  // TAB mode helpers
  void drawInterTrackBezier(float wobbleTime);
  void drawCarForTabMode(TrackInstance& track, int carIndex);
  ofPoint getCarPosition(TrackInstance& track, int carIndex);
  bool isInBlackHole(TrackInstance& track, int carIndex);
  void drawContinuousBezier(ofPoint p0, ofPoint p1, ofPoint p2, ofPoint center,
                            vec3 col, float wobbleTime, int carIndex,
                            TrackInstance& outerTrack, TrackInstance& middleTrack, TrackInstance& innerTrack);
  ofPoint calculateControlPoint(ofPoint start, ofPoint end, ofPoint center,
                                 int direction, float wobbleTime, int carIndex,
                                 float curveIntensity, float curveAngle);
  void drawBezierSegment(ofPoint p0, ofPoint p1, ofPoint p2, ofPoint p3,
                         vec3 col, int segments);

  // Simulation control
  bool simulationStarted = false;  // Simulasi belum mulai sampai tekan 's' atau 'S'
  bool tabMode = false;  // TAB mode: draw inter-track bezier instead of center→car
};
