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
  int spacing = 150; // Jarak antar mobil (cells)

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
      auto &segments = car->getSegmentDistances();

      // 1. Update Head (Index 0) ikut posisi Vehicle (NaSch)
      float headDist = car->getDistance(); // Raw distance (0-300)

      // Handle wrapping jump for head
      // Kalau beda jauh (wrap), reset segments? Atau biarkan physics catch up?
      // Idealnya: segments[0] = headDist.
      segments[0] = headDist;

      // 2. Update Ekor (Index 1..N) - Follow logic
      float time = ofGetElapsedTimef() * 6.0f;

      for (size_t j = 1; j < segments.size(); j++) {
        float leaderDist = segments[j - 1];
        float followerDist = segments[j];

        // Target Ideal Spacing (Inchworm logic)
        float targetSpacing =
            5.0f + sin(time - j * 0.5f) * 2.0f; // Variasi +/- 2

        // Posisi ideal follower
        float targetPos = leaderDist - targetSpacing;

        // WRAPPING LOGIC:
        // Kalau leader ada di 5, follower di 290. Jarak bukan 5 - 290 = -285.
        // Tapi jarak wrapping.
        // Kita normalisasi semua ke linear space relatif terhadap leader.
        // Tapi karena ini loop tertutup, paling gampang kita anggap
        // kalau jarak > maxCells/2, berarti ada wrapping.

        float diff = leaderDist - followerDist;

        // Koreksi wrapping
        if (diff < -maxCells / 2)
          diff += maxCells; // Leader sudah lewat 0, follower masih di 300
        if (diff > maxCells / 2)
          diff -= maxCells; // Leader di 300, follower sudah lewat 0 (jarang)

        // Jarak aktual sekarang
        float actualSpacing = diff;

        // Physics: Move towards target spacing
        // "Force" sebanding dengan error (Spring)
        // Error = actualSpacing - targetSpacing
        // Kalau actual 10, target 5 -> Error 5 (kejauhan) -> speed up
        // Kalau actual 2, target 5 -> Error -3 (kedeketan) -> slow down /
        // mundur

        float spacingError = actualSpacing - targetSpacing;

        // Apply movement (Spring strength 0.1 - 0.3)
        float moveAmt = spacingError * 0.2f;

        segments[j] += moveAmt;

        // Keep in range 0-maxCells
        if (segments[j] >= maxCells)
          segments[j] -= maxCells;
        if (segments[j] < 0)
          segments[j] += maxCells;
      }

      // 3. Konversi Scalar Distances -> World Points (vec2)
      std::vector<vec2> bodyPoints;
      float roadLen = road->getTotalLength();

      for (float d : segments) {
        // Map cell distance -> world distance
        float worldD = d * (roadLen / maxCells);
        bodyPoints.push_back(road->getPointAtDistance(worldD));
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
