#pragma once

#include "entities/SedanCar.h"
#include "entities/Vehicle.h"
#include "ofMain.h"
#include "road/CurvedRoad.h"
#include "road/Road.h"
#include <memory>
#include <vector>

using glm::vec2;
using glm::vec3;

class ofApp : public ofBaseApp {

public:
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
    std::shared_ptr<CurvedRoad> road;
    std::vector<std::shared_ptr<Vehicle>> traffic;
    std::vector<int> grid;
    int maxCells;

    // Helper to update this track
    void setup(ofRectangle bounds, int numCars, int spacing, float maxV,
               float probSlow);
    void update();
    void draw(float curveIntensity, int numLinesPerCar, ofPoint (bezierHelper)(float, ofPoint, ofPoint, ofPoint, ofPoint));
  };

  // Tracks
  std::vector<TrackInstance> tracks;

  // Global params (bisa dipindah ke track kalau mau variasi)
  const float maxV = 20.0f;
  float probSlow = .7f; // Default global

  // Bezier curve helper (dari TestModelNaSch)
  static ofPoint getBezierPoint(float t, ofPoint p0, ofPoint p1, ofPoint p2, ofPoint p3);

  // Parameter untuk intensitas kurva garis radial
  float curveIntensity = 0.5f;

  // Jumlah garis radial per mobil (default 5)
  int numLinesPerCar = 5;
};
