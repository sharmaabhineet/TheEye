// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include <direct.h>


using namespace std;


//OpenCV Header Files
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\ml\ml.hpp>
#include <opencv2\video\background_segm.hpp>





#define SVM_MODEL "SVM"
#define KNN_MODEL "KNN"
#define NORMAL_BAYES_MODEL "NORMAL_BAYES_MODEL"

//Modes 
#define DATA_GATHER_MODE 0
#define TRAINING_MODE 1
#define TEST_MODE 2
#define MODEL_UPDATE_MODE 3

//Constants for Various Learning Algorithms
#define NORMAL_BAYES 0
#define KNN 1
#define SVM 2



#define DATA_GATHER_FILE "gatheredData.csv"
#define TRAINING_DATA_FILE "data.csv"
#define TEST_DATA_FILE "testData.csv"
#define SVM_MODEL_FILE_PATH "SvmDataModel.txt"
#define BAYES_MODEL_FILE_PATH "BayesDataModel.txt"
#define KNN_MODEL_FILE_PATH "data.csv"
#define DATA_MODEL_INFO_FILE "datamodelInfo.txt"
#define UPDATE_DATA_MODEL_FILE "updateModel.csv"
