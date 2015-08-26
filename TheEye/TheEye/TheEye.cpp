// TheEye.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "globals.h"
#include "TrackedObject.h"
#include "Util.h"
#include "TestUtil.h"


void dataGatherRun(string trainingDataDir);
void trainingModeRun();
void testModelRun(string testFilePath);
void modelUpdateRun();

bool arePointsClose(int* p1, int* p2);
void processVideo(string inputFilePath, string outputFilePath);
void pause(void);
void processContours(cv::Mat& frame, std::vector<std::vector<cv::Point> > contours, vector<TrackedObject*>* vecTrackedObj, int frameCount);
void predict(vector<TrackedObject*>* vecTrackedObjects, cv::Mat&  results);
char playVideo(string inputFilePath, int startFrame, int endFrame, vector<int*>* positions);
void printUsage(void);

//Global Parameters
int g_ID = 0;
int g_FrameHt, g_FrameWidth;

//Global variable. Will have value only in case the program runs in Test Mode
DataModelInfo* g_modelInfo; // (*(new CvStatModel()), 0.0); // = *(new DataModelInfo(*(new CvStatModel()), 0.0));

float g_normalClassVal;
float g_abNormalClassVal;


string g_testFilePath;



int MODE = -1;
 
int main(int argc, char* argv[])
{
	if (argc < 2 || argc > 3){
		printUsage();
		system("pause");
		return -1;
	}
	else{
		//do nothign. Go Ahead
	}


	try{
		MODE = atoi(argv[1]);
	}
	catch (...){
		printUsage();
		cout << "ERROR OCCURRED" << endl;
		cout << "Error Occured while parsing Mode.Please check the usage" << endl;
	}
	string trainingDirPath, testFilePath;
	switch (MODE){
	case DATA_GATHER_MODE:
		cout << "Training Directory Path : " << argv[2] << endl;
		dataGatherRun(string(argv[2]));
		break;
	case TRAINING_MODE:
		trainingModeRun();
		system("pause");
		break;
	case TEST_MODE:
		testModelRun(string(argv[2]));
		break;
	case MODEL_UPDATE_MODE:
		modelUpdateRun();
		system("pause");
		break;
	default:
		printUsage();
	}
	return 0;
}

void dataGatherRun(string trainingDataDir){
	string processedDirPath = trainingDataDir + "processed/";
	string mkdirCmd = "mkdir " + processedDirPath;
	system(mkdirCmd.c_str());
	vector<string> lstFiles = listFiles(trainingDataDir, "mov");
	for (vector<string>::iterator iter = lstFiles.begin(); iter != lstFiles.end(); ++iter){
		processVideo(trainingDataDir + (*iter), "processed/" + (*iter) + "_processed.mov");
	}
}

void trainingModeRun(){
	CvMLData& trainingData = readTrainingData(TRAINING_DATA_FILE);
	CvMLData& testData = readTrainingData(TEST_DATA_FILE);

	DataModelInfo& modelInfo = trainModel(trainingData, testData);

	cout << "ACCURACY of Data Model : " << modelInfo.accuracy << endl;
	cout << "SUMMARY : " << endl;
	cout << modelInfo.summary << endl;
	cout << "Saving Model at : " << DATA_MODEL_INFO_FILE << endl;
	map<string, int> mapClasses = trainingData.get_class_labels_map();
	int normal = mapClasses["N"];
	int abnormal = mapClasses["A"];
	ofstream outFile(DATA_MODEL_INFO_FILE);
	switch (modelInfo.modelCode){
	case SVM:
		outFile << "MODEL=" << SVM_MODEL << endl; 
		outFile << "FLIE_PATH=" << SVM_MODEL_FILE_PATH << endl;
		outFile << "NORMAL_CLASS_LABEL=" << normal << endl;
		outFile << "ABNORMAL_CLASS_LABEL=" << abnormal << endl;
		break;
	case KNN:
		outFile << "MODEL=" << KNN_MODEL << endl;
		outFile << "OPT_K=" << modelInfo.optK << endl;
		outFile << "NORMAL_CLASS_LABEL=" << normal << endl;
		outFile << "ABNORMAL_CLASS_LABEL=" << abnormal << endl;
		break;
	case NORMAL_BAYES:
		outFile << "MODEL=" << NORMAL_BAYES_MODEL << endl;
		outFile << "FILE_PATH=" << BAYES_MODEL_FILE_PATH << endl;
		outFile << "NORMAL_CLASS_LABEL=" << normal << endl;
		outFile << "ABNORMAL_CLASS_LABEL=" << abnormal << endl;
		break;
	default:
		cerr << "Unknown Model. Probably case not handled here." << endl;
		cerr << "Model won't be saved. Contact blackShadow or junebug (preferably the former)" << endl;
		break;
	}
	outFile.close();
}

