#ifndef _LEARNING_UTIL_H
#define _LEARNING_UTIL_H

#include "stdafx.h"

struct DataModelInfo{
	
	DataModelInfo(CvStatModel& model, float accuracy) 
	: model(model), accuracy(accuracy){
		
	}

	DataModelInfo(DataModelInfo& dataModelInfo)
	:model(dataModelInfo.model), accuracy(dataModelInfo.accuracy){
		
	}

	DataModelInfo& operator = (const DataModelInfo& dataModelInfo){
		this->model = dataModelInfo.model;
		this->accuracy = dataModelInfo.accuracy;
		this->modelCode = dataModelInfo.modelCode;
		this->optK = dataModelInfo.optK;
		return *this;
	}

	CvStatModel& model;
	float accuracy;
	string summary;
	int modelCode;
	int optK = 3;

	~DataModelInfo(){
		delete &model;
	}

};

DataModelInfo& trainModel(CvMLData& trainingData, CvMLData& testData);

DataModelInfo& trainAndTuneSVM(CvMLData& trainingData, CvMLData& testData);

DataModelInfo& trainAndTuneNormalBayes(CvMLData& trainingData, CvMLData& testData);

DataModelInfo& trainAndTuneKNearest(CvMLData& trainingData, CvMLData& testData);

string getSVMSummary(CvSVM& model, CvSVMParams params);

void printConfusionMat(int** arr);

//TODO Check if Neural Nets can be trained as well

DataModelInfo& trainModel(CvMLData& trainingData, CvMLData& testData){
	DataModelInfo& model1Info = trainAndTuneSVM(trainingData, testData);
	DataModelInfo& model2Info = trainAndTuneKNearest(trainingData, testData);
	DataModelInfo& model3Info = trainAndTuneNormalBayes(trainingData, testData);
	cout << "Normal Bayes Acc : " << model3Info.accuracy << endl;
	DataModelInfo& trueModel = (model1Info.accuracy > model2Info.accuracy ? model1Info : model2Info);
	/*if (trueModel.accuracy > model3Info.accuracy){
		return trueModel;
	}
	else{
		return model3Info;
	}*/

	return trueModel;
	//return model1Info;
}

DataModelInfo& trainAndTuneNormalBayes(CvMLData& trainingData, CvMLData& testData){
	CvNormalBayesClassifier& model = *(new CvNormalBayesClassifier(trainingData.get_values(), trainingData.get_responses()));
	model.train(trainingData.get_values(), trainingData.get_responses());

	cv::Mat testDataMat(testData.get_values(), true);
	cv::Mat testDataResp(testData.get_responses(), true);

	cv::Mat* out = new cv::Mat();
	model.predict(testDataMat, out);
	int classifications, misClassifications;
	classifications = misClassifications = 0;
	int** confMat;
	confMat = new int*[2];
	confMat[0] = new int[2];
	confMat[1] = new int[2];
	confMat[0][0] = confMat[0][1] = confMat[1][0] = confMat[1][1] = 0;
	/*for (int i = 0; i < testDataResp.rows; i++){
		cout << testDataMat.at<float>(i, 5) << " --- " << out->at<float>(i, 0) << endl;
		if (testDataMat.at<float>(i, 5) == out->at<float>(i, 0)){
			classifications++;
		}
		else{
			misClassifications++;
		}
	}*/
	map<string, int> classLabels = trainingData.get_class_labels_map();
	for (int i = 0; i < testDataMat.rows; i++){
		map<string, int> testClassLabels = testData.get_class_labels_map();
		if (out->at<float>(i, 0) == classLabels["N"]){
			if (testDataMat.at<float>(i, 5) == testClassLabels["N"]){
				classifications++;
				confMat[1][1] += 1;
			}
			else{
				misClassifications++;
				confMat[0][1] += 1;
			}
		}
		else{
			if (testDataMat.at<float>(i, 5) == testClassLabels["A"]){
				classifications++;
				confMat[0][0] += 1;
			}
			else{
				misClassifications++;
				confMat[1][0] += 1;
			}
		}
		/*if (testDataMat.at<float>(i, 6) == out.at<float>(i, 0)){
		if (testDataMat.at<float>(i, 6) == classLabels["A"]){
		confMat[0][0] += 1;
		}
		else{
		confMat[1][1] += 1;
		}
		classifications++;
		}
		else{
		misClassifications++;
		if (testDataMat.at<float>(i, 6) == classLabels["A"]){
		confMat[0][1] += 1;
		}
		else{
		confMat[1][0] += 1;
		}
		}*/
	}
	cout << "SVM Confusion Matrix " << endl;
	printConfusionMat(confMat);
	float accuracy = (1.0f * classifications) / testDataMat.rows;
	cout << "****Accuracy : " << accuracy * 100 << endl;
	/*float accuracy = (1.0f * classifications) / testDataMat.rows;
	cout << "Accuracy : " << accuracy << endl;*/
	cout << "-------------------- NORMAL BAYES COMPLETE---------------------"<<endl;
	DataModelInfo& mI = *(new DataModelInfo(model, accuracy));
	stringstream summary;
	summary << "Underlying Model : Normal Bayes " << endl;
	mI.summary = summary.str();
	mI.modelCode = NORMAL_BAYES; 
	model.save(BAYES_MODEL_FILE_PATH, "BayesModel");
	return mI;
}


