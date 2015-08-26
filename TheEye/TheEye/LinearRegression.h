#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H
#include "stdafx.h"

struct Point
{
	double x;
	double y;
};

double* leastSqrRegression(struct Point* xyCollection, int dataSize)
{
	

	double SUMx = 0;     //sum of x values
	double SUMy = 0;     //sum of y values
	double SUMxy = 0;    //sum of x * y
	double SUMxx = 0;    //sum of x^2
	double SUMres = 0;   //sum of squared residue
	double res = 0;      //residue squared
	double slope = 0;    //slope of regression line
	double y_intercept = 0; //y intercept of regression line
	double SUM_Yres = 0; //sum of squared of the discrepancies
	double AVGy = 0;     //mean of y
	double AVGx = 0;     //mean of x
	double Yres = 0;     //squared of the discrepancies
	double Rsqr = 0;     //coefficient of determination

	//calculate various sums 
	for (int i = 0; i < dataSize; i++)
	{
		//sum of x
		SUMx = SUMx + (xyCollection + i)->x;
		//sum of y
		SUMy = SUMy + (xyCollection + i)->y;
		//sum of squared x*y
		SUMxy = SUMxy + (xyCollection + i)->x * (xyCollection + i)->y;
		//sum of squared x
		SUMxx = SUMxx + (xyCollection + i)->x * (xyCollection + i)->x;
	}

	//calculate the means of x and y
	AVGy = SUMy / dataSize;
	AVGx = SUMx / dataSize;

	//slope or a1
	slope = (dataSize * SUMxy - SUMx * SUMy) / (dataSize * SUMxx - SUMx*SUMx);

	//y itercept or a0
	y_intercept = AVGy - slope * AVGx;

	double* retData = new double[2];
	retData[0] = y_intercept;
	retData[1] = slope;
	return retData;
}

double* leastSqrRegression(vector<int*> vecData)
{
	double SUMx = 0;     //sum of x values
	double SUMy = 0;     //sum of y values
	double SUMxy = 0;    //sum of x * y
	double SUMxx = 0;    //sum of x^2
	double SUMres = 0;   //sum of squared residue
	double res = 0;      //residue squared
	double slope = 0;    //slope of regression line
	double y_intercept = 0; //y intercept of regression line
	double SUM_Yres = 0; //sum of squared of the discrepancies
	double AVGy = 0;     //mean of y
	double AVGx = 0;     //mean of x
	double Yres = 0;     //squared of the discrepancies
	double Rsqr = 0;     //coefficient of determination

	//calculate various sums 
	for (int i = 0; i < vecData.size(); i++)
	{
		//sum of x
		SUMx = SUMx + vecData[i][0];
		//sum of y
		SUMy = SUMy + vecData[i][1];
		//sum of squared x*y
		SUMxy = SUMxy + vecData[i][0] * vecData[i][1];
		//sum of squared x
		SUMxx = SUMxx + vecData[i][0] * vecData[i][1];
	}
	int dataSize = vecData.size();
	//calculate the means of x and y
	AVGy = SUMy / dataSize;
	AVGx = SUMx / dataSize;

	//slope or a1
	slope = (dataSize * SUMxy - SUMx * SUMy) / (dataSize * SUMxx - SUMx*SUMx);

	//y itercept or a0
	y_intercept = AVGy - slope * AVGx;

	double* retData = new double[2];
	retData[0] = y_intercept;
	retData[1] = slope;
	//cout << "[REGRESSION] SIZE : " << vecData.size() << " INTERCEPT : " << y_intercept << " SLOPE : " << slope << endl;
	return retData;
}
#endif