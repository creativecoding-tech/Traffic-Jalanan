#include "ofApp.h"
#include "road/CurvedRoad.h"

//--------------------------------------------------------------
void ofApp::setup() {
  ofSetVerticalSync(false);
  ofSetFrameRate(60); // bisa disesuaikan dengan FPS monitor, tidak harus sama
  ofSetEscapeQuitsApp(false);
  ofSetBackgroundAuto(false);
  ofDisableAlphaBlending();
  ofEnableAntiAliasing();
  ofEnableSmoothing();

  // ==================== ROAD SETUP ====================
  road = std::make_unique<CurvedRoad>();
  road->generatePath();

  // ==================== GRID SETUP ====================

  // Resize grid ke maxCells (300)
  grid.resize(maxCells);

  // ==================== SPAWN VEHICLES ====================

  // Spawn 8 mobil dengan jarak 35 cells (supaya muat body panjang)
  int numCars = 8;
  int spacing = 35; // Jarak antar mobil (cells)

  for (int i = 0; i < numCars; i++) {
    // Hitung posisi awal (jarak 7 cells antar mobil)
    float startDist = i * spacing;

    float startV = .005f;

    // Warna random (RGB 0-1)
    vec3 color = vec3(ofRandom(1.0f), // R
                      ofRandom(1.0f), // G
                      ofRandom(1.0f)  // B
    );

    // Buat SedanCar dan tambahkan ke traffic
    traffic.push_back(std::make_unique<SedanCar>(startDist, // Posisi awal
                                                 startV,    // Kecepatan awal
                                                 color,     // Warna
                                                 maxCells,  // maxCells (300)
                                                 maxV,      // maxV (5)
                                                 probSlow   // probSlow (0.2)
                                                 ));
  }
}

//--------------------------------------------------------------
void ofApp::update() {
  // ==================== STEP 1: RESET & MAPPING GRID ====================

  // Reset semua grid jadi -1 (kosong)
  grid.assign(maxCells, -1);

  // Mapping posisi vehicle ke grid
  for (int i = 0; i < traffic.size(); i++) {
    // Ambil posisi vehicle dan cast ke int
    int pos = (int)traffic[i]->getDistance();

    // Pastikan posisi dalam range (dengan wrapping)
    pos = pos % maxCells;

    // Map: posisi → indeks vehicle
    grid[pos] = i;
  }

  // ==================== STEP 2: SET GRID KE SEMUA VEHICLES
  // ====================

  // Set grid pointer ke semua vehicles
  for (int i = 0; i < traffic.size(); i++) {
    // Pass grid pointer ke vehicle (yang akan pass ke strategy)
    traffic[i]->setGrid(grid.data(), grid.size());
  }

  // ==================== STEP 3: UPDATE SEMUA VEHICLES ====================

  // Update semua vehicles (movement logic)
  for (auto &vehicle : traffic) {
    vehicle->update();

    SedanCar *car = dynamic_cast<SedanCar *>(vehicle.get());
    if (car) {
      std::vector<vec2> bodyPoints;
      int bodyLength = 15; // Panjang "cacing" (jumlah segment)
      float segmentSpacing =
          5.0f; // Jarak antar segment dalam world unit (bukan cell)

      float headDist = car->getDistance() * (road->getTotalLength() / maxCells);

      for (int j = 0; j < bodyLength; j++) {
        float d = headDist - (j * segmentSpacing);
        vec2 pt = road->getPointAtDistance(d);
        bodyPoints.push_back(pt);
      }

      car->updateBody(bodyPoints);
    }
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofSetBackgroundAuto(false);
  ofSetColor(0, 15);
  ofFill();
  ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
  road->draw(); // Gambar jalan melengkung

  // ==================== DRAW VEHICLES ====================
  for (auto &vehicle : traffic) {
    float dist = vehicle->getDistance() * (road->getTotalLength() / maxCells);

    // Get posisi (x,y) dari distance
    vec2 pos = road->getPointAtDistance(dist);

    // Get arah (tangent) untuk rotasi
    vec2 tangent = road->getTangentAtDistance(dist);
    float angle = ofRadToDeg(atan2(tangent.y, tangent.x));

    // Gambar vehicle
    vehicle->draw(pos.x, pos.y, angle);
  }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
  if (key == 'q' || key == 'Q')
    ofExit();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}
