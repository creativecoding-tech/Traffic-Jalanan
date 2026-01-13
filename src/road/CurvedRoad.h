#pragma once
#include "Road.h"
class CurvedRoad : public Road
{
public:
	CurvedRoad() = default;
	~CurvedRoad() = default;

	/**
	* Generate Path dengan ofPolyline.curveTo()
	*/
	void generatePath() override;

    /**
     * Get Point at Distance
     * Menggunakan ofPolyline.getPointAtLength()
     */
    vec2 getPointAtDistance(float dist) override;

    /**
     * Get Tangent at Distance
     * Menghitung arah dengan sampling 2 titik
     */
    vec2 getTangentAtDistance(float dist) override;
};

