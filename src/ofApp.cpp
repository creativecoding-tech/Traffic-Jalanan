#include "ofApp.h"
#include "road/CurvedRoad.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::setup() {
  ofSetVerticalSync(false);
  ofSetFrameRate(60);
  ofSetEscapeQuitsApp(false);
  ofSetBackgroundAuto(false);
  ofDisableAlphaBlending();
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
    // Spawn mobil dari parameter numCarsOuter, probSlowOuter, maxCells
    t.setup(bounds, numCarsOuter, 50, maxV, probSlowOuter, maxCells);
    tracks.push_back(t);
  }

  // Track 2: Middle (Sedang)
  // Margin lebih besar (misal 200)
  {
    TrackInstance t;
    ofRectangle bounds(200, 200, w - 400, h - 400);
    // Spawn mobil dari parameter numCarsMiddle, probSlowMiddle, maxCells
    t.setup(bounds, numCarsMiddle, 50, maxV * 0.8f, probSlowMiddle, maxCells);
    tracks.push_back(t);
  }

  // Track 3: Inner (Kecil)
  // Margin lebih besar lagi (misal 350)
  {
    TrackInstance t;
    ofRectangle bounds(350, 350, w - 700, h - 700);
    // Spawn mobil dari parameter numCarsInner, probSlowInner, maxCells
    t.setup(bounds, numCarsInner, 45, maxV * 0.6f, probSlowInner, maxCells);
    tracks.push_back(t);
  }
}

//--------------------------------------------------------------
void ofApp::update() {
  for (auto &track : tracks) {
    track.update();
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofSetBackgroundAuto(false);
  ofSetColor(0, 15);
  ofFill();
  ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

  for (auto &track : tracks) {
    track.draw(curveIntensity, numLinesPerCar, &ofApp::getBezierPoint);
  }
}

// ==================== TRACK INSTANCE IMPLEMENTATION ====================

void ofApp::TrackInstance::setup(ofRectangle bounds, int numCars, int spacing,
                                 float maxV, float probSlow, int maxCells) {
  this->maxCells = maxCells; // Gunakan parameter maxCells, bukan hardcoded 300

  // 1. Road
  road = std::make_shared<CurvedRoad>();
  road->generatePath(bounds);

  // 2. Grid
  grid.resize(maxCells);

  // 3. Traffic
  for (int i = 0; i < numCars; i++) {
    float startDist = i * spacing;

    // Warna random unik tiap track? Atau random total?
    // Random total aja biar estetik warna warni
    vec3 color = vec3(ofRandom(1.0f), ofRandom(1.0f), ofRandom(1.0f));

    traffic.push_back(std::make_shared<SedanCar>(startDist, 0.005f, color,
                                                 maxCells, maxV, probSlow));
  }
}

void ofApp::TrackInstance::update() {
  // 1. Reset Grid
  grid.assign(maxCells, -1);

  // 2. Map Vehicles to Grid
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

    // Update SedanCar specific logic (Body Segments)
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
        bodyPoints.push_back(road->getPointAtDistance(worldD));
      }

      car->updateBody(bodyPoints);
    }
  }
}

void ofApp::TrackInstance::draw(float curveIntensity, int numLinesPerCar, ofPoint (bezierHelper)(float, ofPoint, ofPoint, ofPoint, ofPoint)) {
  // Draw Road
  road->draw();

  // Draw Vehicles
  for (auto &vehicle : traffic) {
    float dist = vehicle->getDistance() * (road->getTotalLength() / maxCells);
    vec2 pos = road->getPointAtDistance(dist);
    vec2 tangent = road->getTangentAtDistance(dist);
    float angle = ofRadToDeg(atan2(tangent.y, tangent.x));

    vehicle->draw(pos.x, pos.y, angle);

    // Gambar MULTIPLE garis radial bezier dari center screen ke mobil
    vec2 carPos(pos.x, pos.y);
    ofPoint p0(ofGetWidth() / 2, ofGetHeight() / 2);
    float angleToCar = atan2(carPos.y - p0.y, carPos.x - p0.x);
    float radius = glm::length(carPos - vec2(p0.x, p0.y));
    vec3 col = vehicle->getColor();

    // Loop untuk setiap garis
    for(int lineIdx = 0; lineIdx < numLinesPerCar; lineIdx++) {
      // Hitung offset angle untuk setiap garis (spread sekitar angleToCar)
      float angleOffset = ofMap(lineIdx, 0, numLinesPerCar - 1,
                                 -PI / 6, PI / 6); // Spread 60 derajat total
      float lineAngle = angleToCar + angleOffset;

      // Curve amount (bervariasi sedikit per garis)
      float curveAmount = radius * curveIntensity * (1.0f + lineIdx * 0.1f);

      // Control points untuk bezier dengan variasi angle
      ofPoint p1(cos(lineAngle + HALF_PI) * curveAmount + p0.x,
                 sin(lineAngle + HALF_PI) * curveAmount + p0.y);
      ofPoint p2(p1.x, p1.y);
      ofPoint p3(carPos.x, carPos.y);

      // Tessellate bezier curve
      ofPolyline bezierPolyline;
      int segments = 30;

      for(int k = 0; k <= segments; k++) {
        float t = (float)k / segments;
        ofPoint p = bezierHelper(t, p0, p1, p2, p3);
        bezierPolyline.addVertex(p.x, p.y);
      }

      // Gambar per segment dengan efek pulsing (offset per garis)
      for(int k = 0; k < bezierPolyline.size() - 1; k++) {
        float t = (float)k / bezierPolyline.size();
        // Pulse offset berbeda untuk setiap garis biar tidak sinkron
        float pulse = sin(t * TWO_PI + ofGetFrameNum() * 0.05f + lineIdx * 0.5f);
        float lineWidth = ofMap(pulse, -1, 1, 1, 3);
        ofSetLineWidth(lineWidth);

        // Alpha bervariasi per garis (garis tengah lebih solid)
        float alpha = ofMap(lineIdx, 0, numLinesPerCar - 1, 80, 150);
        ofSetColor(col.r * 255, col.g * 255, col.b * 255, alpha);
        ofDrawLine(bezierPolyline[k], bezierPolyline[k + 1]);
      }
    }
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