DataModelInfo& trainAndTuneKNearest(CvMLData& trainingData, CvMLData& testData){

	CvKNearest& model = *(new CvKNearest(trainingData.get_values(), trainingData.get_responses(), trainingData.get_train_sample_idx(), false, 40));
	model.train(trainingData.get_values(), trainingData.get_responses(), trainingData.get_train_sample_idx(), false, 40, false);
	cv::Mat out;
	cv::Mat testDataMat(testData.get_values(), true);
	cv::Mat responseMat(testData.get_responses(), true);
	
	float bestAcc = 0.0f;
	int optK = 0;

	for (int k = 1; k < 40; k++){

		model.find_nearest(testDataMat, k, out, cv::Mat(), cv::Mat());
		int** confMat;
		confMat = new int*[2];
		confMat[0] = new int[2];
		confMat[1] = new int[2];
		confMat[0][0] = confMat[0][1] = confMat[1][0] = confMat[1][1] = 0;
		map<string, int> classLabels = trainingData.get_class_labels_map();
		int classifications, misClassifications;
		classifications = misClassifications = 0;
		for (int i = 0; i < testDataMat.rows; i++){
			if (testDataMat.at<float>(i, 5) == out.at<float>(i, 0)){
				classifications++;
			}
			else{
				misClassifications++;
			}
		}
		float accuracy = (1.0f * classifications) / testDataMat.rows;
		if (optK == 0 || bestAcc < accuracy){
			bestAcc = accuracy;
			optK = k;
		}
		else{
			//do nothing
		}
	}
	
	cout << " Best Accuracy  : " << bestAcc << endl; 
	cout << " Optimal K : " << optK << endl;
	{
		model.find_nearest(testDataMat, optK, out, cv::Mat(), cv::Mat());
		int** confMat;
		confMat = new int*[2];
		confMat[0] = new int[2];
		confMat[1] = new int[2];
		confMat[0][0] = confMat[0][1] = confMat[1][0] = confMat[1][1] = 0;
		map<string, int> classLabels = trainingData.get_class_labels_map();
		int classifications, misClassifications;
		classifications = misClassifications = 0;
		for (int i = 0; i < testDataMat.rows; i++){
			if (testDataMat.at<float>(i, 5) == out.at<float>(i, 0)){
				if (testDataMat.at<float>(i, 5) == classLabels["A"]){
					confMat[0][0] += 1;
				}
				else{
					confMat[1][1] += 1;
				}
				classifications++;
			}
			else{
				misClassifications++;
				if (testDataMat.at<float>(i, 5) == classLabels["A"]){
					confMat[0][1] += 1;
				}
				else{
					confMat[1][0] += 1;
				}
			}
		}
		cout << "KNN Confusiont Matrix" << endl;
		cout << "Accuracy : " << bestAcc * 100 << endl;
		printConfusionMat(confMat);
	}
	cout << "-------------------- K -NEAREST COMPLETE---------------------" << endl;
	DataModelInfo& modelInfo = *(new DataModelInfo (model, bestAcc));
	stringstream summary;
	summary << "Underlying Model : K Nearest Neighbours" << endl;
	summary << "K : " << optK << endl;
	summary << "Accuracy on Test Data Set : " << (modelInfo.accuracy * 100) << endl;
	modelInfo.summary = summary.str();
	modelInfo.optK = optK;
	modelInfo.modelCode = KNN;
	return modelInfo;
}

