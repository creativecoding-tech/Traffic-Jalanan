#include "ofApp.h"
#include "road/CurvedRoad.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::setup() {
  ofSetVerticalSync(false);
  ofSetFrameRate(60);
  ofSetEscapeQuitsApp(false);
  ofSetBackgroundAuto(false);
  ofEnableAntiAliasing();
  ofEnableSmoothing();

  // ==================== MULTIPLE TRACKS SETUP ====================
  // Kita buat 3 lintasan konsentris (Outer, Middle, Inner)

  float w = ofGetWidth();
  float h = ofGetHeight();
  float cx = w / 2;
  float cy = h / 2;

  // Track 1: Outer (Besar)
  // Margin kecil (misal 50)
  {
    TrackInstance t;
    // Bounds: full screen minus margin
    ofRectangle bounds(50, 50, w - 100, h - 100);
    // Spawn mobil dengan maxVOuter, spiralMaxVOuter, maxCellsOuter, dll
    t.setup(bounds, numCarsOuter, 50, maxVOuter, spiralMaxVOuter, probSlowOuter, maxCellsOuter, currentRoadType,
            numLinesPerCarOuter, curveIntensityOuter, curveAngle1Outer, curveAngle2Outer, directionOuter);
    t.visible = true;  // Default visible
    t.drawFromCenter = (ofRandom(1.0f) < 0.5f);  // Random: center→car atau car→center
    t.gradientMode = false;  // Default: normal mode
    tracks.push_back(t);
  }

  // Track 2: Middle (Sedang)
  // Margin lebih besar (misal 200)
  {
    TrackInstance t;
    ofRectangle bounds(200, 200, w - 400, h - 400);
    // Spawn mobil dengan maxVMiddle, spiralMaxVMiddle, maxCellsMiddle, dll
    t.setup(bounds, numCarsMiddle, 50, maxVMiddle, spiralMaxVMiddle, probSlowMiddle, maxCellsMiddle, currentRoadType,
            numLinesPerCarMiddle, curveIntensityMiddle, curveAngle1Middle, curveAngle2Middle, directionMiddle);
    t.visible = true;  // Default visible
    t.drawFromCenter = (ofRandom(1.0f) < 0.5f);  // Random: center→car atau car→center
    t.gradientMode = false;  // Default: normal mode
    tracks.push_back(t);
  }

  // Track 3: Inner (Kecil)
  // Margin lebih besar lagi (misal 350)
  {
    TrackInstance t;
    ofRectangle bounds(350, 350, w - 700, h - 700);
    // Spawn mobil dengan maxVInner, spiralMaxVInner, maxCellsInner, dll
    t.setup(bounds, numCarsInner, 45, maxVInner, spiralMaxVInner, probSlowInner, maxCellsInner, currentRoadType,
            numLinesPerCarInner, curveIntensityInner, curveAngle1Inner, curveAngle2Inner, directionInner);
    t.visible = true;  // Default visible
    t.drawFromCenter = (ofRandom(1.0f) < 0.5f);  // Random: center→car atau car→center
    t.gradientMode = false;  // Default: normal mode
    tracks.push_back(t);
  }
}

