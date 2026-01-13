#include "NaSchMovement.h"
#include "../entities/Vehicle.h"
#include "ofMain.h"

/**
 * Constructor
 *
 * Initialize semua member variables dengan nilai default atau parameter
 *
 * @param maxCells Jumlah total cells (default 300)
 * @param maxV Kecepatan maksimal (default 5)
 * @param probSlow Probabilitas random braking (default 0.2)
 */
NaSchMovement::NaSchMovement(int maxCells, float maxV, float probSlow)
    : maxCells(maxCells)
    , maxV(maxV)
    , probSlow(probSlow)
    , grid(nullptr)    // Belum di-set, nanti di-set via setGrid()
    , gridSize(0)      // Belum di-set, nanti di-set via setGrid()
{
}

/**
 * Set Grid Array
 *
 * @param gridPtr Pointer ke array grid (dari ofApp)
 * @param size Ukuran grid array
 *
 * Penting: Kita hanya borrow pointer, tidak own memori
 */
void NaSchMovement::setGrid(const int* gridPtr, int size) {
    grid = gridPtr;
    gridSize = size;
}

/**
 * Update Vehicle dengan 4 Aturan Nagel-Schreckenberg
 *
 * URUTAN PENTING! Tidak boleh diubah:
 * 1. Accelerate (v++)
 * 2. Brake (cek mobil depan via grid)
 * 3. Randomize (20% kemungkinan v--)
 * 4. Move (distance += v dengan wrapping)
 *
 * @param vehicle Reference ke Vehicle yang akan di-update
 */
void NaSchMovement::update(Vehicle& vehicle) {
    accelerate(vehicle);
    brake(vehicle);
    randomize(vehicle);
    move(vehicle);
}


/**
 * Override Brake dengan Grid Logic
 *
 * Logic: Cek apakah ada vehicle lain di depan dalam jarak secepatatnya.
 * Gunakan grid untuk O(1) lookup (bukan O(n) loop).
 *
 * Contoh:
 * - Vehicle di posisi 100 dengan v = 5
 * - Cek: pos 101 (j=1), pos 102 (j=2), pos 103 (j=3)
 * - Kalau grid[103] != -1 → Ada vehicle! → v = 3 - 1 = 2
 *
 * @param vehicle Reference ke Vehicle
 */
void NaSchMovement::brake(Vehicle& vehicle) {
    // Pastikan grid sudah di-set!
    if (grid == nullptr) {
            MovementStrategy::brake(vehicle);
        return;
    }
    
    float currentV = vehicle.getVelocity();
    int currentDist = (int)vehicle.getDistance();

    for (int j = 1; j <= currentV; j++) {
        // Hitung posisi yang akan dicek dengan WRAPPING
        int checkPos = (currentDist + j) % maxCells;

        // Pastikan posisi dalam range grid
        if (checkPos >= 0 && checkPos < gridSize) {
            // Cek apakah ada vehicle di posisi ini
            if (grid[checkPos] != -1) {
                // ADA VEHICLE DI DEPAN!
                // Kurangi kecepatan: j - 1 (jarak aman)
                vehicle.setVelocity(j - 1);

                // Stop checking, kita sudah cari kendaraan terdekat
                break;
            }
        }
    }
    // Kalau loop selesai tanpa break → tidak ada mobil di depan
    // → kecepatan tidak berubah
}

/**
 * Override Move dengan Wrapping Logic
 *
 * Logic: Tambahkan kecepatan ke distance, lalu modulo maxCells
 * supaya vehicle "teleport" ke awal kalau sudah sampai ujung.
 *
 * Contoh:
 * - distance = 298, v = 5 → distance = (298 + 5) % 300 = 3
 * - distance = 50, v = 3  → distance = (50 + 3) % 300 = 53
 *
 * @param vehicle Reference ke Vehicle
 */
void NaSchMovement::move(Vehicle& vehicle) {
    // Ambil distance dan velocity saat ini
    float currentDist = vehicle.getDistance();
    float currentV = vehicle.getVelocity();

    // Tambahkan velocity ke distance
    float newDist = currentDist + currentV;

    while (newDist >= maxCells) {
        newDist -= maxCells;
    }
    while (newDist < 0) {
        newDist += maxCells;
    }

    // Update distance
    vehicle.setDistance(newDist);
}

/**
 * Override Randomize dengan probSlow dari Constructor
 *
 * Logic: Dengan probabilitas probSlow, kurangi kecepatan 1.
 * probSlow di-set dari constructor (bukan hardcoded 0.2).
 */
void NaSchMovement::randomize(Vehicle& vehicle) {
    if (vehicle.getVelocity() > 0) {
        if (ofRandom(1.0f) < probSlow) {
            vehicle.setVelocity(vehicle.getVelocity() - .5f);
        }
    }
}

void NaSchMovement::accelerate(Vehicle& vehicle) {
    if (vehicle.getVelocity() < maxV) {
        vehicle.setVelocity(vehicle.getVelocity() + 0.4f);
    }
}