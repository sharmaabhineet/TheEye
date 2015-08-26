#ifndef _TEST_UTIL_H
#define _TEST_UTIL_H

#include "Util.h"
#include "LearningUtil.h"

void testParseUpdateModelFile(){
	vector<ObjectActivity*>* vecObj = parseUpdateModelFile();
	for (int i = 0; i < vecObj->size(); i++){
		ObjectActivity* objAc = vecObj->at(i);
		cout << objAc->inputVideoFile << " - " << objAc->firtFrame << " - " << objAc->lastFrame << endl;
		for (int j = 0; j < objAc->positions->size(); j++){
			cout << "\t" << objAc->positions->at(j)[0] << " - " << objAc->positions->at(j)[1] << endl;
		}

		cout << "----------------------------------------" << endl << endl;
	}
	system("pause");
	exit(1);
}

void testTokenize(){
	string testStr = "This,is,comma,separated,line";
	vector<string> vec = tokenize(testStr, ",");
	for (vector<string>::iterator iter = vec.begin(); iter != vec.end(); ++iter){
		cout << *iter << endl;
	}
	system("pause");
	exit(1);
}

void testReadTrainingData(){
	CvMLData& mlData = readTrainingData("D:/TheEyeTrainingData/data.csv");

	const CvMat* resp = mlData.get_responses();
	
	/*for (int i = 0; i < resp->rows; ++i){
		cout << CV_MAT_ELEM(*resp, float, i, 0) << endl;
	}*/
	cout << "COLUMNS in RESPONSE : " << resp->cols << endl;
	map<string, int> mapClassLables = mlData.get_class_labels_map();

	for (map<string, int>::iterator iter = mapClassLables.begin(); iter != mapClassLables.end(); ++iter){
		string key = iter->first;
		cout << key << " --> " << mapClassLables[iter->first] << endl;
	}

	CvMLData& testData = readTrainingData("D:/TheEyeTrainingData/testData.csv");

	trainModel(mlData, testData);
}

void testAppendToFile(){
	appendToFile("D:/TheEyeTrainingData/testAppend.txt", "Hello, This is Test for non existing file");
	appendToFile("D:/TheEyeTrainingData/testAppend.txt", "Hello, This is Test for append");
}

void testListFiles(){
	cout << "**** TEST List Files ****" << endl;
	string dirName = "D:/TheEyeTrainingData";
	vector<string> vecFileNames = listFiles(dirName);
	for (vector<string>::iterator iter = vecFileNames.begin(); iter != vecFileNames.end(); ++iter){
		cout << *iter << endl;
	}

	cout << " ------------------- " << endl;

	dirName = "D:/TheEyeTrainingData/Videos";
	string extension = ".mov";
	vecFileNames = listFiles(dirName, extension);
	for (vector<string>::iterator iter = vecFileNames.begin(); iter != vecFileNames.end(); ++iter){
		cout << *iter << endl;
	}
}

#endif