DataModelInfo& trainAndTuneSVM(CvMLData& trainingData, CvMLData& testData){
	CvSVM& model = *(new CvSVM(trainingData.get_values(), trainingData.get_responses(), cv::Mat(), cv::Mat()));
	//CvSVMParams params;
	/*model.train_auto(trainingData.get_values(), trainingData.get_responses(), cv::Mat(), cv::Mat(), params, 5,
		CvSVM::get_default_grid(CvSVM::C), CvSVM::get_default_grid(CvSVM::GAMMA), CvSVM::get_default_grid(CvSVM::P),
		CvSVM::get_default_grid(CvSVM::NU), CvSVM::get_default_grid(CvSVM::COEF), CvSVM::get_default_grid(CvSVM::DEGREE), true);*/

	/*CvParamGrid grid = CvSVM::get_default_grid(CvSVM::C);
	grid.max_val = 100;
	grid.min_val = 0.1;
	grid.step = 0.1;*/

	CvSVMParams params;
	params.C = 0.5;
	params.gamma = 0.00001;
	params.kernel_type = CvSVM::RBF;
	params.svm_type = CvSVM::C_SVC;
	//params.nu = 0.5;
	CvParamGrid paramGrid = CvParamGrid(0.1, 100.0, 0.1);
	model.train(trainingData.get_values(), trainingData.get_responses(), cv::Mat(), cv::Mat(), params);
	//model.train_auto(trainingData.get_values(), trainingData.get_responses(), cv::Mat(), cv::Mat(), params, 10, paramGrid, paramGrid, paramGrid, paramGrid, paramGrid,paramGrid, true);
	//model.train_auto(trainingData.get_values(), trainingData.get_responses(), cv::Mat(), cv::Mat(), params );
	cout << "Support Vector Count : " << model.get_support_vector_count() << endl;
	
	cout << "No Of Rows in Test Data : " << testData.get_values()->rows << endl;
	cv::Mat testDataMat(testData.get_values(), true);
	cv::Mat out;
	model.predict(testDataMat, out);
	
	int** confMat = new int*[2];
	confMat[0] = new int[2];
	confMat[1] = new int[2];
	confMat[0][0] = confMat[0][1] = confMat[1][0] = confMat[1][1] = 0;
	map<string, int> classLabels = trainingData.get_class_labels_map();
	int classifications, misClassifications;
	classifications = misClassifications = 0;
	for (int i = 0; i < testDataMat.rows; i++){
		map<string, int> testClassLabels = testData.get_class_labels_map();
		if (out.at<float>(i, 0) == classLabels["N"]){
			if (testDataMat.at<float>(i, 5) == testClassLabels["N"]){
				classifications++;
				confMat[1][1] += 1;
			}
			else{
				misClassifications++;
				confMat[0][1] += 1;
			}
		}
		else{
			if (testDataMat.at<float>(i, 5) == testClassLabels["A"]){
				classifications++;
				confMat[0][0] += 1;
			}
			else{
				misClassifications++;
				confMat[1][0] += 1;
			}
		}
		/*if (testDataMat.at<float>(i, 6) == out.at<float>(i, 0)){
			if (testDataMat.at<float>(i, 6) == classLabels["A"]){
				confMat[0][0] += 1;
			}
			else{
				confMat[1][1] += 1;
			}
			classifications++;
		}
		else{
			misClassifications++;
			if (testDataMat.at<float>(i, 6) == classLabels["A"]){
				confMat[0][1] += 1;
			}
			else{
				confMat[1][0] += 1;
			}
		}*/
	}
	cout << "SVM Confusion Matrix " << endl;
	printConfusionMat(confMat);
	float accuracy = (1.0f * classifications) / testDataMat.rows;
	cout << "****Accuracy : " << accuracy * 100 << endl;
	cout << "-------------------- SVM COMPLETE---------------------" << endl;
	DataModelInfo& mI = *(new DataModelInfo(model, accuracy));
	stringstream summary;
	params = model.get_params();
	mI.summary = getSVMSummary(model, params);
	mI.modelCode = SVM;
	model.save(SVM_MODEL_FILE_PATH);
	return mI;
}