void testModelRun(string testFilePath){
	g_testFilePath = testFilePath;
	cout << "Processing File : " << g_testFilePath << endl;
	// 1. Load the model from saved file
	// 2. Store the value corresponding to Normal and Abnormal Class
	g_modelInfo = loadModelFromFile(g_normalClassVal, g_abNormalClassVal);
	// 3. Process Video & Highlight Abnormal Cases
	processVideo(testFilePath, testFilePath + "_processed.mov");
}

void modelUpdateRun(){
	vector<ObjectActivity*>* vecObjs = parseUpdateModelFile();
	int rowsProcessed = 0;
	for (int iter = 0; iter < vecObjs->size(); ++iter){
		ObjectActivity* objAc = vecObjs->at(iter);
		int startFrame = objAc->firtFrame;
		int lastFrame = objAc->lastFrame;
		char response = playVideo(objAc->inputVideoFile, startFrame, lastFrame, objAc->positions);
		bool breakLoop = false;
		switch (response){
		case 'A':
		case 'a':
			objAc->classLabel = 'A';
			rowsProcessed++;
			break;
		case 'n':
		case 'N':
			objAc->classLabel = 'N';
			rowsProcessed++;
			break;
		case 'Q':
		case 'q':
			breakLoop = true;
			break;
		default:
			cerr << "Unknown Response" << endl;
			cerr << "Program won't process this row and all the rest..." << endl;
			system("pause");
			breakLoop = true;
		}
		if (breakLoop){
			break;
		}
		stringstream dataStr;
		dataStr << objAc->speed << "," 
			<< objAc->intercept << "," << objAc->slope << "," << objAc->avgX << "," << objAc->avgY << "," 
			<< (objAc->lastFrame - objAc->firtFrame) << "," << objAc->classLabel;
		appendToFile(TRAINING_DATA_FILE, dataStr.str());
	}

	vector<string> lines = readFileLineByLine(UPDATE_DATA_MODEL_FILE);
	fstream outFile(UPDATE_DATA_MODEL_FILE);
	for (int iter = rowsProcessed; iter < lines.size(); iter++){
		outFile << lines[iter] << endl;
	}
	outFile.close();

	
	trainingModeRun();
}

void processVideo(string inputFilePath, string outputFilePath){
	cout << "Processing : " << inputFilePath << endl;
	int frameCount = 0;

	cv::VideoCapture cap(inputFilePath);
	if (!cap.isOpened()){
		cerr << "Failed to open the file : " << inputFilePath << endl;
		system("pause");
		return;
	}
	else{
		//do nothing.
	}
	cv::VideoWriter writer;
	
	writer.open(outputFilePath, cap.get(CV_CAP_PROP_FOURCC),
		cap.get(CV_CAP_PROP_FPS), cv::Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT)));
	cv::Mat frame, fore, back;
	cv::BackgroundSubtractorMOG2 bgs;
	std::vector<std::vector<cv::Point> > contours;
	vector<TrackedObject*>* vecTrackedObj = new vector<TrackedObject*>();
	bool firstFrame = true;
	while (cap.read(frame)){
		frameCount++;
		if (g_FrameHt == -1){
			g_FrameHt = frame.rows;
			g_FrameWidth = frame.cols;
		}
		else{
			//do nothing
		}

		bgs.operator ()(frame, fore);
		bgs.getBackgroundImage(back);
		cv::medianBlur(fore, fore, 11);
		cv::threshold(fore, fore, 100, 255, cv::THRESH_BINARY);
		cv::Mat strEl = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		//fill up the holes first 
		//Get it back now
		cv::erode(fore, fore, strEl, cv::Point(-1, -1), 3);
		cv::dilate(fore, fore, strEl, cv::Point(-1, -1), 8);
		cv::imshow("Fore", fore);
		pause();
		contours.clear();
		cv::findContours(fore, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		processContours(frame, contours, vecTrackedObj, frameCount);
		stringstream outputss;
		outputss << "Objects Tracked : " << vecTrackedObj->size();
		putText(frame, outputss.str(), cv::Point(10, frame.rows - 10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 0), 2);
		writer.write(frame);
		cv::imshow("Video", frame);
		pause();
	}
	writer.release();
	cap.release();
	cv::destroyAllWindows();
}


