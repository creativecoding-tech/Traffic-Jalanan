#pragma once
// Forward declaration untuk Vehicle
class Vehicle;
/**
 * MovementStrategy - Abstract Base Class
 *
 * Interface untuk mengontrol bagaimana mobil bergerak.
 * Menggunakan reference (Vehicle&) bukan pointer (Vehicle*) untuk lebih safety.
 *
 * Design Pattern: Strategy Pattern
 * - Memisahkan "bagaimana mobil bergerak" dari "mobil itu sendiri"
 * - Memungkinkan switching movement algorithm di runtime
 * - Easy untuk testing & extensibility
 */
class MovementStrategy
{
public:
	// Destructor virtual (wajib untuk polymorphism)
	virtual ~MovementStrategy() = default;
		/**
	 * Method ini WAJIB di-override oleh concrete strategy.
	 *
	 * @param vehicle Reference ke Vehicle yang akan di-update
	 *
	 * Note: Pakai reference (Vehicle&) bukan pointer (Vehicle*) karena:
	 * - Tidak bisa nullptr (lebih safe)
	 * - Tidak ada ownership transfer (hanya borrow)
	 * - Lebih modern C++ style
	 *
	 * Logic: Implementasi akan memanggil 4 aturan Nagel-Schreckenberg:
	 * - Rule 1: Accelerate (v < maxV ? v++ : skip)
	 * - Rule 2: Brake (cek mobil depan via grid)
	 * - Rule 3: Randomize (20% kemungkinan v--)
	 * - Rule 4: Move (distance += v)
	 */
	virtual void update(Vehicle& vehicle) = 0;

protected:
	// Helper methods untuk 4 aturan Nagel-Schreckenberg
	// Bisa dipakai atau di-override oleh child class

	virtual void accelerate(Vehicle& vehicle);
	virtual void brake(Vehicle& vehicle);
	virtual void randomize(Vehicle& vehicle);
	virtual void move(Vehicle& vehicle);
};

