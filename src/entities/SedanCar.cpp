#include "SedanCar.h"
#include "ofMain.h"

/**
 * Constructor
 *
 * Initialize SedanCar dengan parent constructor dan buat strategy.
 */
SedanCar::SedanCar(float startDist, float velocity, vec3 color, int maxCells,
                   float maxV, float probSlow)
    : Vehicle(startDist, velocity, color), storedMaxCells(maxCells),
      storedMaxV(maxV) {
  movementStrat = std::make_unique<NaSchMovement>(maxCells, maxV, probSlow);

  // Init segments (misal 15 segment)
  int numSegments = 15;
  for (int i = 0; i < numSegments; i++) {
    // Awalnya semua numpuk di startDist, atau berjejer ke belakang
    segmentDistances.push_back(startDist - (i * 2.0f));
  }
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
  // Gambar body bezier
  drawBody();

  // Tidak ada kotak/rectangle mobil
  // Body = mobil itu sendiri
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
void SedanCar::setGrid(const int *gridPtr, int gridSize) {
  // Pastikan movementStrat ada dan bertipe NaSchMovement
  if (movementStrat) {
    // Cast ke NaSchMovement* untuk akses setGrid()
    NaSchMovement *naschStrat =
        dynamic_cast<NaSchMovement *>(movementStrat.get());

    if (naschStrat) {
      // Panggil setGrid() dari strategy
      naschStrat->setGrid(gridPtr, gridSize);
    }
  }
}

void SedanCar::updateBody(const std::vector<glm::vec2> &newPoints) {
  if (newPoints.empty())
    return;
  bodyPoints = newPoints;
}

/**
 * Draw Simple Circle
 *
 * Gambar mobil sebagai circle sederhana seperti TestModelNaSch.
 * Size berdasarkan velocity (lambat = besar, cepat = kecil).
 * Warna merah jika macet (v = 0).
 */
void SedanCar::drawBody() {
  if (bodyPoints.size() < 1)
    return;

  // Ambil posisi mobil (titik pertama = kepala)
  vec2 pos = bodyPoints[0];

  // Ambil velocity
  float v = getVelocity();

  // Size berdasarkan velocity (lambat = besar, cepat = kecil)
  // maxV disimpan di storedMaxV
  float size = ofMap(v, 0, storedMaxV, 20, 10);

  // Warna: merah jika macet (v ≈ 0), warna mobil jika jalan
  vec3 col = getColor();
  ofColor renderCol;

  if (v < 0.1f) {
    // Macet - warna merah
    renderCol = ofColor(255, 0, 0);
  } else {
    // Jalan - warna mobil
    renderCol = ofColor(col.r * 255, col.g * 255, col.b * 255);
  }

  // Gambar circle dengan alpha 150
  ofSetColor(renderCol, 150);
  ofDrawCircle(pos.x, pos.y, size);
}