void processContours(cv::Mat& frame, std::vector<std::vector<cv::Point> > contours, vector<TrackedObject*>* vecTrackedObj, int frameCount){
	/*
	Updating inactive count for all currently tracked objects
	This would help in cleaning them
	*/
	for (int objIter = 0; objIter < vecTrackedObj->size(); objIter++){
		(*vecTrackedObj)[objIter]->reportInactive();
	}

	for (int contIter = 0; contIter < contours.size(); contIter++){
		cv::Rect boundingBox = cv::boundingRect(contours[contIter]);
		int centerX = boundingBox.x + (boundingBox.width / 2);
		int centerY = boundingBox.y + (boundingBox.height / 2);
		int* currPos = new int[2];
		currPos[0] = centerX;
		currPos[1] = centerY;
		bool objFound = false;

		for (int objIter = 0; objIter < vecTrackedObj->size(); objIter++){
			TrackedObject* ptrObj = (*vecTrackedObj)[objIter];
			int* lastTrackedPos = ptrObj->getCurrentPos();
			if (arePointsClose(lastTrackedPos, currPos)){
				ptrObj->updatePosition(centerX, centerY);
				objFound = true;
				break;
			}
			//Reclaiming memory.
			delete lastTrackedPos;
		}

		if (!objFound){
			TrackedObject* ptrTrackedObj = new TrackedObject(g_ID++, centerX, centerY, frameCount);
			vecTrackedObj->push_back(ptrTrackedObj);
		}

		//We no longer require currPos. Freeing memory here
		delete currPos;
	}
	cv::Mat results(1000,1, CV_32F);
	if (MODE == TEST_MODE){
		predict( vecTrackedObj, results);
	}

	//CLEAN UP
	for (int objIter = 0; objIter < vecTrackedObj->size(); objIter++){
		TrackedObject* ptrTrackedObj = (*vecTrackedObj)[objIter];
		if (ptrTrackedObj->getInactiveCount() > 5){
			vecTrackedObj->erase(vecTrackedObj->begin() + objIter);
			objIter--;
			ptrTrackedObj->updateLastFrame(frameCount);
			if (MODE == DATA_GATHER_MODE){
				ptrTrackedObj->evaluateParameters();
				ptrTrackedObj->writeResults();
			}
			else if(MODE == TEST_MODE){
				string data = ptrTrackedObj->getDataForUpdate(g_testFilePath);
				appendToFile(UPDATE_DATA_MODEL_FILE, data);
			}
			else{
				//do nothing. Go Ahead
			}
			delete ptrTrackedObj;
		}
		else{
			int* currPos = ptrTrackedObj->getCurrentPos();
			if (MODE == TEST_MODE){
				if (results.at<float>(objIter, 0) == 1.0f){// g_abNormalClassVal){
					cv::rectangle(frame, cv::Rect(currPos[0] - 20, currPos[1] - 20, 20, 20), cv::Scalar(0, 0, 255), 2);
					cv::circle(frame, cv::Point(currPos[0], currPos[1]), 3, cv::Scalar(0, 0, 255), 3);
				}
				else{
					//do nothing. RELAX!!!
					cv::circle(frame, cv::Point(currPos[0], currPos[1]), 3, cv::Scalar(255, 0, 0), 3);
				}
			}
			else{
				cv::circle(frame, cv::Point(currPos[0], currPos[1]), 3, cv::Scalar(255, 0, 0), 3);
			}
			stringstream oss;
			oss << ptrTrackedObj->getID();
			cv::putText(frame, oss.str(), cv::Point(currPos[0] - 10, currPos[1] - 10), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 2);
			//We are done with the location. Not Required. Claiming memory back. < evil laugh >
			delete currPos;
		}
	}
}

void pause(void){
	switch (cvWaitKey(1)){
	default:
		break;
	}
}


bool arePointsClose(int* p1, int* p2){
	// Return true if points are within 10 unit radius of each other
	return (pow(p1[0] - p2[0], 2) < 2500 && pow(p1[1] - p2[1], 2) < 1500);
}

vector<string> getFiles(string dirPath, string type){
	vector<string> retVec;
	DIR *dir;
	struct dirent *ent;

	/* open directory stream */
	dir = opendir(dirPath.c_str());
	if (dir != NULL) {

		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			string fileName = string(ent->d_name);
			int index = fileName.find(type);
			if (index >= 0 && index < fileName.length()){
				retVec.push_back(fileName);
			}
		}

		closedir(dir);
	}
	else {
		cerr << "Error : Could not open directory : " << dirPath << endl;
	}
	return retVec;
}

