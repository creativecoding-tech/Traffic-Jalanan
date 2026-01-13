#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(false);
	ofSetFrameRate(60); //bisa disesuaikan dengan FPS monitor, tidak harus sama
	ofSetEscapeQuitsApp(false);
	ofSetBackgroundAuto(false);
	ofDisableAlphaBlending();
	ofEnableAntiAliasing();
	ofEnableSmoothing();

    // ==================== ROAD SETUP ====================

         // Road length = lebar layar - margin (100px kiri-kanan)
    roadLength = ofGetWidth() - 100;

    // Road position = tengah vertikal layar
    roadY = ofGetHeight() / 2.0f;

    // ==================== GRID SETUP ====================

    // Resize grid ke maxCells (300)
    grid.resize(maxCells);

    // ==================== SPAWN VEHICLES ====================

    // Spawn 40 mobil dengan jarak 7 cells
    int numCars = 40;
    int spacing = 7;  // Jarak antar mobil (cells)

    for (int i = 0; i < numCars; i++) {
        // Hitung posisi awal (jarak 7 cells antar mobil)
        float startDist = i * spacing;

        // Kecepatan awal random (0-5)
        int startV = (int)ofRandom(maxV + 1);  // +1 supaya maxV ikut

        // Warna random (RGB 0-1)
        vec3 color = vec3(
            ofRandom(1.0f),  // R
            ofRandom(1.0f),  // G
            ofRandom(1.0f)   // B
        );

        // Buat SedanCar dan tambahkan ke traffic
        traffic.push_back(std::make_unique<SedanCar>(
            startDist,    // Posisi awal
            startV,       // Kecepatan awal
            color,        // Warna
            maxCells,     // maxCells (300)
            maxV,         // maxV (5)
            probSlow      // probSlow (0.2)
        ));
    }
}

//--------------------------------------------------------------
void ofApp::update(){
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

    // ==================== STEP 2: SET GRID KE SEMUA VEHICLES ====================

    // Set grid pointer ke semua vehicles
    for (int i = 0; i < traffic.size(); i++) {
        // Pass grid pointer ke vehicle (yang akan pass ke strategy)
        traffic[i]->setGrid(grid.data(), grid.size());
    }

    // ==================== STEP 3: UPDATE SEMUA VEHICLES ====================

    // Update semua vehicles (movement logic)
    for (auto& vehicle : traffic) {
        vehicle->update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // ==================== FADE EFFECT (TRAIL) ====================

    // Gambar semi-transparent black overlay
    ofSetColor(0, 15);  // Hitam dengan alpha 15/255 (~6%)
    ofFill();
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

    // Gambar garis putus-putus setiap 30 pixels
    for (float x = 50; x < 50 + roadLength; x += 30) {
        ofDrawLine(x, roadY, x + 15, roadY);  // Garis pendek 15px
    }

    // ==================== DRAW VEHICLES ====================

    // Translate origin ke kiri layar + margin
    ofPushMatrix();
    ofTranslate(50, roadY);

    // Gambar semua vehicles
    for (auto& vehicle : traffic) {
        // Mapping distance linear → X coordinate
        float xPos = ofMap(
            vehicle->getDistance(),  // Input: distance (0-299)
            0, maxCells,              // Input range
            0, roadLength,            // Output range
            false                     // Clamp ke range (false = bisa lewat)
        );

        // Y position = 0 (karena kita sudah translate ke roadY)
        float yPos = 0;

        // Angle = 0 (linear road horizontal, tidak perlu rotasi)
        float angle = 0;

        // Gambar vehicle
        vehicle->draw(xPos, yPos, angle);
    }

    ofPopMatrix();

    // ==================== OPTIONAL: DEBUG INFO ====================

    // Tampilkan jumlah vehicles (opsional, bisa dihapus nanti)
    ofSetColor(255);
    ofDrawBitmapString("Vehicles: " + ofToString(traffic.size()), 10, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'q' || key == 'Q') ofExit();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