//--------------------------------------------------------------
void ofApp::update() {
  // Hanya update jika simulasi sudah dimulai
  if (!simulationStarted) {
    return;
  }

  for (auto &track : tracks) {
    track.update();
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofSetBackgroundAuto(false);

  // Jika simulasi belum mulai, layar tetap hitam
  if (!simulationStarted) {
    ofSetColor(0);
    ofFill();
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    return;
  }

  // Gambar trail effect dan scene
  ofSetColor(0, 15);
  ofFill();
  ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

  // Hitung wobble time untuk bezier curves
  float wobbleTime = ofGetElapsedTimef() * .5f;  // Kecepatan wobble

  for (auto &track : tracks) {
    // Hanya draw jika visible
    if (track.visible) {
      track.draw(&ofApp::getBezierPoint, wobbleTime, track.gradientMode);
    }
  }
}

// ==================== TRACK INSTANCE IMPLEMENTATION ====================

void ofApp::TrackInstance::setup(ofRectangle bounds, int numCars, int spacing,
                                 float maxV, float spiralMaxV, float probSlow, int maxCells, RoadType roadType,
                                 int numLinesPerCar, float curveIntensity, float curveAngle1, float curveAngle2, int direction) {
  this->bounds = bounds;
  this->roadType = roadType;          // Simpan roadType untuk cek SpiralRoad
  this->maxCells = maxCells;
  this->maxV = maxV;                  // Simpan maxV untuk normal mode
  this->spiralMaxV = spiralMaxV;      // Simpan maxV khusus SpiralRoad
  this->numLinesPerCar = numLinesPerCar;  // Simpan numLinesPerCar untuk track ini
  this->curveIntensity = curveIntensity;  // Simpan curveIntensity untuk track ini
  this->curveAngle1 = curveAngle1;        // Simpan curveAngle1 untuk track ini
  this->curveAngle2 = curveAngle2;        // Simpan curveAngle2 untuk track ini
  this->direction = direction;            // Simpan direction untuk track ini

  // 1. Road - buat berdasarkan roadType
  regenerateRoad(roadType);

  // 2. Grid
  grid.resize(maxCells);

  // 3. Traffic
  for (int i = 0; i < numCars; i++) {
    float startDist = i * spacing;

    vec3 color = vec3(ofRandom(1.0f), ofRandom(1.0f), ofRandom(1.0f));

    traffic.push_back(std::make_shared<SedanCar>(startDist, 0.005f, color,
                                                 maxCells, maxV, probSlow));
  }

  // 4. Set velocity berdasarkan roadType (HARUS SETELAH traffic dibuat!)
  if (roadType == SPIRAL) {
    // Gunakan kecepatan SpiralRoad
    for (auto &vehicle : traffic) {
      vehicle->setMaxVelocity(this->spiralMaxV);
      vehicle->setVelocity(this->spiralMaxV);
    }
  }
  // Untuk road type lain, velocity sudah diset dari maxV di constructor SedanCar
}

void ofApp::TrackInstance::regenerateRoad(RoadType roadType) {
  // Update roadType
  this->roadType = roadType;

  // Buat road baru berdasarkan tipe
  if (roadType == CIRCLE) {
    road = std::make_shared<CircleRoad>();
    // Restore kecepatan normal
    for (auto &vehicle : traffic) {
      vehicle->setMaxVelocity(this->maxV);
    }
  } else if (roadType == CURVED) {
    road = std::make_shared<CurvedRoad>();
    // Restore kecepatan normal
    for (auto &vehicle : traffic) {
      vehicle->setMaxVelocity(this->maxV);
    }
  } else if (roadType == PERLIN_NOISE) {
    road = std::make_shared<PerlinNoiseRoad>();
    // Restore kecepatan normal
    for (auto &vehicle : traffic) {
      vehicle->setMaxVelocity(this->maxV);
    }
  } else {  // SPIRAL
    road = std::make_shared<SpiralRoad>();

    // ===== GUNAKAN KECEPATAN KHUSUS SPIRALROAD =====
    // Setiap track punya maxV sendiri untuk SpiralRoad
    // PENTING: Juga set velocity saat ini, bukan cuma maxV!
    for (auto &vehicle : traffic) {
      vehicle->setMaxVelocity(this->spiralMaxV);
      vehicle->setVelocity(this->spiralMaxV);  // Reset velocity saat ini juga!
    }
  }

  // Generate path dengan bounds yang tersimpan
  road->generatePath(bounds);
}

void ofApp::TrackInstance::update() {
  // 0. Hapus vehicles yang ditandai untuk dihapus (SpiralRoad black hole)
  if (!vehiclesToRemove.empty()) {
    // Sort descending untuk menghapus dari indeks terbesar (aman!)
    std::sort(vehiclesToRemove.begin(), vehiclesToRemove.end(), std::greater<int>());

    // Hapus dari vector (dari indeks terbesar ke terkecil)
    for (int idx : vehiclesToRemove) {
      if (idx < traffic.size()) {
        traffic.erase(traffic.begin() + idx);
      }
    }

    // Clear list untuk frame berikutnya
    vehiclesToRemove.clear();
  }

  // 1. Reset Grid
  grid.assign(maxCells, -1);

  // 2. Map Vehicles to Grid (NORMAL untuk SEMUA direction)
  for (int i = 0; i < traffic.size(); i++) {
    int pos = (int)traffic[i]->getDistance();
    pos = pos % maxCells;
    grid[pos] = i;
  }

  // 3. Set Grid to Vehicles
  for (auto &vehicle : traffic) {
    vehicle->setGrid(grid.data(), grid.size());
  }

  // 4. Update Vehicles & Segments
  for (auto &vehicle : traffic) {
    vehicle->update();

    // Update SedanCar segment positions (untuk physics simulation)
    SedanCar *car = dynamic_cast<SedanCar *>(vehicle.get());
    if (car) {
      auto &segments = car->getSegmentDistances();

      // A. Head check
      segments[0] = car->getDistance();

      // B. Follow logic
      float time = ofGetElapsedTimef() * 6.0f;
      for (size_t j = 1; j < segments.size(); j++) {
        float leaderDist = segments[j - 1];
        float followerDist = segments[j];
        float targetSpacing = 5.0f + sin(time - j * 0.5f) * 2.0f;

        float diff = leaderDist - followerDist;
        if (diff < -maxCells / 2.0f)
          diff += maxCells;
        if (diff > maxCells / 2.0f)
          diff -= maxCells;

        float actualSpacing = diff;
        float spacingError = actualSpacing - targetSpacing;
        float moveAmt = spacingError * 0.2f;

        segments[j] += moveAmt;

        // Range check
        if (segments[j] >= maxCells)
          segments[j] -= maxCells;
        if (segments[j] < 0)
          segments[j] += maxCells;
      }

      // C. Convert to World Points using THIS track's road
      std::vector<vec2> bodyPoints;
      float roadLen = road->getTotalLength();

      for (float d : segments) {
        float worldD = d * (roadLen / maxCells);

        // Jika direction -1, reverse distance untuk world positioning
        if (direction == -1) {
          worldD = roadLen - worldD;
        }

        bodyPoints.push_back(road->getPointAtDistance(worldD));
      }

      car->updateBody(bodyPoints);
    }
  }
}

void ofApp::TrackInstance::draw(ofPoint (bezierHelper)(float, ofPoint, ofPoint, ofPoint, ofPoint), float wobbleTime, bool gradientMode) {
  // TEST: Gambar road polyline untuk lihat apa yang terjadi
  //road->draw();

  // Draw Vehicles
  for (auto &vehicle : traffic) {
    float dist = vehicle->getDistance() * (road->getTotalLength() / maxCells);

    // Jika direction = -1 (clockwise), reverse distance
    if (direction == -1) {
      dist = road->getTotalLength() - dist;
    }

    vec2 pos = road->getPointAtDistance(dist);
    vec2 tangent = road->getTangentAtDistance(dist);
    float angle = ofRadToDeg(atan2(tangent.y, tangent.x));

    // ===== SPIRAL ROAD BLACK HOLE EFFECT =====
    // Check apakah mobil di GAP area (SpiralRoad only)
    vec2 carPos(pos.x, pos.y);

    // Pakai center dari bounds, bukan screen center (untuk multi-track)
    vec2 trackCenter(bounds.x + bounds.width / 2.0f, bounds.y + bounds.height / 2.0f);
    float radius = glm::length(carPos - trackCenter);

    bool inBlackHole = false;  // Flag untuk cek apakah di black hole

    if (roadType == SPIRAL) {
      // Threshold GAP: Absolute 150 pixels (SEMUA inner track masuk black hole!)
      float gapThreshold = 100.0f;

      if (radius < gapThreshold) {
        inBlackHole = true;  // Masuk black hole!

        // Mark vehicle untuk DIHAPUS (cari indeksnya di traffic vector)
        for (int i = 0; i < traffic.size(); i++) {
          if (traffic[i] == vehicle) {
            vehiclesToRemove.push_back(i);  // Tambah ke list untuk dihapus
            break;  // Sudah ketemu, keluar loop
          }
        }
      }
    }

    // Skip drawing vehicle jika gradient mode aktif
    if (!gradientMode) {
      if (inBlackHole) {
        // Gambar mobil dengan WARNA HITAM (merge dengan background)
        ofSetColor(0, 0, 0);  // Hitam total
        vehicle->draw(pos.x, pos.y, angle);
      } else {
        vehicle->draw(pos.x, pos.y, angle);
      }
    }

    // Gambar MULTIPLE garis radial bezier
    ofPoint centerPoint(trackCenter.x, trackCenter.y);  // Convert vec2 ke ofPoint
    float angleToCar = atan2(carPos.y - centerPoint.y, carPos.x - centerPoint.x);
    vec3 col = vehicle->getColor();

    // Jika di black hole, bezier juga HITAM
    if (inBlackHole) {
      col = vec3(0, 0, 0);  // Hitam
    }

    // Hitung radius mobil (berdasarkan velocity)
    float v = vehicle->getVelocity();
    float carDiameter = ofMap(v, 0, 15, 20, 10);  // Diameter mobil (10-20 pixel)
    float carRadius = carDiameter / 2.0f;            // Radius mobil
    float gap = 5.0f;  // Gap 5 pixel dari pinggir mobil

    // Loop untuk setiap garis
    for(int lineIdx = 0; lineIdx < numLinesPerCar; lineIdx++) {
      // Offset angle untuk posisi tersebar merata di sekeliling mobil
      float spreadAngle = lineIdx * (TWO_PI / numLinesPerCar);

      // Titik di pinggir luar mobil
      ofPoint carEdgePoint(carPos.x + cos(spreadAngle) * (carRadius + gap),
                           carPos.y + sin(spreadAngle) * (carRadius + gap));

      // Tentukan P0 dan P3 berdasarkan drawFromCenter
      ofPoint p0, p3;
      if (drawFromCenter) {
        // Dari center ke car
        p0 = centerPoint;
        p3 = carEdgePoint;
      } else {
        // Dari car ke center
        p0 = carEdgePoint;
        p3 = centerPoint;
      }

      // Control points: P1 dan P2
      // Untuk S-CURVE (lengkungan ganda), P1 dan P2 melengkung ke arah BERLAWANAN
      float curveAmount = radius * curveIntensity;

      // Hitung angle untuk garis ini (dari pusat ke P3)
      float lineAngle = atan2(p3.y - p0.y, p3.x - p0.x);

      // Wobble besar supaya pasti kelihatan
      float wobble1 = sin(wobbleTime * 3.0f + lineIdx * 0.5f) * 85.0f;
      float wobble2 = cos(wobbleTime * 3.0f + lineIdx * 0.5f) * 85.0f;

      // P1 di posisi dengan angle offset + DRAMATIC WOBBLE
      ofPoint p1(p0.x + cos(lineAngle + curveAngle1) * (curveAmount + wobble1),
                 p0.y + sin(lineAngle + curveAngle1) * (curveAmount + wobble1));

      // P2 di posisi dengan angle offset + DRAMATIC WOBBLE
      ofPoint p2(p0.x + cos(lineAngle + curveAngle2) * (curveAmount + wobble2),
                 p0.y + sin(lineAngle + curveAngle2) * (curveAmount + wobble2));

      // Tessellate bezier curve
      int segments = 100;

      if (gradientMode) {
        // GRADIENT MODE: Hitam ke Orange dengan vertex colors
        ofMesh bezierMesh;
        bezierMesh.setMode(OF_PRIMITIVE_LINE_STRIP);

        for(int k = 0; k <= segments; k++) {
          float t = (float)k / segments;
          ofPoint p = bezierHelper(t, p0, p1, p2, p3);
          bezierMesh.addVertex(glm::vec3(p.x, p.y, 0.0f));

          // Gradient dari putih (255,255,255) ke gelap (0,0,0)
          float val = ofMap(t, 0.0f, 1.0f, 255.0f, 0.0f);  // Reverse: 255→0
          bezierMesh.addColor(ofColor(val, val, val, 150));  // Grayscale gradient, Alpha 200
        }

        // Gambar mesh dengan gradient
        bezierMesh.draw();
      } else {
        // NORMAL MODE: Single color dengan alpha variation
        ofPolyline bezierPolyline;

        for(int k = 0; k <= segments; k++) {
          float t = (float)k / segments;
          ofPoint p = bezierHelper(t, p0, p1, p2, p3);
          bezierPolyline.addVertex(p.x, p.y);
        }

        // Alpha bervariasi per garis
        float alpha = ofMap(lineIdx, 0, numLinesPerCar - 1, 80, 150);
        ofSetColor(col.r * 255, col.g * 255, col.b * 255, alpha);

        // Gambar bezier polyline sebagai garis kontinu
        ofSetLineWidth(3);
        bezierPolyline.draw();
      }
    }
  }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
  // Mulai simulasi dengan tombol 's' atau 'S'
  if (key == 's' || key == 'S') {
    simulationStarted = true;
  }

  // Switch road type: '1' = CircleRoad, '2' = CurvedRoad
  if (key == '1') {
    currentRoadType = CIRCLE;
    // Regenerate semua track dengan CircleRoad
    for (auto &track : tracks) {
      ofBackground(0);
      track.regenerateRoad(currentRoadType);
    }
  }

  if (key == '2') {
    currentRoadType = CURVED;
    // Regenerate semua track dengan CurvedRoad
    for (auto &track : tracks) {
      ofBackground(0);
      track.regenerateRoad(currentRoadType);
    }
  }

  if (key == '3') {
    currentRoadType = PERLIN_NOISE;
    // Regenerate semua track dengan PerlinNoiseRoad
    for (auto &track : tracks) {
      ofBackground(0);
      track.regenerateRoad(currentRoadType);
    }
  }

  if (key == '4') {
    currentRoadType = SPIRAL;
    // Regenerate semua track dengan SpiralRoad
    for (auto &track : tracks) {
      ofBackground(0);
      track.regenerateRoad(currentRoadType);
    }
  }

  // Kontrol curveIntensity untuk track luar (outer)
  if (key == '=' || key == '+') {  // '=' biasanya '+' tanpa shift
    curveIntensityOuter += 0.1f;
    if (!tracks.empty())tracks[0].curveIntensity = curveIntensityOuter;
    
  }

  if (key == '-' || key == '_') {  // '_' biasanya '-' tanpa shift
      curveIntensityOuter -= 0.1f;
      if (!tracks.empty()) tracks[0].curveIntensity = curveIntensityOuter;
      
  }

  if (key == ']' || key == '}') {
      curveIntensityMiddle += .1f;
      if (!tracks.empty()) tracks[1].curveIntensity = curveIntensityMiddle;
  }

  if (key == '[' || key == '{') {
      curveIntensityMiddle -= .1f;
      if (!tracks.empty()) tracks[1].curveIntensity = curveIntensityMiddle;
  }

  if (key == '.' || key == '>') {
      curveIntensityInner += .1f;
      if (!tracks.empty()) tracks[2].curveIntensity = curveIntensityInner;
  }

  if (key == ',' || key == '<') {
      curveIntensityInner -= .1f;
      if (!tracks.empty()) tracks[2].curveIntensity = curveIntensityInner;
  }

  // Toggle visibility track outer dengan 'Z' atau 'z'
  if (key == 'z' || key == 'Z') {
    if (!tracks.empty()) {
      tracks[0].visible = !tracks[0].visible;  // Toggle outer track
    }
  }

  // Toggle visibility track middle dengan 'X' atau 'x'
  if (key == 'x' || key == 'X') {
    if (tracks.size() > 1) {
      tracks[1].visible = !tracks[1].visible;  // Toggle middle track
    }
  }

  // Toggle visibility track inner dengan 'C' atau 'c'
  if (key == 'c' || key == 'C') {
    if (tracks.size() > 2) {
      tracks[2].visible = !tracks[2].visible;  // Toggle inner track
    }
  }

  // Reset simulasi dengan 'R' atau 'r'
  if (key == 'r' || key == 'R') {
    tracks.clear();  // Hapus semua track lama
    setup();         // Buat ulang semua track, mobil, dan bezier
  }

  // Toggle gradient mode untuk OUTER track dengan 'T' atau 't'
  if (key == 't' || key == 'T') {
    if (!tracks.empty()) {
      tracks[0].gradientMode = !tracks[0].gradientMode;
    }
  }

  // Toggle gradient mode untuk MIDDLE track dengan 'Y' atau 'y'
  if (key == 'y' || key == 'Y') {
    if (tracks.size() > 1) {
      tracks[1].gradientMode = !tracks[1].gradientMode;
    }
  }

  // Toggle gradient mode untuk INNER track dengan 'U' atau 'u'
  if (key == 'u' || key == 'U') {
    if (tracks.size() > 2) {
      tracks[2].gradientMode = !tracks[2].gradientMode;
    }
  }

  // Keluar dengan tombol 'q' atau 'Q'
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

//--------------------------------------------------------------
ofPoint ofApp::getBezierPoint(float t, ofPoint p0, ofPoint p1, ofPoint p2, ofPoint p3) {
	// Cubic bezier formula: B(t) = (1-t)³P0 + 3(1-t)²tP1 + 3(1-t)t²P2 + t³P3
	float u = 1.0f - t;
	float tt = t * t;
	float uu = u * u;
	float uuu = uu * u;
	float ttt = tt * t;

	ofPoint p;
	p.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
	p.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;

	return p;
}
