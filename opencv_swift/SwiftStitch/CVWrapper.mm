//
//  CVWrapper.m
//  CVOpenTemplate
//
//  Created by Washe on 02/01/2013.
//  Copyright (c) 2013 foundry. All rights reserved.
//

#import "CVWrapper.h"
#import "UIImage+OpenCV.h"
#import "stitching.h"
#import "UIImage+Rotate.h"
#import <opencv2/opencv.hpp>

@implementation CVWrapper

+ (UIImage*) processImageWithOpenCV: (UIImage*) inputImage
{
    NSArray* imageArray = [NSArray arrayWithObject:inputImage];
    UIImage* result = [[self class] processWithArray:imageArray];
    return result;
}

+ (UIImage*) processWithOpenCVImage1:(UIImage*)inputImage1 image2:(UIImage*)inputImage2;
{
    NSArray* imageArray = [NSArray arrayWithObjects:inputImage1,inputImage2,nil];
    UIImage* result = [[self class] processWithArray:imageArray];
    return result;
}

+ (UIImage*) processWithArray:(NSArray*)imageArray
{
    if ([imageArray count]==0){
        NSLog (@"imageArray is empty");
        return 0;
        }
    std::vector<cv::Mat> matImages;
    
    for (id image in imageArray) {
        if ([image isKindOfClass: [UIImage class]]) {
            /*
             All images taken with the iPhone/iPa cameras are LANDSCAPE LEFT orientation. The  UIImage imageOrientation flag is an instruction to the OS to transform the image during display only. When we feed images into openCV, they need to be the actual orientation that we expect them to be for stitching. So we rotate the actual pixel matrix here if required.
             */
            UIImage* rotatedImage = [image rotateToImageOrientation];
            cv::Mat matImage = [rotatedImage CVMat3];
            NSLog (@"matImage: %@",image);
            matImages.push_back(matImage);
        }
    }
    NSLog (@"stitching...");
    
   
   
    cv::Mat stitchedMat = stitch1 (matImages);
    
    UIImage* result =  [UIImage imageWithCVMat:stitchedMat];
    return result;
}

+ (NSString*) predict:(UIImage*)image
{
    NSString *path1 = [[NSBundle mainBundle] pathForResource:@"LBP_Model_test1"
                                                                ofType:@"xml"];
    NSString *path2 = [[NSBundle mainBundle] pathForResource:@"LBP_Model_test2"
                                                      ofType:@"xml"];

    cv::Ptr<cv::FaceRecognizer> modelLBPH1,modelLBPH2;
    modelLBPH1 = cv::createLBPHFaceRecognizer();
    modelLBPH1->load(path1.UTF8String);
    modelLBPH2 = cv::createLBPHFaceRecognizer();
    modelLBPH2->load(path2.UTF8String);
    cv::Mat test = [image CVMat];
    cv::Mat testresize;
    cv::resize(test, testresize, cv::Size(128,128));
    cv::Mat testresize_gray;
    cv::cvtColor(testresize, testresize_gray, CV_BGR2GRAY);
    int label1,label2;
   
    label1 = modelLBPH1->predict(testresize_gray);
    label2 = modelLBPH2->predict(testresize_gray);
    
    NSString* result=[NSString stringWithFormat:@"%d %d",label1,label2];
    return result;
}


@end
