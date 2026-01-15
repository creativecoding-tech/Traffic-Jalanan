## Traffic-Jalanan Nagel-Schreckenberg | OpenFrameworks

[![C++](https://img.shields.io/badge/C++-17-blue)](https://isocpp.org/)
[![OpenFrameworks](https://img.shields.io/badge/OpenFrameworks-0.12.1-blue)](https://openframeworks.cc)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)](https://microsoft.com/windows)
![License](https://img.shields.io/badge/License-Apache%202.0-green)
![Branch](https://img.shields.io/badge/Branch-master-success)

[![Fund The Experiments](https://img.shields.io/badge/Fund-The_Experiments-FF5722?style=for-the-badge&logo=buy-me-a-coffee)](https://sociabuzz.com/abdkdhni)

Eksperimen creative coding dengan openFrameworks. Menggabungkan Nagel-Schreckenberg movement algorithm, bezier curves visualization, dan multi-track system untuk menciptakan pola visual yang dinamis dan estetik dalam C++17.

---

## 📺 Demo Video

Lihat hasilnya di YouTube: [Watch Demo](https://youtu.be/nE35ZAXm14c)

---

## 📺 Preview

Project ini adalah eksplorasi **generative art** yang menggunakan Nagel-Schreckenberg movement algorithm untuk menggerakkan objek-objek visual secara organik. Terdapat __3 lintasan konsentris__ (Outer, Middle, Inner) yang dapat dikonfigurasi:

### Road Types

- __CIRCLE ROAD__ - Lingkaran sempurna dengan radius berbeda per track
- __CURVED ROAD__ - Oval dengan straight sections, menggunakan ofPolyline untuk smooth curves

### Vehicle System

- __SedanCar__ - Kendaraan standar dengan physics-based movement
- __Nagel-Schreckenberg Model__ - 4 aturan dasar simulasi traffic:
  1. __Accelerate__ - Kecepatan bertambah sampai maxV
  2. __Brake__ - Cek mobil depan via grid untuk collision avoidance
  3. __Randomize__ - Probabilitas random braking (human error)
  4. __Move__ - Update posisi dengan wrapping

### Visualization Features

- __Radial Bezier Curves__ - Garis dari center layar ke setiap mobil dengan S-curve control points
- __Dynamic Line Width__ - Ketebalan garis berdasarkan kecepatan kendaraan
- __Trail Effect__ - Semi-transparent overlay untuk visual jejak yang menarik
- __Multiple Lines Per Car__ - Konfigurasi jumlah garis radial per mobil
- __Custom Curve Intensity__ - Pengaturan kelengkungan garis per track

---

## ✨ Fitur & Teknik

### Architecture Patterns

- __Strategy Pattern__ - MovementStrategy untuk flexible movement algorithms
  - NaSchMovement - Implementasi Nagel-Schreckenberg model
  - Extensible untuk AutonomousMovement, HumanMovement, dll
- __Template Method Pattern__ - Vehicle base class dengan virtual methods
- __Polymorphic Road System__ - Road base class untuk CircleRoad, CurvedRoad

### Technical Features

- __Grid-Based Collision Detection__ - O(1) lookup untuk kendaraan di depan
- __Multi-Track Configuration__ - 3 tracks dengan parameter independen:
  - Jumlah kendaraan per track
  - Kecepatan maksimal berbeda per track
  - Jumlah cells berbeda per track
  - Arah putaran berbeda (clockwise/counterclockwise)
- __Bezier Curve Visualization__ - Cubic bezier dengan 100 tessellation segments
- __Physics-Based Body Simulation__ - Multi-segment vehicle body dengan follow logic
- __Real-time Parameter Tuning__ - Keyboard shortcuts untuk ubah curve intensity

---

## 🎮 Controls

| Input | Action |
| --- | --- |
| __Key 'S'__ | Mulai simulasi (Start) |
| __Key '1'__ | Switch ke CircleRoad (lingkaran sempurna) |
| __Key '2'__ | Switch ke CurvedRoad (oval dengan straight sections) |
| __Key '+' / '='__ | Tingkatkan curve intensity track OUTER |
| __Key '-' / '_'__ | Kurangi curve intensity track OUTER |
| __Key ']'__ | Tingkatkan curve intensity track MIDDLE |
| __Key '['__ | Kurangi curve intensity track MIDDLE |
| __Key '.' / '>'__ | Tingkatkan curve intensity track INNER |
| __Key ',' / '<'__ | Kurangi curve intensity track INNER |
| __Key 'Q'__ | Keluar dari aplikasi |

---

## 🛠️ Tech Stack

- __OpenFrameworks 0.12.1__
  - ofPolyline untuk smooth curves
  - ofEasyCam untuk camera system
  - OpenGL-based rendering
- __C++17__
- __Visual Studio 2022 Community__ (v143 toolset)
- __GLM (OpenGL Mathematics)__ - vec2, vec3 untuk vector operations
- __Strategy Pattern__ untuk flexible movement algorithms
- __Nagel-Schreckenberg Traffic Model__ - Cellular automata untuk traffic flow

---

## 📦 Installation

### Prerequisites

1. Install __OpenFrameworks__ untuk Windows
2. Install __Visual Studio 2022 Community__ dengan workload "Desktop development with C++"

### Setup

```bash
# Clone repository ini
git clone https://github.com/username/Traffic-Jalanan.git

# Buka Visual Studio
# Double-click: Traffic-Jalanan.sln

# Build & Run
# Press F5 atau klik "Local Windows Debugger"
```

---

## 📁 Project Structure

```
Traffic-Jalanan/
├── src/
│   ├── main.cpp              # Entry point aplikasi
│   ├── ofApp.cpp/h           # Main application class
│   ├── entities/             # Vehicle implementations
│   │   ├── Vehicle.h         # Abstract base class untuk semua kendaraan
│   │   └── SedanCar.cpp/h    # Concrete implementation (mobil sedan)
│   ├── road/                 # Road system implementations
│   │   ├── Road.h            # Abstract base class untuk semua jenis road
│   │   ├── CircleRoad.cpp/h  # Lingkaran sempurna
│   │   └── CurvedRoad.cpp/h  # Oval dengan straight sections
│   └── strategies/           # Movement strategies (Strategy Pattern)
│       ├── MovementStrategy.h         # Interface untuk movement algorithms
│       ├── MovementStrategy.cpp       # Base implementation
│       ├── NaSchMovement.h            # Nagel-Schreckenberg model
│       └── NaSchMovement.cpp          # Implementasi NaSch rules
├── bin/                      # Compiled executable
├── dll/                      # OF dependencies
└── Traffic-Jalanan.sln       # Visual Studio project file
```

---

## 🧬 Mathematics Behind

### Nagel-Schreckenberg Model

Model traffic cellular automata dengan 4 aturan:

```
// Rule 1: Accelerate
if (v < maxV) {
    v = v + 1;
}

// Rule 2: Brake (cek mobil depan via grid)
gap = distanceToNextVehicle - 1;
if (v > gap) {
    v = gap;
}

// Rule 3: Randomize (human error)
if (random() < probSlow) {
    v = max(0, v - 1);
}

// Rule 4: Move
distance = (distance + v) % maxCells;
```

### Bezier Curve Visualization

```
// Cubic bezier formula untuk radial lines
B(t) = (1-t)³P0 + 3(1-t)²tP1 + 3(1-t)t²P2 + t³P3

// P0 = Center layar
// P3 = Tepi mobil (dengan gap 5px)
// P1 = Control point 1 (dengan curveAngle1 offset)
// P2 = Control point 2 (dengan curveAngle2 offset)
```

### Physics-Based Body Simulation

```
// Follow logic untuk multi-segment body
targetSpacing = 5.0f + sin(time - segmentIndex * 0.5f) * 2.0f;
spacingError = actualSpacing - targetSpacing;
moveAmount = spacingError * 0.2f;  // Smooth follow
```

---

## 🎯 Purpose

Project ini adalah bagian dari eksplorasi __Creative Coding__ dan pembelajaran:

- 🔬 Eksperimen generative art dengan cellular automata movement
- 🎨 Belajar design patterns (Strategy, Template Method) dalam C++
- ⚡ Implementasi smooth bezier curves visualization dengan efek visual
- 📚 Memahami arsitektur modular untuk creative coding systems
- 🌿 Fondasi untuk project visual art yang lebih kompleks dan interaktif

---

## 🚀 Performance

Dengan optimasi C++ modern dan openFrameworks:

- __Solid 60 FPS__ pada resolusi 1920x1080
- __Smooth bezier curves__ dengan 100 tessellation segments
- __Anti-aliased rendering__ untuk kualitas visual tinggi
- __Grid-based O(1) collision detection__ untuk efficient movement system

---

## 🤝 Contributing

Ide, saran, dan improvement sangat welcome! Feel free to share your experiments!

---

## 📄 License

This project is licensed under the __Apache License 2.0__ - see the LICENSE file for details.

---

## 🙏 Credits & References

- __OpenFrameworks__ - Creative coding framework
- __Nagel-Schreckenberg Model__ - Traffic flow cellular automata
- __Creative Coding Community__ - Inspirasi dan resources

---

## 🔗 Links

- __OpenFrameworks__ - openframeworks.cc
- __Watch Demo__ - [YouTube demonstration](https://youtu.be/nE35ZAXm14c)
- __Support Me__ - [Fund the experiments ☕](https://sociabuzz.com/abdkdhni)

---

__Made with ❤️ for learning and creative exploration__
