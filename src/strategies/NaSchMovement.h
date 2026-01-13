#pragma once
#include "MovementStrategy.h"

/**
 * NaSchMovement - Concrete Strategy untuk Nagel-Schreckenberg Model
 *
 * Ini adalah implementasi lengkap dari MovementStrategy dengan 4 aturan
 * Nagel-Schreckenberg:
 * 1. Accelerate: v++
 * 2. Brake: Cek mobil depan via grid
 * 3. Randomize: 20% kemungkinan v--
 * 4. Move: distance += v (dengan wrapping)
 *
 * Class ini membutuhkan:
 * - Grid array (untuk O(1) lookup mobil lain)
 * - maxCells (untuk wrapping/modulo)
 * - probSlow (probabilitas random braking)
 */
class NaSchMovement : public MovementStrategy
{
public:
    /**
  * Constructor
  *
  * @param maxCells Jumlah total cells di road (default 300)
  * @param maxV Kecepatan maksimal (default 5)
  * @param probSlow Probabilitas pengereman mendadak (default 0.2 = 20%)
  */
    NaSchMovement(int maxCells = 300, float maxV = 5.0f, float probSlow = 0.2f);


    /**
 * Destructor
 */
    ~NaSchMovement() = default;

    /**
 * Override method update() dari MovementStrategy
 *
 * Di sini kita panggil 4 aturan dengan URUTAN YANG BENAR:
 * 1. Accelerate
 * 2. Brake
 * 3. Randomize
 * 4. Move
 *
 * @param vehicle Reference ke Vehicle yang akan di-update
 */
    void update(Vehicle& vehicle) override;

    /**
 * Set grid array untuk lookup O(1)
 *
 * Grid adalah array 1D yang mapping posisi → indeks vehicle
 * grid[i] = j  → Vehicle indeks j ada di posisi i
 * grid[i] = -1 → Posisi i kosong
 *
 * @param gridPtr Pointer ke array grid
 * @param gridSize Ukuran grid array
 */
    void setGrid(const int* gridPtr, int gridSize);

    void setMaxCells(int cells) { maxCells = cells; }
    void setMaxV(float v) { this->maxV = v; }
    void setProbSlow(float prob) { probSlow = prob; }

    int getMaxCells() const { return maxCells; }
    float getMaxV() const { return maxV; }
    float getProbSlow() const { return probSlow; }

protected:
    /**
 * Override brake() dengan implementasi GRID
 *
 * Logic: Cek apakah ada vehicle lain di depan dalam jarak secepatatnya.
 * Gunakan grid untuk O(1) lookup.
 *
 * Contoh:
 * - Vehicle di posisi 100 dengan v = 5
 * - Cek: pos 101 (j=1), pos 102 (j=2), pos 103 (j=3)
 * - Kalau di pos 103 ada vehicle lain → v = 3 - 1 = 2
 */
    void brake(Vehicle& vehicle) override;

    /**
 * Override move() dengan implementasi WRAPPING
 *
 * Logic: Tambahkan kecepatan ke distance, lalu modulo maxCells.
 *
 * Contoh:
 * - distance = 298, v = 5 → distance = (298 + 5) % 300 = 3
 * - distance = 50, v = 3 → distance = (50 + 3) % 300 = 53
 */
    void move(Vehicle& vehicle) override;

    virtual void randomize(Vehicle& vehicle) override;
    virtual void accelerate(Vehicle& vehicle) override;

private:
    // Parameter Nagel-Schreckenberg
    int maxCells;      // Jumlah total cells (default 300)
    float maxV;          // Kecepatan maksimal (default 5)
    float probSlow;    // Probabilitas random braking (default 0.2)

    // Grid untuk O(1) lookup
    const int* grid;   // Pointer ke array grid (tidak own, hanya borrow)
    int gridSize;      // Ukuran grid array
};