void predict(vector<TrackedObject*>* vecTrackedObjects, cv::Mat&  results){
	/*
	 * Format : Speed, Intercept, Slope, AvgX, AvgY, Life(Frame), Class 
	 */
	cv::Mat testData(vecTrackedObjects->size(), 6, CV_32F);

	for (int iterationCount = 0; iterationCount < vecTrackedObjects->size(); ++iterationCount){
		TrackedObject* object = vecTrackedObjects->at(iterationCount);
		object->evaluateParameters();
		testData.at<float>(iterationCount, 0) = 1.0f *object->getSpeed();
		testData.at<float>(iterationCount, 1) = 1.0f *object->getIntercept();
		testData.at<float>(iterationCount, 2) = 1.0f *object->getSlope();
		testData.at<float>(iterationCount, 3) = 1.0f *object->getAvgX();
		testData.at<float>(iterationCount, 4) = 1.0f *object->getAvgY();
		testData.at<float>(iterationCount, 5) = 1.0f *object->getLife();
	//	testData.at<float>(iterationCount, 6) = 0.0f;
	}

	switch (g_modelInfo->modelCode){
	case SVM:
		((CvSVM&)g_modelInfo->model).predict(testData, results);
		break;
	case KNN:
		
		((CvKNearest&)g_modelInfo->model).find_nearest(testData, g_modelInfo->optK, results, cv::Mat(), cv::Mat());
		break;
	case NORMAL_BAYES:
		((CvNormalBayesClassifier&)g_modelInfo->model).predict(testData, &results);
		break;
	default:
		cerr << "Unknown Model Code. Software need to be upgraded acoordingly" << endl;
		cerr << "Program will now quit" << endl;
		system("pause");
	}
}

char playVideo(string inputFilePath, int startFrame, int endFrame, vector<int*>* positions){
	int frameCount = 0;
	cv::VideoCapture cap(inputFilePath);
	if (!cap.isOpened()){
		cerr << "Failed to open the file : " << inputFilePath << endl;
		system("pause");
		return '\0';
	}
	else{
		//do nothing.
	}
	cv::Mat frame;
	vector<int*>::iterator posIter = positions->begin();
	while (cap.read(frame)){
		frameCount++;
		if (frameCount >= startFrame){
			if (frameCount > endFrame){
				break;
			}
			else{
				//do nothing Display.
			}


			int* currPos = *posIter;

			cv::circle(frame, cv::Point(currPos[0], currPos[1]), 25, cv::Scalar(0, 0, 255), 3);
			if (posIter != positions->end()){
				posIter++;
				if (posIter == positions->end()){
					posIter--;
				}
			}
			else{
				//do nothing
			}

			cv::imshow("Update Model Video", frame);
			switch (cvWaitKey(4)){
			default:
				break;
			}
		}
	}
	char response;
	bool firstTime = true;
	do{
		if (!firstTime){
			cout << "\tValid Values : A/N/Q" << endl;
		}
		cout << "Normal / Abnormal / Quit( N / A / Q) : ";
		cin >> response;
		firstTime = false;
	} while (response != 'a' && response != 'A' && response != 'n' && response != 'N' && response != 'q' && response != 'Q');
	return response;
}

void printUsage(void){
	cout << "TheEye.exe <MODE> <Input Params...>" << endl;
	cout << "Modes & Input Parameters: " << endl << endl;
	cout << "------ DATA GATHER MODE ------" << endl;
	cout << "USAGE : TheEye.exe 0 <TRAINING DIR PATH>" << endl;
	cout << "\tMODE = 0 : For Data Gather Mode" << endl;
	cout << "\tTRAINING DIR PATH : Directory containing all the videos to gather training data" << endl << endl;
	cout << "------ TRAINING MODE ------" << endl;
	cout << "USAGE : TheEye.exe 1" << endl;
	cout << "\tMODE = 1 : For Training Mode" << endl << endl;
	cout << "------ TEST MODE ------" << endl;
	cout << "USAGE : TheEye.exe 2 <INPUT FILE PATH>" << endl;
	cout << "\tMODE = 2 : For Test Mode" << endl;
	cout << "INPUT FILE PATH : Absolute path of the input file to run in test mode" << endl << endl;
	cout << "------ MODEL UPDATE MODE ------" << endl;
	cout << "USAGE : TheEye.exe 3" << endl;
	cout << "\tMODE = 3 : For Model Update Mode" << endl;
	cout << "-------------------------------" << endl;
	cout << "That's all folks" << endl;
	cout << "-------------------------------" << endl;

}