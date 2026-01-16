#include "PerlinNoiseRoad.h"

//--------------------------------------------------------------
PerlinNoiseRoad::PerlinNoiseRoad()
    : centerX(0)
    , centerY(0)
    , baseRadius(0)
    , numPoints(360)      // Satu titik per derajat untuk smoothness
    , noiseScale(1.5f)    // Kontrol frekuensi noise
    , deformAmount(0.3f)  // 30% deformasi (tengah dari range 20-40%)
{
}

//--------------------------------------------------------------
void PerlinNoiseRoad::generatePath(ofRectangle bounds) {
    // Bersihkan path yang sudah ada
    polyline.clear();
    path.clear();

    // Hitung titik pusat dari bounds (sama seperti CircleRoad)
    centerX = bounds.x + bounds.width / 2.0f;
    centerY = bounds.y + bounds.height / 2.0f;

    // Hitung radius dasar dari bounds (sama seperti CircleRoad)
    baseRadius = std::min(bounds.width / 2.0f, bounds.height / 2.0f);

    // Generate titik-titik di sekeliling lingkaran menggunakan Perlin noise untuk deformasi
    for (int i = 0; i < numPoints; i++) {
        // Hitung sudut (0 sampai TWO_PI)
        float angle = ofMap(i, 0, numPoints, 0.0f, TWO_PI);

        // Multi-octave Perlin noise untuk detail yang terlihat natural
        // Setiap octave menambah detail pada skala yang berbeda
        float n = 0.0f;
        n += ofNoise(angle * noiseScale, 0.0f) * 0.5f;        // Fitur besar (weight 50%)
        n += ofNoise(angle * noiseScale * 2.0f, 1.0f) * 0.3f; // Fitur sedang (weight 30%)
        n += ofNoise(angle * noiseScale * 4.0f, 2.0f) * 0.2f; // Detail kecil (weight 20%)

        // Map noise dari [0,1] ke [-1,1] untuk deformasi simetris
        // Ini memungkinkan radius untuk membesar DAN mengecil
        n = n * 2.0f - 1.0f;

        // Hitung radius yang sudah dideformasi (baseRadius +/- deformasi)
        float radius = baseRadius * (1.0f + n * deformAmount);

        // Hitung posisi menggunakan koordinat polar
        float x = centerX + cos(angle) * radius;
        float y = centerY + sin(angle) * radius;

        // Tambah titik ke polyline dan path
        polyline.addVertex(x, y);
        path.push_back(vec2(x, y));
    }

    // Tutup polyline untuk membentuk loop lengkap
    polyline.close();

    // Hitung total panjang dari perimeter
    totalLength = polyline.getPerimeter();
}

//--------------------------------------------------------------
vec2 PerlinNoiseRoad::getPointAtDistance(float dist) {
    // Normalisasi distance untuk handle wrapping
    dist = fmod(dist, totalLength);
    if (dist < 0) {
        dist += totalLength;
    }

    // Gunakan method bawaan OpenFrameworks untuk interpolasi smooth
    ofPoint pt = polyline.getPointAtLength(dist);
    return vec2(pt.x, pt.y);
}

//--------------------------------------------------------------
vec2 PerlinNoiseRoad::getTangentAtDistance(float dist) {
    // Ambil dua titik berdekatan untuk menghitung vektor arah
    float delta = 1.0f;
    vec2 p1 = getPointAtDistance(dist);
    vec2 p2 = getPointAtDistance(dist + delta);

    // Hitung arah dan normalize
    vec2 dir = p2 - p1;
    return normalize(dir);
}
