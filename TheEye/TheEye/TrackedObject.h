#pragma once
#include "stdafx.h"
#include <opencv2\ml\ml.hpp>

class TrackedObject
{
public:
	TrackedObject(int id, int currX, int currY, int firstFrame);
	~TrackedObject();

	int* getCurrentPos(void);
	int getID(){ return id; }
	void updatePosition(int currX, int currY);
	void reportInactive(void){		inactiveCount++;		}
	int getInactiveCount(void){		return inactiveCount;	}
	void updateLastFrame(int frameCount);
	void evaluateParameters(void);

	float getSpeed(){ return speed; }
	float getSlope(){ return slope; }
	float getIntercept(){ return intercept; }
	float getAvgX(){ return avgX; }
	float getAvgY(){ return avgY; }
	int getLife(){ return (lastFrame - firstFrame); }

	string TrackedObject::getDataForUpdate(string inputVideoFile);
	void writeResults();
	

private:
	
	int currX, currY, id, inactiveCount;
	vector<int*> positions;

	float speed, intercept, slope, avgX, avgY;
	int firstFrame, lastFrame;

	
	
};

