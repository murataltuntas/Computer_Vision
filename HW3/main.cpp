/**
*	111044043
*   Murat ALTUNTAS
*   Computer Vision
*   HW3
*/

#include "opencv2/opencv.hpp"
#include <iostream>
#include <exception>

using namespace cv;
using namespace std;

#define Img_Or_Camera 0  // 0 camera , 1 image

int threshold_value = 70;          // threshold degeri
int threshold_type = 0;            // Binary Inverted Threshold
int const max_BINARY_value = 255;  // maksimum threshold degeri

// fiducial merkez noktasini bulan fonksiyon
void fiducialPoint(Mat &corner, vector<vector<Point> > contours, int cIndex, Scalar color, int fid, vector<Point2f> &imgPoints, vector<Point3f> &objPoints);

int main(int argc, char** argv)
{
	Mat src, cdst, src_gray, dst_Thre, gaus, corner;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Mat intrisicMat(3, 3, cv::DataType<double>::type); // Intrisic matrix
	Mat distCoeffs(5, 1, cv::DataType<double>::type);   // Distortion vector
	vector<Point2f> imagePoints;  // fiducial'larin goruntudeki kose noktalarý
	vector<Point3f> objectPoints; // fiducial larin uzerindeki kose noktalari, z'leri 0
	vector<Point3f> cubePoints;   // 3 boyutlu kubun noktalari

	// camera matrix 
	intrisicMat.at<double>(0, 0) = 8.0590569657930405e+002;
	intrisicMat.at<double>(0, 1) = 0;
	intrisicMat.at<double>(0, 2) = 3.1992663716840809e+002;

	intrisicMat.at<double>(1, 0) = 0;
	intrisicMat.at<double>(1, 1) = 8.0402874361053591e+002;
	intrisicMat.at<double>(1, 2) = 2.8882990821476335e+002;

	intrisicMat.at<double>(2, 0) = 0;
	intrisicMat.at<double>(2, 1) = 0;
	intrisicMat.at<double>(2, 2) = 1;

	// distortion coefficients 
	distCoeffs.at<double>(0) = -3.7248947688123030e-001;
	distCoeffs.at<double>(1) = 2.6888027840614788e+000;
	distCoeffs.at<double>(2) = 2.6268461713588056e-003;
	distCoeffs.at<double>(3) = -5.2527099349920147e-003;
	distCoeffs.at<double>(4) = -7.0073154990556654e+000;

	// cube
	cubePoints.push_back(Point3d(0, 0, 0));
	cubePoints.push_back(Point3d(0, 1, 0));
	cubePoints.push_back(Point3d(1, 0, 0));
	cubePoints.push_back(Point3d(1, 1, 0));
	cubePoints.push_back(Point3d(0, 0, -1));
	cubePoints.push_back(Point3d(0, 1, -1));
	cubePoints.push_back(Point3d(1, 0, -1));
	cubePoints.push_back(Point3d(1, 1, -1));

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened()) // check if we succeeded
		return -1;
	for (;;)
	{

		Mat rVec(3, 1, cv::DataType<double>::type);
		Mat tVec(3, 1, cv::DataType<double>::type);
		vector<Point2f> outImage;

		cap >> src;         // kameradan yeni bir frame alinir
		flip(src, src, 1);  // kamera goruntusu ters oldugu icin goruntu cevrilir

		corner = Mat::zeros(src.rows, src.cols, CV_8UC3); // Mat initialize
		// Convert the image to Gray
		cvtColor(src, src_gray, CV_BGR2GRAY);
		// gaussian ile smooting
		GaussianBlur(src_gray, gaus, Size(3, 3), 0, 0);
		// threshold
		threshold(src_gray, dst_Thre, threshold_value, max_BINARY_value, threshold_type);
		//cv::imshow("threshold", dst_Thre);
		// edge detected
		Canny(dst_Thre, cdst, 100, 200, 3);
		// Contours bulan fonksiyon
		findContours(cdst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		int hryArr[10]; // hierarsi dizisi


		// contour sayisi kdr donen dongu
		for (int i = 0; i < contours.size(); i++){
			outImage.clear();
			objectPoints.clear();
			imagePoints.clear();

			// hierarchy de, 0 next - 1 previous - 2 child - 3 parent
			if (hierarchy[i][2] != -1 && hierarchy[i][3] == -1) // cocugu varsa ve parenti yoksa
			{
				hryArr[0] = hierarchy[i][2];
				if (hierarchy[hryArr[0]][2] != -1) // cocugu varsa
				{
					hryArr[1] = hierarchy[hryArr[0]][2];
					if (hierarchy[hryArr[1]][2] != -1) // cocugu varsa
					{
						hryArr[2] = hierarchy[hryArr[1]][2];
						if (hierarchy[hryArr[2]][2] != -1) // cocugu varsa
						{
							hryArr[3] = hierarchy[hryArr[2]][2];
							if (hierarchy[hryArr[3]][2] != -1) // cocugu varsa
							{
								hryArr[4] = hierarchy[hryArr[3]][2];
								if (hierarchy[hryArr[4]][2] != -1) // cocugu varsa
								{
									hryArr[5] = hierarchy[hryArr[4]][2];
									if (hierarchy[hryArr[5]][2] != -1) // cocugu varsa
									{
										approxPolyDP(Mat(contours[i]), contours[i], 3, true);

										circle(corner, contours[i][0], 5, Scalar(0, 0, 255), 5, 8, 0);
										circle(corner, contours[i][1], 5, Scalar(0, 255, 0), 5, 8, 0);
										circle(corner, contours[i][2], 5, Scalar(255, 0, 0), 5, 8, 0);
										circle(corner, contours[i][3], 5, Scalar(0, 255, 255), 5, 8, 0);

										cout << contours[i][0] << endl;
										cout << contours[i][1] << endl;
										cout << contours[i][2] << endl;
										cout << contours[i][3] << endl;

										hryArr[6] = hierarchy[hryArr[5]][2];
										if (hierarchy[hryArr[6]][2] != -1) // cocugu varsa
										{
											hryArr[7] = hierarchy[hryArr[6]][2];
											// 2 .fiducial
											if (hierarchy[hryArr[7]][1] == -1 && hierarchy[hryArr[7]][0] == -1) // kardesi yoksa
											{
												drawContours(src, contours, i, Scalar(0, 255, 255), 2, LINE_AA, hierarchy, 0, Point());
												drawContours(corner, contours, i, Scalar(255, 255, 255), 2, LINE_AA, hierarchy, 0, Point());
												fiducialPoint(corner, contours, i, Scalar(0, 255, 255), 2, imagePoints, objectPoints);

												if (objectPoints.size() != 0)
												{
													solvePnP(objectPoints, imagePoints, intrisicMat, distCoeffs, rVec, tVec);
													projectPoints(cubePoints, rVec, tVec, intrisicMat, distCoeffs, outImage);
													for (int i = 0; i < outImage.size(); i++)
														for (int j = 0; j < outImage.size(); j++)
															line(src, outImage[i], outImage[j], Scalar(0, 255, 255), 3, 2, 0);
												}

											}
											// 3. fiducial
											else if (hierarchy[hryArr[7]][1] != -1 || hierarchy[hryArr[7]][0] != -1) // kardesi varsa
											{
												drawContours(src, contours, i, Scalar(255, 0, 0), 2, LINE_AA, hierarchy, 0, Point());
												drawContours(corner, contours, i, Scalar(255, 255, 255), 2, LINE_AA, hierarchy, 0, Point());
												fiducialPoint(corner, contours, i, Scalar(255, 0, 0), 3, imagePoints, objectPoints);

												if (objectPoints.size() != 0)
												{
													solvePnP(objectPoints, imagePoints, intrisicMat, distCoeffs, rVec, tVec);
													projectPoints(cubePoints, rVec, tVec, intrisicMat, distCoeffs, outImage);
													for (int i = 0; i < outImage.size(); i++)
														for (int j = 0; j < outImage.size(); j++)
															line(src, outImage[i], outImage[j], Scalar(255, 0, 0), 3, 2, 0);
												}

											}
										}
										// 1 .fiducial
										else{ // cocugu yoksa
											drawContours(src, contours, i, Scalar(0, 255, 0), 2, LINE_AA, hierarchy, 0, Point());
											drawContours(corner, contours, i, Scalar(255, 255, 255), 2, LINE_AA, hierarchy, 0, Point());
											fiducialPoint(corner, contours, i, Scalar(0, 255, 0), 1, imagePoints, objectPoints);

											if (objectPoints.size() != 0)
											{
												solvePnP(objectPoints, imagePoints, intrisicMat, distCoeffs, rVec, tVec);
												projectPoints(cubePoints, rVec, tVec, intrisicMat, distCoeffs, outImage);
												for (int i = 0; i < outImage.size(); i++)
													for (int j = 0; j < outImage.size(); j++)
														line(src, outImage[i], outImage[j], Scalar(0, 255, 0), 3, 2, 0);
											}											

										}
									}
								}
							}
						}
					}
				}
			}
		}	

		cv::imshow("Original Image and Contours", src);  // Contours
		cv::imshow("Center of Contours", corner);       // center

		if (waitKey(30) >= 0)     // bir tusa basilinca  donguden cikar
			break;
	}

	return 0;
}
// fiducial merkez noktasini bulan fonksiyon
void fiducialPoint(Mat &corner, vector<vector<Point> > contours, int cIndex, Scalar color, int fid, vector<Point2f> &imgPoints, vector<Point3f> &objPoints){
	double refArea = 0;        // gecici alan
	bool objectFound = false;
	Moments moment = moments((cv::Mat)contours[cIndex]); // moment'e contouru veriyorum
	double area = moment.m00; // contourun alani
	int x = 0, y = 0;         // merkezin x ve y noktasi
	if (area > refArea){
		x = moment.m10 / area; // x noktasi
		y = moment.m01 / area; // y noktasi
		objectFound = true;
		refArea = area;
	}
	else objectFound = false;
	if (objectFound == true){
		stringstream sx, sy;
		sx << x;
		sy << y;
		putText(corner, sx.str() + "," + sy.str(), Point(x, y + 30), 1, 1, color, 2);
		//draw object location on screen
		circle(corner, Point(x, y), 5, color, 2);
	}

	imgPoints.push_back(Point2d(contours[cIndex][0].x, contours[cIndex][0].y));
	imgPoints.push_back(Point2d(contours[cIndex][1].x, contours[cIndex][1].y));
	imgPoints.push_back(Point2d(contours[cIndex][2].x, contours[cIndex][2].y));
	imgPoints.push_back(Point2d(contours[cIndex][3].x, contours[cIndex][3].y));
	objPoints.push_back(Point3d(0, 0, 0));
	objPoints.push_back(Point3d(0, 1, 0));
	objPoints.push_back(Point3d(1, 1, 0));
	objPoints.push_back(Point3d(1, 0, 0));

	if (fid == 1){
		cout << "1. Fiducial: " << x << "," << y << endl;
	}
	else if (fid == 2){
		cout << "2. Fiducial: " << x << "," << y << endl;
	}
	else if (fid == 3){
		cout << "3. Fiducial: " << x << "," << y << endl;
	}
}
