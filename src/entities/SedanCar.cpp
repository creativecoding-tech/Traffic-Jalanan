#include "SedanCar.h"
#include "ofMain.h"

/**
 * Constructor
 *
 * Initialize SedanCar dengan parent constructor dan buat strategy.
 */
SedanCar::SedanCar(float startDist, float velocity, vec3 color, int maxCells, float maxV, float probSlow)
    : Vehicle(startDist, velocity, color)
    , storedMaxCells(maxCells)
    , storedMaxV(maxV)
{
    movementStrat = std::make_unique<NaSchMovement>(maxCells, maxV, probSlow);
}


/**
 * Override update() dari Vehicle
 *
 * Method ini men-delegate movement logic ke strategy.
 *
 * Flow:
 * 1. Cek apakah movementStrat sudah di-set
 * 2. Kalau sudah, panggil movementStrat->update(*this)
 * 3. Strategy akan menjalankan 4 aturan Nagel-Schreckenberg:
 *    - Accelerate
 *    - Brake (dengan grid lookup)
 *    - Randomize (20% kemungkinan)
 *    - Move (dengan wrapping)
 */
void SedanCar::update() {
    // Pastikan strategy sudah di-set
    if (movementStrat) {
        // Delegate ke strategy
        movementStrat->update(*this);
    }
}

/**
  * Override draw() dari Vehicle
  *
  * Method ini menggambar SedanCar ke layar.
  *
  * Visualisasi:
  * - Mobil digambar sebagai rectangle (kotak)
  * - Size berbanding terbalik dengan kecepatan (lambat = besar, cepat = kecil)
  * - Warna merah kalau macet (v = 0)
  * - Rotasi sesuai arah road
  *
  * @param xPos Posisi X di layar
  * @param yPos Posisi Y di layar
  * @param angle Rotasi dalam derajat (0 = horizontal ke kanan)
  */
void SedanCar::draw(float xPos, float yPos, float angle) {
    // Ambil data dari vehicle
    float currentV = getVelocity();
    vec3 col = getColor();

    // Hitung size berdasarkan kecepatan
    // Contoh value
    // v = 0 (macet)  → size besar (20px)
    // v = 5 (ngebut) → size kecil (8px)
    float size = ofMap(currentV, 0.0f, storedMaxV, 20.0f, 40.0f);

    // Tentukan warna render
    ofColor renderCol;
    if (currentV == 0) {
        // Kalau macet (v = 0), warna merah terang
        renderCol = ofColor(255, 0, 0);
    }
    else {
        // Kalau bergerak, pakai warna asli (convert vec3 0-1 ke ofColor 0-255)
        renderCol = ofColor(
            col.r * 255,  // R
            col.g * 255,  // G
            col.b * 255,  // B
            200           // Alpha (semi-transparent)
        );
    }

    // Gambar mobil dengan transformasi (rotasi)
    ofPushMatrix();

    // Pindah origin ke posisi mobil
    ofTranslate(xPos, yPos);

    // Rotasi sesuai arah road
    ofRotateDeg(angle);

    // Set warna
    ofSetColor(renderCol);

    // Gambar mobil sebagai rectangle
    // -size/2, -size/4 → supaya center rectangle di (0, 0)
    // lebar = size, tinggi = size/2 (mobil lebih panjang dari lebar)
    ofDrawRectangle(-size / 2, -size / 4, size, size / 2);

    ofPopMatrix();
}

/**
 * Set Grid untuk Strategy
 *
 * Convenience method untuk memudahkan set grid dari ofApp.
 *
 * @param gridPtr Pointer ke array grid
 * @param gridSize Ukuran grid array
 *
 * Di ofApp:
 * strategy->setGrid(grid.data(), grid.size());
 *
 * Atau lewat method ini:
 * car.setGrid(grid.data(), grid.size());
 */
void SedanCar::setGrid(const int* gridPtr, int gridSize) {
    // Pastikan movementStrat ada dan bertipe NaSchMovement
    if (movementStrat) {
        // Cast ke NaSchMovement* untuk akses setGrid()
        NaSchMovement* naschStrat = dynamic_cast<NaSchMovement*>(movementStrat.get());

        if (naschStrat) {
            // Panggil setGrid() dari strategy
            naschStrat->setGrid(gridPtr, gridSize);
        }
    }
}