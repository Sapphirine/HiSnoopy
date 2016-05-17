#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "dirent.h"
#include <string.h>
#include "dog_breed.h"

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay(Mat frame);
void read_image_train(string& directory, vector<Mat>& images, vector<int>& labels, int label);
void read_image_test(string& directory, vector<Mat>& images, double& count);
void predict(string& directory);
int compare(const void *a, const void *b);

/** Global variables */
String face_cascade_name = "haarcascade_frontalcatface_extended.xml";
String eyes_cascade_name = "haarcascade_eye.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
struct element{
	double confidence;
	int label;
};


/** @function main */
int main(int argc, const char** argv)
{
	element PCAarray[17], LBParray[17], Fisherarray[17];
	vector<Mat> trainimages;
	vector<Mat> testimages;
	vector<int> labels;

	string train;
	string test;
	string PCA = "../model/PCA_Model_test";
	string LBP = "../model/LBP_Model_test";
	string Fisher = "../model/Fisher_Model_test";
	int index = 1;
	char findex[25];
	int j = 0;
	Ptr<FaceRecognizer> modelPCA = createEigenFaceRecognizer();
	Ptr<FaceRecognizer> modelFisher = createFisherFaceRecognizer();
	Ptr<FaceRecognizer> modelLBP = createLBPHFaceRecognizer();
	
	for (int i = 0; i < 14; i++){
		
		train = "../dogface_train/" + dog_breeds[subarray1[i] - 1];
		read_image_train(train, trainimages, labels, subarray1[i]);
	}
		
	modelPCA->train(trainimages, labels);
	modelPCA->save(PCA  + "1.xml");
	modelLBP->train(trainimages, labels);
	modelLBP->save(LBP + "1.xml");
	modelFisher->train(trainimages, labels);
	modelFisher->save(Fisher + "1.xml");
		
	vector<Mat>().swap(trainimages);
	vector<int>().swap(labels);

	for (int i = 0; i < 16; i++){
		train = "../dogface_train/" + dog_breeds[subarray2[i] - 1];
		read_image_train(train, trainimages, labels, subarray2[i]);
	}
	modelPCA->train(trainimages, labels);
	modelPCA->save(PCA + "2.xml");
	modelLBP->train(trainimages, labels);
	modelLBP->save(LBP + "2.xml");
	modelFisher->train(trainimages, labels);
	modelFisher->save(Fisher + "2.xml");
	vector<Mat>().swap(trainimages);
	vector<int>().swap(labels);


	//modelPCA->load("../model/PCA_Model_test.xml");
	//modelFisher->load("../model/Fisher_Model_test.xml");
	//modelLBP->load("../model/LBP_Model_test.xml");

	double count = 0;
	int test_number = 0;
	double correct_PCA = 0;
	double correct_LBP = 0;
	double correct_Fisher = 0;
	string LBPHfile = "../accuracy/LBPH64.txt";
	string Fisherfile = "../accuracy/Fisher64.txt";
	string PCAfile = "../accuracy/PCA64.txt";
	ofstream foutLBPH(LBPHfile);
	ofstream foutFisher(Fisherfile);
	ofstream foutPCA(PCAfile);
	//cout << " ";
	
	for (int i = 0; i < 30; i++){
		
		test = "../dogface_test/" + dog_breeds[subarray[i]-1];
		read_image_test(test, testimages, count);
		cout << "This is the category " << subarray[i] << endl;
		
		for (int j = 0; j < count; j++){
			for (int m = 0; m < 2; m++){

				_itoa_s(m+1, findex, 10);
				Ptr<FaceRecognizer> modelPCA = createEigenFaceRecognizer();
				Ptr<FaceRecognizer> modelFisher = createFisherFaceRecognizer();
				Ptr<FaceRecognizer> modelLBP = createLBPHFaceRecognizer();

				modelPCA->load(PCA + findex + ".xml");
				modelFisher->load(Fisher + findex + ".xml");
				modelLBP->load(LBP + findex + ".xml");
				
				modelPCA->predict(testimages[test_number], PCAarray[m].label, PCAarray[m].confidence);
				modelLBP->predict(testimages[test_number], LBParray[m].label, LBParray[m].confidence);
				modelFisher->predict(testimages[test_number], Fisherarray[m].label, Fisherarray[m].confidence);
				
				
			}
			//cout << "finish one picture" << endl;
			test_number++;
			//qsort(PCAarray, 17, sizeof(PCAarray[0]), compare);
			//qsort(LBParray, 17, sizeof(LBParray[0]), compare);
			//qsort(Fisherarray, 17, sizeof(Fisherarray[0]), compare);
			if (PCAarray[0].label == subarray[i] | PCAarray[1].label == subarray[i])
				correct_PCA++;
			//cout << LBParray[0].label << " " << LBParray[1].label << endl;
			if (LBParray[0].label == subarray[i] | LBParray[1].label == subarray[i])
				correct_LBP++;
			if (Fisherarray[0].label == subarray[i] | Fisherarray[1].label == subarray[i])
				correct_Fisher++;
		}
		cout << "Category: " << dog_breeds[i] << endl;
		cout << "PCA accuracy: " << correct_PCA / count << " LBP accuracy: " << correct_LBP / count <<
				" Fisher accuracy: " << correct_Fisher / count << endl;
		foutFisher << correct_Fisher / count << " ";
		foutLBPH << correct_LBP / count << " ";
		foutPCA << correct_PCA / count << " ";
		//cout << "LBP accuracy: " << correct_LBP / count << endl;
		correct_PCA = 0;
		correct_LBP = 0;
		correct_Fisher = 0;
		test_number = 0;
		vector<Mat>().swap(testimages);
	}

	//-- 1. Load the cascades
	//if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	//if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	
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
			resize(imread(imgPath,0), image, Size(64, 64));
			
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
			resize(imread(imgPath, 0), image, Size(64, 64));
			images.push_back(image);
			count++;
		}
	}

}
int compare(const void *a, const void *b)
{
	return (*(const element*)a).confidence > (*(const element*)b).confidence;
}
/** @function detectAndDisplay */
void detectAndDisplay(Mat frame)
{
	vector<Rect> faces(10);
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1,10, 0 | CV_HAAR_SCALE_IMAGE, Size(80, 80));

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		ellipse(frame, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);

		Mat faceROI = frame_gray(faces[i]);
		//std::vector<Rect> eyes;

		//-- In each face, detect eyes
		//eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

		/*for (size_t j = 0; j < eyes.size(); j++)
		{
		Point center(faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5);
		int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
		circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
		}*/
	}
	//-- Show what you got
	//vector<Rect>(faces).swap(faces);
	imshow(window_name, frame);
	waitKey(500);
}