#pragma once
#include "ofMain.h"
#include <glm/glm.hpp>
#include <vector>

// namespace alias
using glm::vec2;
using glm::vec3;

/**
 * Road - Abstract Base Class untuk Road Systems
 *
 * Ini adalah base class untuk semua jenis jalan:
 * - CurvedRoad (jalan melengkung smooth dengan ofPolyline)
 * - PerlinNoiseRoad (jalan organik dengan noise)
 * - HighwayRoad (jalan bebas lurus dengan ramps)
 * - ProceduralRoad (jalan hasil procedural generation)
 * - dll
 *
 * Class ini mendefinisikan interface yang WAJIB ada di semua
 * jenis road, tapi tidak mengimplementasi detail-nya.
 *
 * Design Pattern: Template Method + Strategy
 * - Polymorphism: Bisa ganti jenis road di runtime
 * - Flexibility: Mudah tambah jenis road baru
 */
class Road {
public:
  // Constructor
  Road() = default;

  // Destructor virtual (WAJIB untuk polymorphism)
  virtual ~Road() = default;

  /**
   * Generate Path
   *
   * Method ini WAJIB di-override oleh child class.
   *
   * Fungsi: Generate jalur/jalan (path)
   *
   * Implementasi child class:
   * - CurvedRoad: ofPolyline dengan curveTo()
   * - PerlinNoiseRoad: Generate titik dengan perlin noise
   * - HighwayRoad: Garis lurus dengan ramps
   *
   * Setelah generatePath(), path dan polyline harus siap digunakan.
   */
  virtual void generatePath(ofRectangle bounds) = 0;

  /**
   * Get Point at Distance
   *
   * Method ini WAJIB di-override oleh child class.
   *
   * Fungsi: Mapping dari distance (dalam pixels) ke posisi (x,y)
   *
   * @param dist Distance dalam pixels
   * @return vec2 Posisi (x,y) di road
   */
  virtual vec2 getPointAtDistance(float dist) = 0;

  /**
   * Get Tangent at Distance
   *
   * Method ini WAJIB di-override oleh child class.
   *
   * Fungsi: Menghitung arah (tangent) road pada distance tertentu.
   *        Berguna untuk rotasi vehicle.
   *
   * @param dist Distance dalam pixels
   * @return vec2 Unit vector (normalized) yang menunjuk arah road
   */
  virtual vec2 getTangentAtDistance(float dist) = 0;

  /**
   * Draw Road
   *
   * Render road ke layar.
   * Default implementation: panggil polyline.draw()
   * Child class bisa override untuk custom rendering.
   *
   * @param offsetX Offset X (untuk posisi di layar)
   * @param offsetY Offset Y (untuk posisi di layar)
   */
  virtual void draw(float offsetX = 0.0f, float offsetY = 0.0f);

  /**
   * Get Total Length
   *
   * @return Total panjang road dalam pixels
   */
  virtual float getTotalLength() const { return totalLength; }

  /**
   * Get Path
   *
   * @return Reference ke path vector (const)
   *
   * Note: Return const reference karena:
   * - Tidak boleh modify dari luar class
   * - Efisien (tidak copy)
   */
  virtual const std::vector<vec2> &getPath() const { return path; }

  /**
   * Get Polyline
   *
   * @return Reference ke polyline (const)
   *
   * Note: ofPolyline adalah class OF untuk rendering
   */
  virtual const ofPolyline &getPolyline() const { return polyline; }

protected:
  /**
   * path - Vector berisi titik-titik yang menyusun road
   *
   * Type: vector<glm::vec2>
   *
   * Fungsi:
   * - Untuk math calculations (getPointAtDistance, dll)
   * - Di-generate oleh child class di generatePath()
   * - Bisa juga dibuat manual (hardcoded titik-titik)
   *
   * Contoh isi:
   * - CurvedRoad: Hasil interpolasi ofPolyline (smooth curve)
   * - PerlinNoiseRoad: Titik-titik hasil noise function
   * - HighwayRoad: Garis lurus dengan corner
   */
  std::vector<vec2> path;

  /**
   * polyline - ofPolyline untuk rendering
   *
   * Type: ofPolyline
   *
   * Fungsi:
   * - Render ke layar (polyline.draw())
   * - Hitung perimeter (totalLength)
   * - Bisa juga dipakai untuk getPointAtLength() bawaan OF
   *
   * Note:
   * - ofPolyline milik openFrameworks
   * - Tidak menggunakan glm::vec2, tapi ofPoint/ofVec3f
   * - Perlu konversi glm::vec2 → ofPoint saat addVertex()
   */
  ofPolyline polyline;

  /**
   * totalLength - Total panjang road dalam pixels
   *
   * Fungsi:
   * - Untuk wrapping distance (distance % totalLength)
   * - Untuk normalisasi posisi
   *
   * Dihitung di generatePath() oleh child class:
   * - CurvedRoad: polyline.getPerimeter()
   * - Manual: sum of jarak antar titik
   */
  float totalLength;
};
