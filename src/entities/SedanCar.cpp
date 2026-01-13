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
    , lastDistance(startDist)
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

void SedanCar::addBodyPoint(vec2 pos) {
    // DETEKSI WRAPPING
    float currentDist = getDistance();

    if (currentDist < lastDistance - 50) {
        // Clear body saat wrapping
        bodyPoints.clear();
    }

    lastDistance = currentDist;

    // Tambahkan point
    bodyPoints.push_back(pos);

    if (bodyPoints.size() > BODY_POINTS) {
        bodyPoints.erase(bodyPoints.begin());
    }
}
/**
 * Draw Bezier Trail
 *
 * Gambar trail dengan bezier curve yang "melilit" seperti ular.
 * Menggunakan teknik dari GridBezier::setBezierMulurLR().
 */
void SedanCar::drawBody() {
    if (bodyPoints.size() < 4) return;

    // Loop setiap segment body
    for (int i = 0; i < bodyPoints.size() - 1; i++) {
        vec2 p0 = bodyPoints[i];
        vec2 p3 = bodyPoints[i + 1];

        // Control points
        vec2 p1 = p0 + vec2(curveIntensity * 10, (p0.y - p3.y) / 2);
        vec2 p2 = p3 - vec2(curveIntensity * 10, (p0.y - p3.y) / 2);

        // Tessellate
        int segments = 20;
        ofPolyline bezierPolyline;

        for (int k = 0; k <= segments; k++) {
            float t = (float)k / segments;
            vec2 pt = getBezierPoint(t, p0, p1, p2, p3);
            bezierPolyline.addVertex(pt.x, pt.y);
        }

        // ==================== LANGSUNG DRAW ====================
        float alpha = ofMap(i, 0, bodyPoints.size() - 1, 0, 150);
        float thickness = ofMap(i, 0, bodyPoints.size() - 1, 1, 6);

        vec3 col = getColor();
        ofSetColor(col.r * 255, col.g * 255, col.b * 255, alpha);
        ofSetLineWidth(thickness);

        bezierPolyline.draw();  // LANGSUNG DRAW, tanpa loop ofDrawLine
    }
}

/**
 * Get Bezier Point
 *
 * Hitung posisi titik di cubic bezier curve pada parameter t (0.0 - 1.0).
 * Formula: B(t) = (1-t)³P0 + 3(1-t)²tP1 + 3(1-t)t²P2 + t³P3
 */
vec2 SedanCar::getBezierPoint(float t, vec2 p0, vec2 p1, vec2 p2, vec2 p3) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    vec2 p;
    p.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
    p.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;

    return p;
}