string getSVMSummary(CvSVM& model ,CvSVMParams params){
	stringstream summary;
	summary << "C : " << params.C << endl;
	summary << "SVM Type : \t";
	switch (params.svm_type){
	case CvSVM::C_SVC:
		summary << "C-Support Vector Classification" << endl;
		break;
	case CvSVM::NU_SVC:
		summary << "Nu-Support Vector Classification" << endl;
		summary << "Nu : " << params.nu << endl;
		break;
	case CvSVM::ONE_CLASS:
		summary << "One Class Classficiation" << endl;
		break;
	default:
		summary << "Unknown" << endl;
	}
	summary << "Kernel Type :\t";
	switch (params.kernel_type){
	case CvSVM::LINEAR:
		summary << "LINEAR" << endl;
		break;
	case CvSVM::POLY:
		summary << "POLYNOMIAL " << endl;
		summary << "Degree : " << params.degree << endl;
		summary << "Gamma : " << params.gamma << endl;
		break;
	case CvSVM::RBF:
		summary << "Radial Basis Function (Gaussian) " << endl;
		summary << "Gamma : " << params.gamma << endl;
		break;
	case CvSVM::SIGMOID:
		summary << "Sigmoid Function" << endl;
		summary << "Gamma : " << params.gamma << endl;
		summary << "Coef0 : " << params.coef0 << endl;
		break;
	default:
		summary << "Unknown" << endl;
	}
	summary << "Support Vector Count : " << model.get_support_vector_count() << endl;
	return summary.str();
}


DataModelInfo* loadModelFromFile(float& normal, float& abnormal){
	vector<string> vecLines = readFileLineByLine(DATA_MODEL_INFO_FILE);
	string modelInfoLine = vecLines[0];
	if (contains(modelInfoLine, SVM_MODEL)){
		cout << "Underlying Model : SVM" << endl;
		CvSVM& model = *(new CvSVM());
		model.load(SVM_MODEL_FILE_PATH);
		DataModelInfo* modelInfo = new DataModelInfo(model, 0.0);
		string normalLine = vecLines[2];
		normalLine = normalLine.substr(normalLine.find("=") + 1);
		normal = atoi(normalLine.c_str());
		string abNormalLine = vecLines[3];
		abNormalLine = abNormalLine.substr(abNormalLine.find("=") + 1);
		abnormal = atoi(abNormalLine.c_str());
		modelInfo->modelCode = SVM;
		return modelInfo;
	}
	else if (contains(modelInfoLine, KNN_MODEL)){
		cout << "Underlyiing Model : KNN" << endl;
		CvMLData& trainingData = readTrainingData(KNN_MODEL_FILE_PATH);
		CvKNearest& model = *(new CvKNearest(trainingData.get_values(), trainingData.get_responses(), trainingData.get_train_sample_idx(), false, 40));
		model.train(trainingData.get_values(), trainingData.get_responses(), trainingData.get_train_sample_idx(), false, 40, false);
		DataModelInfo* modelInfo = new DataModelInfo(model, 0.0);
		string optKLine = vecLines[1];
		optKLine = optKLine.substr(optKLine.find("=") + 1);
		modelInfo->optK = atoi(optKLine.c_str());
		cout << "Using Optimum K : " << modelInfo->optK << endl;
		string normalLine = vecLines[2];
		normalLine = normalLine.substr(normalLine.find("=") + 1);
		normal = atoi(normalLine.c_str());
		string abNormalLine = vecLines[3];
		abNormalLine = abNormalLine.substr(abNormalLine.find("=") + 1);
		abnormal = atoi(abNormalLine.c_str());
		modelInfo->modelCode = KNN;
		cout << "MODEL INFO IN LOAD : " << modelInfo->optK << endl;
		return modelInfo;
	}
	else if (contains(modelInfoLine, NORMAL_BAYES_MODEL)){
		cout << "Underlying Model : Bayes Model" << endl;
		CvNormalBayesClassifier& model = *(new CvNormalBayesClassifier());
		model.load(BAYES_MODEL_FILE_PATH);
		DataModelInfo* modelInfo = new DataModelInfo(model, 0.0);
		string normalLine = vecLines[2];
		normalLine = normalLine.substr(normalLine.find("=") + 1);
		normal = atoi(normalLine.c_str());
		string abNormalLine = vecLines[3];
		abNormalLine = abNormalLine.substr(abNormalLine.find("=") + 1);
		abnormal = atoi(abNormalLine.c_str());
		modelInfo->modelCode = NORMAL_BAYES;
		return modelInfo;
	}
	else{
		//do nothing
	}
	return NULL;
}


void printConfusionMat(int** arr){
	cout << "\tA\tN" << endl;
	cout << "A\t" << arr[0][0] << "\t" << arr[0][1] << endl;
	cout << "N\t" << arr[1][0] << "\t" << arr[1][1] << endl;
}

#endif