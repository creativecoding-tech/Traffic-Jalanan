#pragma once
#include "../strategies/MovementStrategy.h"
#include <glm/glm.hpp>
#include <memory>

// Namespace alias untuk GLM (biar singkat)
using glm::vec2;
using glm::vec3;


/**
 * Vehicle - Abstract Base Class untuk SEMUA jenis kendaraan
 *
 * Ini adalah base class untuk semua jenis kendaraan:
 * - SedanCar (mobil biasa)
 * - Motorcycle (motor)
 * - Truck (truk)
 * - Bus (bus)
 * - Bicycle (sepeda)
 * - dll
 *
 * Class ini tidak bisa langsung di-instantiate karena ada method virtual murni.
 *
 * Design Pattern: Strategy Pattern + Template Method
 * - Vehicle "HAS-A" MovementStrategy (composition)
 * - MovementStrategy mengontrol bagaimana Vehicle bergerak
 * - Vehicle men-delegate movement logic ke strategy
 */
class Vehicle
{
public:
	Vehicle(float startDist, float velocity, vec3 col) : distance(startDist)
		, v(velocity)
		, color(col)
		, movementStrat(nullptr)
	{
	}
	// Destructor virtual (WAJIB untuk abstract base class)
	virtual ~Vehicle() = default;

	/**
 * Method ini WAJIB di-override oleh child class (SedanCar, TruckCar, dll)
 *
 * Di sini kita implementasi Template Method Pattern:
 * - update() punya alur yang TETAP
 * - Tapi detail movement bisa BERBEDA tergantung strategy
 */
	virtual void update() = 0;
/**
	* Method ini juga WAJIB di-override
	* Setiap jenis mobil punya cara visualisasi berbeda
	*/
	virtual void draw(float xPos, float yPos, float angle = 0) = 0;

	virtual float getDistance() const {
		return distance;
	}

	virtual float getVelocity() const {
		return v;
	}

	virtual vec3 getColor() const {
		return color;
	}

	virtual void setDistance(float d) {
		distance = d;
	}

	virtual void setVelocity(float vel) {
		v = vel;
	}

	virtual void setColor(vec3 col) {
		color = col;
	}

	/**
 * Set movement strategy untuk mobil ini
 *
 * Contoh penggunaan:
 * car.setMovementStrategy(std::make_unique<NaSchMovement>());
 */
	virtual void setMovementStrategy(std::unique_ptr<MovementStrategy> strat) {
		movementStrat = std::move(strat);
	}

	virtual void setGrid(const int* gridPtr, int gridSize){}

	// Set max velocity di movement strategy
	virtual void setMaxVelocity(float maxV) {
		if (movementStrat) {
			movementStrat->setMaxV(maxV);
		}
	}

protected:
 /**
 * distance - Posisi mobil di jalan (dalam satuan pixels/units)
 *
 * Untuk curved road:
 * - distance = 0   → di awal path
 * - distance = 100 → 100 pixels dari awal path
 * - distance >= totalLength → wrapping ke awal
 */
	float distance;

    /**
 * v (velocity) - Kecepatan mobil saat ini
 *
 * Range: 0 - 5
 * - 0 = berhenti (macet)
 * - 1 = lambat
 * - 5 = maksimal (ngebut)
 */
	float v;

    /**
 * color - Warna mobil (RGB 0.0 - 1.0)
 *
 * Menggunakan glm::vec3 (bukan ofColor) karena:
 * - Lebih modern (GLSL-compatible)
 * - Range 0-1 (standard di graphics programming)
 * - Bisa di-convert ke ofColor kalau perlu
 */
	vec3 color;

    /**
 * movementStrat - Strategy yang mengontrol movement mobil
 *
 * Menggunakan unique_ptr karena:
 * - Automatic memory management (tidak perlu delete)
 * - Exclusive ownership (satu Vehicle punya satu Strategy)
 * - Exception safe
 *
 * Bisa diganti di runtime:
 * vehicle.setMovementStrategy(std::make_unique<AutonomousMovement>());
 */
	std::unique_ptr<MovementStrategy> movementStrat;
};

