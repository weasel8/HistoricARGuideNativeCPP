// #pragma once
#include <string.h>
#include <jni.h>
#include "opencv2/core/core.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <map>

using namespace std;
using namespace cv;

/// Type Defs
// First int is ID, second int is counter and a contour
typedef std::map<int, std::pair<vector<Point>, int> > ContoursMap;

/// "Global" vars
// Variable to switch between buffers
bool bufferSwitch = true;
// Number of remaining frames a historic image will be displayed
int remainingContourFrames = 0;
// Minimum seconds the historic image will be displayed, if no other match was found
int minSecsToShowARImg = 2;
// Number of frames a new mapped historic image will be displayed
int arImgFrames = 5;
// Stores the current width of the passed inner frame
int innerFrameWidth;
// Stores the current height of the passed inner frame
int innerFrameHeight;
// Stores the current width of the passed inner frame
int cameraFrameWidth;
// Stores the current height of the passed inner frame
int cameraFrameHeight;
// Indicates whether the UI switch is turned on or of, to distinct between "Siegestor" and "Feldherrnhalle"
bool switchState = false;

/// Define structure dimensions and pos of zenith
// Feldherrnhalle
const float FELDHERRNHALLE_WIDTH = 106.4f;
const float FELDHERRNHALLE_HEIGHT = 46.0f;
const float FELDHERRNHALLE_ZENITH_PERC_POS_X = 0.5f;
const float FELDHERRNHALLE_ZENITH_PERC_POS_Y = 84.0f / 363.0f;
// Siegestor
const float SIEGESTOR_WIDTH = 125.0f;
const float SIEGESTOR_HEIGHT = 54.0f;
const float SIEGESTOR_PERC_POS_X = 0.5f;
const float SIEGESTOR_PERC_POS_Y = 265.0f / 546.0f;

// Init structure values
float archHeight = FELDHERRNHALLE_HEIGHT;
float archWidth = FELDHERRNHALLE_WIDTH;
float archZenithPercPosX = FELDHERRNHALLE_ZENITH_PERC_POS_X;
float archZenithPercPosY = FELDHERRNHALLE_ZENITH_PERC_POS_Y;
bool archHasBackground = true;

// The entire RGB camera frame
Mat cameraRgb;
// Stores RGB frame before it will be processes
Mat originalRGBImg;
// Stores the passed historic image that will be used to create AR
Mat arImage;
// Stores the transformed historic image
Mat resizedARImage;
// The point which was touched at 
Point touchedPoint;

// Number of arches, that are considered for the average width calculation
int number_of_arch_widths = arImgFrames * 2;
// The last index, a width was pushed to
int last_pushed_index = 0;
// Stores the widths of the arches
vector<int> last_arch_widths(number_of_arch_widths);

// Stores the values of the matched 
int old_middle_arch_width = 0;
vector<Point> old_middle_arch;
Rect old_boundingRec;

// Store the previous found contours
vector<vector<Point>> prevFoundArchContours;
// Init the switchBuffer for binary images
vector<vector<vector<Point>>> switchBuffer(2);
// Store contours that can be compared against detected contours
vector<vector<Point>> searchContours;
// Global ContoursMap
ContoursMap globalContoursMap;

// Center of the frame
Point center;

// Define basic colors
Scalar white = Scalar(255, 255, 255);
Scalar black = Scalar(0, 0, 0);
Scalar red = Scalar(255, 0, 0);
Scalar green = Scalar(0, 255, 0);
Scalar blue = Scalar(0, 0, 255);

/// Method Declarations
void generateSearchContours();

void updateSettings(int innerWidth, int innerHeight, int frameWidth, int frameHeight);

void resetOldMiddleArchProps();

void applySobel(Mat& mGray, int kernel_size = 5);

void applyBlurAndCanny(Mat& mGray, double factor = 0.5);

void opticalDetectionDebug(Mat& mRgb, Mat& mGray);

void opticalDetection(Mat& mRgb, Mat& mGray);

Point getCenterOfContour(vector<Point> contour);

float getLongestContourSide(vector<Point> contour);

