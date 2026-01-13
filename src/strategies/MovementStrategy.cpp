#include "MovementStrategy.h"
#include "../entities/Vehicle.h"
#include "ofMain.h"

/**
 * Rule 1: Akselerasi
 *
 * Logic: Kendaraan selalu mencoba menambah kecepatan sampai maksimal.
 * Ini mensimulasikan pengendara yang ingin ngebut.
 *
 * Contoh:
 * - v = 0, maxV = 5  → v menjadi 1
 * - v = 3, maxV = 5  → v menjadi 4
 * - v = 5, maxV = 5  → v tetap 5 (sudah maksimal)
 */
void MovementStrategy::accelerate(Vehicle& vehicle) {
	const int maxV = 5; //hardcode dulu

	// Tambah kecepatan kalau belum maksimal
	if (vehicle.getVelocity() < maxV) {
		vehicle.setVelocity(vehicle.getVelocity() + 1);
	}
}

/**
 * Rule 2: Pengereman (Braking)
 *
 * Logic: Kendaraan mengecek apakah ada kendaraan lain di depan dalam jarak
 * secepatatnya. Jika ada, kurangi kecepatan supaya tidak nabrak.
 *
 * CATATAN PENTING:
 * Method ini adalah PLACEHOLDER (kosong).
 *
 * Logic yang sebenarnya butuh:
 * - Grid array (untuk O(1) lookup posisi kendaraan lain)
 * - Posisi semua kendaraan
 * - maxCells (untuk modulo/wrapping)
 *
 * Implementasi lengkap akan ada di (child class),
 * karena di sana kita bisa pass grid dan maxCells sebagai parameter.
 */
void MovementStrategy::brake(Vehicle& vehicle) {
	// Placeholder - logic akan di-override child class
	// Di sana kita akan implementasi pengecekan kendaraan di depan via grid
}

/**
 * Rule 3: Randomization (Pengereman Mendadak)
 *
 * Logic: Dengan probabilitas tertentu (default 20%), kendaraan akan
 * mengurangi kecepatan 1. Ini mensimulasikan perilaku manusia yang
 * tidak prediktif:
 * - Mengantuk
 * - HP bunyi
 * - Lihat HP
 * - Distraction lainnya
 *
 * Contoh:
 * - v = 3, probSlow = 0.2, random = 0.15 → v menjadi 2 (rem!)
 * - v = 3, probSlow = 0.2, random = 0.25 → v tetap 3 (tidak rem)
 */
void MovementStrategy::randomize(Vehicle& vehicle) {
	const float probSlow = 0.2f; //kemungkinan 20%
	//hanya randomize kalau kecepatan > 0 
	if (vehicle.getVelocity() > 0) {
		//generate angka random 0.0 sd 1.0
		if (ofRandom(1.0f) < probSlow) vehicle.setVelocity(vehicle.getVelocity() - 1);
	}
}


/**
 * Rule 4: Movement (Gerak)
 *
 * Logic: Kendaraan bergerak sesuai kecepatan saat ini.
 *
 * CATATAN PENTING:
 * Method ini belum implementasi wrapping (modulo maxCells).
 *
 * Contoh yang seharusnya:
 * - distance = 50, v = 3     → distance menjadi 53
 * - distance = 299, v = 2    → distance harusnya jadi 1 (wrapping!)
 * - distance = 298, v = 5    → distance harusnya jadi 3 (wrapping!)
 *
 * Implementasi lengkap (dengan wrapping) akan ada di child-class,
 * karena di sana kita bisa pass maxCells sebagai parameter.
 */
void MovementStrategy::move(Vehicle& vehicle) {
	// Basic movement: tambahkan kecepatan ke distance
	vehicle.setDistance(vehicle.getDistance() + vehicle.getVelocity());

	// Wrapping (modulo) akan di-override di NaSchMovement
	// karena butuh parameter maxCells
}
