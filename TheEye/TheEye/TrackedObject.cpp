#include "stdafx.h"
#include "TrackedObject.h"
#include "LinearRegression.h"


TrackedObject::TrackedObject(int id, int currX, int currY, int firstFrame)
{
	this->currX = currX;
	this->currY = currY;
	this->id = id;
	this->inactiveCount = 0;
	int* pos = new int[2];
	pos[0] = currX;
	pos[1] = currY;
	positions.push_back(pos);

	speed = slope = intercept = avgX = avgY = 0.0;
	this->firstFrame = firstFrame;
	lastFrame = -1;
}


TrackedObject::~TrackedObject()
{	
	
	for (int posIter = 0; posIter < positions.size(); posIter++){
		delete positions[posIter];
	}
	positions.clear();
}

int* TrackedObject::getCurrentPos(void){
	int* retVal = new int[2];
	retVal[0] = currX;
	retVal[1] = currY;
	return retVal;
}

void TrackedObject::updatePosition(int currX, int currY){
	this->currX = currX;
	this->currY = currY;
	this->inactiveCount = 0;
	int* pos = new int[2];
	pos[0] = currX;
	pos[1] = currY;
	positions.push_back(pos);
	avgX = (avgX * (positions.size() - 1) + currX) / positions.size();
	avgY = (avgY * (positions.size() - 1) + currY) / positions.size();
}


void TrackedObject::evaluateParameters(void){
	//Storing mag of distance vector (squared) divided by the frames object was there.
	speed = ( pow(positions[0][0] - positions[positions.size() - 1][0], 2) + pow(positions[0][1] - positions[positions.size() - 1][1], 2) ) / (lastFrame - firstFrame);

	double* regCoeff = leastSqrRegression(positions);
	intercept = regCoeff[0];
	slope = regCoeff[1];
}

void TrackedObject::updateLastFrame(int frameCount){
	this->lastFrame = frameCount;
}

void TrackedObject::writeResults(){
	ofstream outFile(DATA_GATHER_FILE, std::ios::app);

	/*
		Format : ID		Speed		Intercept		Slope		AvgX		AvgY		Life(In Frames)
	*/
	outFile << id << "," << speed << "," << intercept << "," << slope << "," << avgX << "," << avgY << "," << (lastFrame - firstFrame) << endl;

	outFile.close();

}


string TrackedObject::getDataForUpdate(string inputVideoFile){
	evaluateParameters();
	stringstream outStream;
	outStream << inputVideoFile << "," << firstFrame << "," << lastFrame << ",";
	outStream << id << "," << speed << "," << intercept << "," << slope << "," << avgX << "," << avgY;
	for (vector<int*>::iterator iter = positions.begin(); iter != positions.end(); ++iter){
		outStream << "," << (*iter)[0] << "_" << (*iter)[1];
	}
	return outStream.str();
}