void mapARImageOnMiddleArch(Mat mRgb, vector<vector<Point>> contours, float middleArchDiameter);

void mapARImageOnZenithOfMiddleArch(Mat mRgb, vector<Point> middleArch, float middleArchDiameter, bool newArchFound);

vector<Point> convertContourToOtherMat(Mat srcMat, Mat dstMat, vector<Point> contour);

Point getZenithOfContour(vector<Point> contour);

vector<Point> getAtPointedArch(Mat mRgb, vector<vector<Point>> contours);

bool contourIsArch(vector<Point> contour);

bool pointsAreAtOuterPos(Point point1, Point point2, vector<Point> contour);

bool pointIsAtBottom(Point point, vector<Point> contour);

float getSideLength(Point p1, Point p2);

float getOppositeAngleOfSideC(float sideA, float sideB, float sideC);

bool pointsAtSamePos(Point p1, Point p2, float maxDeviation = 0.2f);

bool contourCentersMatching(vector<Point> contour1, vector<Point> contour2, float maxBoundAreaDiff = 0.8f, float maxCenterDiff = 0.5f);

bool pointInRect(Point point, Rect rect);

vector<vector<Point>> eliminateDuplicateContours(vector<vector<Point>> contours);

vector<vector<Point>> extractContours(Mat grayImg, int minShapeArea = 50, int minSideLength = 20, int minVerts = 3, int maxVerts = 24);

vector<vector<Point>> pruneContours(Mat mRgb, vector<vector<Point>> contours, int minPoints = 4, float maxRotArea = 0.66f, double maxProportion = 4.0, bool onlySpecContours = true, double shapeAccuracy = 0.15);

vector<Point> extractArch(vector<Point> contour);

ContoursMap contoursToContoursMap(vector<vector<Point>> contours, int maxNumOfMisses = 3);

vector<vector<Point>> contoursMapToContours(ContoursMap contoursMap);

ContoursMap matchContoursMaps(vector<vector<Point>> newContours, ContoursMap oldContoursMap, float maxDeviation = 0.25f, float matchShapeDeviation = 0.05f, int maxNumOfMisses = 3);

vector<vector<Point>> matchContours(vector<vector<Point>> contours1, vector<vector<Point>> contours2, float maxDeviation = 0.25f, float matchShapeDeviation = 0.05f);

bool contoursRoughlyAtSamePos(vector<Point> contour1, vector<Point> contour2, float maxDeviation = 0.1f, int maxTotalDiffer = 1);

void setBlackPixelsTransparent(Mat image, Mat backgroundImg);

void changeAlphaValue(Mat image, Mat backgroundImg, float opaquePercentage = 0.4f);

void drawFoundContours(Mat srcDst, vector<vector<Point>> foundContours, int thickness = 2);

vector<Point> createArch(int radius, int numOfPointsInSemiCircle = 5, float scaleX = 1.0f, float scaleY = 1.0f, float circleScaleX = 0.33f, bool archExtension = true);

void drawBorder(Mat mRgb, int borderSize = 4);

/// Native Method Declarations
/*
To allow for overloading of functions, C++ uses something called ** name mangling **.
This means that function names are not the same in C++ as in plain C.
To inhibit this name mangling, you have to declare functions as extern �C�
*/
extern "C" {
	JNIEXPORT void JNICALL Java_com_tum_historicarguide_MainActivity_setupDetection(JNIEnv *env, jobject instance, jint innerWidth, jint innerHeight, jint frameWidth, jint frameHeight, jlong addrARImg);

	JNIEXPORT void JNICALL Java_com_tum_historicarguide_MainActivity_setSwitchState(JNIEnv *env, jobject instance, jboolean switchState);

	JNIEXPORT void JNICALL Java_com_tum_historicarguide_MainActivity_nativeOpticalDetectionDebug(JNIEnv *env, jobject instance, jlong innerRgba, jlong addrRgba, jfloat fps);

	JNIEXPORT void JNICALL Java_com_tum_historicarguide_MainActivity_nativeOpticalDetection(JNIEnv *env, jobject instance, jlong innerRgba, jlong addrRgba, jfloat fps);
}// END extern "C"