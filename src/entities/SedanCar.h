#pragma once
#include "../strategies/NaSchMovement.h"
#include "Vehicle.h"
#include <memory>

/**
 * SedanCar - Concrete Vehicle (Implementasi Nyata)
 *
 * Ini adalah implementasi concrete dari Vehicle abstract class.
 * SedanCar adalah mobil biasa (sedan) dengan behavior standar
 * Nagel-Schreckenberg.
 *
 * Design Pattern:
 * - Inherits: Vehicle (abstract base)
 * - Uses: NaSchMovement (concrete strategy)
 * - Template Method: update() dan draw() di-override
 *
 * Catatan:
 * - Nama "SedanCar" karena nanti bisa ada jenis lain:
 *   - Motorcycle, Truck, Bus, SportsCar, dll
 * - Semua jenis vehicle akan inherit dari Vehicle
 */
class SedanCar : public Vehicle {
public:
  /**
   * Constructor
   *
   * @param startDist Posisi awal di jalan (distance)
   * @param velocity Kecepatan awal (0.0 - maxV)
   * @param color Warna kendaraan (RGB 0.0 - 1.0)
   * @param maxCells Jumlah total cells (default 600, tapi tiap track beda)
   * @param maxV Kecepatan maksimal (track-specific: outer=20, middle=16, inner=12)
   * @param probSlow Probabilitas random braking (track-specific: 0.02-0.25)
   *
   * Di constructor ini kita:
   * 1. Panggil constructor Vehicle (parent)
   * 2. Buat NaSchMovement strategy
   * 3. Set strategy ke vehicle ini
   */
  SedanCar(float startDist, float velocity, vec3 color, int maxCells = 600,
           float maxV = 20.0f, float probSlow = 0.02f);

  /**
   * Destructor
   *
   * Virtual karena ini polymorphic class
   */
  ~SedanCar() = default;

  /**
   * Override update() dari Vehicle
   *
   * Method ini men-delegate movement logic ke strategy.
   *
   * Flow:
   * 1. Cek apakah movementStrat sudah di-set
   * 2. Kalau sudah, panggil movementStrat->update(*this)
   * 3. Strategy akan menjalankan 4 aturan Nagel-Schreckenberg
   *
   * Ini adalah Template Method Pattern:
   * - Alur update() TETAP (delegation ke strategy)
   * - Detail movement BERBEDA (tergantung strategy)
   */
  void update() override;

  /**
   * Override draw() dari Vehicle
   *
   * Method ini menggambar SedanCar ke layar.
   *
   * Visualisasi:
   * - Mobil digambar sebagai circle sederhana
   * - Size berbanding terbalik dengan kecepatan (lambat = besar, cepat = kecil)
   * - Warna merah kalau macet (v < 0.1)
   *
   * @param xPos X position (dihitung dari road/path)
   * @param yPos Y position (dihitung dari road/path)
   * @param angle Rotation angle (dalam derajat)
   */
  void draw(float xPos, float yPos, float angle = 0);

  /**
   * Setter untuk Grid (di-pass ke strategy)
   *
   * Method ini memudahkan kita set grid ke strategy
   * tanpa perlu akses langsung ke movementStrat.
   *
   * @param gridPtr Pointer ke array grid
   * @param gridSize Ukuran grid array
   */
  void setGrid(const int *gridPtr, int gridSize) override;

  void updateBody(const std::vector<glm::vec2> &newPoints);

  // Getter for physics simulation
  std::vector<float> &getSegmentDistances() { return segmentDistances; }
  void drawBody();

private:
  // Parameter yang disimpan (optional, kalau butuh akses nanti)
  int storedMaxCells; // Disimpan untuk referensi
  float storedMaxV;

  // Body system - points yang menyusun bentuk mobil
  std::vector<vec2> bodyPoints;

  // PHYSICS: Simpan jarak (distance) tiap segment
  // Index 0 = HEAD, Index N = TAIL
  std::vector<float> segmentDistances;
};
