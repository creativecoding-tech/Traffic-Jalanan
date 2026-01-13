#pragma once

#include "ofMain.h"
#include "entities/Vehicle.h"
#include "entities/SedanCar.h"
#include <vector>
#include <memory>
#include "road/Road.h"

using glm::vec2;
using glm::vec3;


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

private:
	// Road parameters
	std::unique_ptr<Road> road;

	// Nagel-Schreckenberg parameters
	const int maxCells = 300;
	const float maxV = 20.0f;
	float probSlow = .7f;

	// Data structures
	std::vector<std::unique_ptr<Vehicle>> traffic;
	std::vector<int> grid;
		
};
