#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <stdio.h>
#include "dirent.h"
#include <string.h>
#include "dog_breed.h"
#include <fstream>
using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay(Mat frame);
void read_image_train(string& directory, vector<Mat>& images, vector<int>& labels, int label);
void read_image_test(string& directory, vector<Mat>& images, double& count);


/** Global variables */
String face_cascade_name = "haarcascade_frontalcatface_extended.xml";
String eyes_cascade_name = "haarcascade_eye.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);



/** @function main */
int main(int argc, const char** argv)
{
	
	vector<Mat> trainimages;
	vector<Mat> testimages;
	vector<int> labels;
	int confusion[34][34][3];
	memset(confusion, 0, sizeof(confusion));
	string train;
	string test;
	string PCA = "../model/PCA_Model_test128.xml";
	string LBP = "../model/LBP_Model_test128.xml";
	string Fisher = "../model/Fisher_Model_test128.xml";
	int index = 1;
	char findex[25];
	int j = 0;
	Ptr<FaceRecognizer> modelPCA = createEigenFaceRecognizer();
	Ptr<FaceRecognizer> modelFisher = createFisherFaceRecognizer();
	Ptr<FaceRecognizer> modelLBP = createLBPHFaceRecognizer();

	for (int i = 0; i < 34; i++){

		train = "../dogface_train/" + dog_breeds[i];
		read_image_train(train, trainimages, labels, i + 1);
	}

	modelPCA->train(trainimages, labels);
	modelPCA->save(PCA);
	cout << "PCA finished" << endl;
	modelLBP->train(trainimages, labels);
	modelLBP->save(LBP);
	cout << "LBP finished" << endl;
	modelFisher->train(trainimages, labels);
	modelFisher->save(Fisher);
	cout << "Fisher finished" << endl;

	modelPCA->load(PCA);
	modelLBP->load(LBP);
	modelFisher->load(Fisher);

	double count = 0;
	int test_number = 0;
	double correct_PCA = 0;
	double correct_LBP = 0;
	double correct_Fisher = 0;


	for (int i = 0; i < 34; i++){

		test = "../dogface_test/" + dog_breeds[i];
		read_image_test(test, testimages, count);
		cout << "This is the category " << i + 1 << " " << dog_breeds[i] << endl;

		for (int j = 0; j < count; j++){

				int predictPCA = modelPCA->predict(testimages[test_number] );
				int predictLBP = modelLBP->predict(testimages[test_number]);
				int predictFisher = modelFisher->predict(testimages[test_number]);
				test_number++;
				cout << "PCA = " << predictPCA << " LBP = " << predictLBP << " Fisher = " << predictFisher << endl;
				confusion[i][predictPCA - 1][0]++;
				confusion[i][predictLBP - 1][1]++;
				confusion[i][predictFisher - 1][2]++;
		
		}
	}
	string filename[3] = { "../confusion_matrix/PCA.txt", "../confusion_matrix/LBP.txt", "../confusion_matrix/Fisher.txt" };
	for (int f = 0; f < 3; f++){
		ofstream fout(filename[f]);
		for (int i = 0; i < 34; i++){
			for (int j = 0; j < 34; j++){
				fout << confusion[i][j][f] << " ";
			}
			fout << endl;
		}
	};
	return 0;
}

void read_image_train(string& directory, vector<Mat>& images, vector<int>& labels, int label)
{
	DIR *dir;
	struct dirent *ent;
	string imgName;
	string imgPath;
	Mat image;
	Mat tt;
	int k = 0;
	int limit = 0;
	dir = opendir(directory.c_str());
	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (limit >= 50)
				break;
			imgName = ent->d_name;
			if (imgName.compare(".") == 0 | imgName.compare("..") == 0) { continue; }
			imgPath = directory + "/" + ent->d_name;
			//cout << imgPath << endl;
			Mat image;
			resize(imread(imgPath, 0), image, Size(128, 128));

			images.push_back(image);
			labels.push_back(label);
			//cout << images.capacity() << endl;
			limit++;
		}
	}

}
void read_image_test(string& directory, vector<Mat>& images, double& count)
{
	DIR *dir;
	struct dirent *ent;
	string imgName;
	string imgPath;
	Mat image;
	count = 0;
	dir = opendir(directory.c_str());
	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			imgName = ent->d_name;
			if (imgName.compare(".") == 0 | imgName.compare("..") == 0) { continue; }
			imgPath = directory + "/" + ent->d_name;
			//cout << imgPath << endl;
			Mat image;
			resize(imread(imgPath, 0), image, Size(128, 128));
			images.push_back(image);
			count++;
		}
	}

}
