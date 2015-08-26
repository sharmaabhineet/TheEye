#ifndef _UTIL_H
#define _UTIL_H

#include "stdafx.h"
#include "dirent.h"

vector<string> listFiles(string dirPath);
vector<string> listFiles(string dirPath, string extension);

void appendToFile(string filePath, string text);

CvMLData& readTrainingData(string filePath);

bool contains(string mainStr, string partStr);

vector<string> tokenize(string inputStr, string token);


CvMLData& readTrainingData(string filePath){
	CvMLData& objMLData = *(new CvMLData());
	if (objMLData.read_csv(filePath.c_str()) == -1){
		cerr << "Error Opening File : " << filePath << endl;
		cerr << "Program will now quit..." << endl;
		system("pause");
		exit(1);
	}
	else{
		//do nothing. Go Ahead
	}
	objMLData.set_response_idx(5);
	/*
	 * Dividing the Training Data into 80% Training Data & 20% Test Data
	 * ( 80-20 Split )
	 */
	//CvTrainTestSplit* trainTestSplit = new CvTrainTestSplit(700, true);
	//objMLData.set_train_test_split(trainTestSplit);
	
	return objMLData;
}

void appendToFile(string filePath, string text){
	/*
	 * Creates File if it does not exist, Otherwise would append to the end of it.
	*/
	ofstream outFile(filePath, std::ios::app);
	outFile << text << endl;
}

vector<string> listFiles(string dirPath){
	return listFiles(dirPath, string());
}

vector<string> listFiles(string dirPath, string extension){
	vector<string> vecFileNames;
	DIR* dir = opendir(dirPath.c_str());
	struct dirent* ent;
	if (dir != NULL){
		while ((ent = readdir(dir)) != NULL){
			string fileName = string(ent->d_name);
			if (extension.empty() || contains(fileName, extension)){
				vecFileNames.push_back(fileName);
			}
			else{
				//do nothing. Continue
			}
		}
	}
	else{
		//do nothing
	}
	return vecFileNames;
}

bool contains(string mainStr, string partStr){
	return mainStr.find(partStr) != string::npos;
}

vector<string> readFileLineByLine(string inputFile){
	ifstream inFile(inputFile);
	string line;
	vector<string> vecLines;
	while (getline(inFile, line)){
		vecLines.push_back(line);
	}
	return vecLines;
}


struct ObjectActivity{
	string inputVideoFile;
	int firtFrame, lastFrame, id;
	float speed, slope, intercept, avgX, avgY, life;
	char classLabel;
	vector<int*>* positions;
};


vector<ObjectActivity*>* parseUpdateModelFile(){
	vector<ObjectActivity*>* retVec = new vector<ObjectActivity*>();
	vector<string> vecLines = readFileLineByLine(UPDATE_DATA_MODEL_FILE);
	for (vector<string>::iterator iter = vecLines.begin(); iter != vecLines.end(); ++iter){
		vector<string> vecTokens = tokenize(*iter, ",");
		ObjectActivity* obj = new ObjectActivity();
		retVec->push_back(obj);
		obj->inputVideoFile = vecTokens[0];
		obj->firtFrame = atoi(vecTokens[1].c_str());
		obj->lastFrame = atoi(vecTokens[2].c_str());
		obj->id = atoi(vecTokens[3].c_str());
		obj->speed = atof(vecTokens[4].c_str());
		obj->intercept = atof(vecTokens[5].c_str());
		obj->slope = atof(vecTokens[6].c_str());
		obj->avgX = atof(vecTokens[7].c_str());
		obj->avgY = atof(vecTokens[8].c_str());
		vector<int*>* positions = new vector<int*>();
		obj->positions = positions;
		for (int i = 9; i < vecTokens.size(); i++){
			int* arr = new int[2];
			vector<string> pos = tokenize(vecTokens[i], "_");
			if (pos.size() != 2){
				break;
			}
			arr[0] = atoi(pos[0].c_str());
			arr[1] = atoi(pos[1].c_str());
			positions->push_back(arr);
		}
	}
	return retVec;
}


vector<string> tokenize(string inputStr, string token){
	vector<string> retVec;
	int lastIndex = inputStr.find(token);
	while (lastIndex != std::string::npos){
		retVec.push_back(inputStr.substr(0, lastIndex));
		lastIndex += token.length();
		inputStr = inputStr.substr(lastIndex);
		lastIndex = inputStr.find(token);
	}
	retVec.push_back(inputStr);
	return retVec;
}

#endif