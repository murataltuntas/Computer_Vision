#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

#define Img_Or_Camera 0  // 0 camera , 1 image

int threshold_value = 70;          // threshold degeri
int threshold_type = 0;            // Binary Inverted Threshold
int const max_BINARY_value = 255;  // maksimum threshold degeri

// fiducial merkez noktasini bulan fonksiyon
void fiducialPoint(Mat &corner, vector<vector<Point> > contours, int cIndex, Scalar color, int fid);

int main(int argc, char** argv)
{
	Mat src, cdst, src_gray, dst_Thre, gaus, corner;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened()) // check if we succeeded
		return -1;
	for (;;)
	{
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
										hryArr[6] = hierarchy[hryArr[5]][2];
										if (hierarchy[hryArr[6]][2] != -1) // cocugu varsa
										{
											hryArr[7] = hierarchy[hryArr[6]][2];
											// 2 .fiducial
											if (hierarchy[hryArr[7]][1] == -1 && hierarchy[hryArr[7]][0] == -1) // kardesi yoksa
											{
												drawContours(src, contours, i, Scalar(0, 255, 255), 2, LINE_AA, hierarchy, 0, Point());
												drawContours(corner, contours, i, Scalar(255, 255, 255), 2, LINE_AA, hierarchy, 0, Point());
												fiducialPoint(corner, contours, i, Scalar(0, 255, 255), 2);
											}
											// 3. fiducial
											else if (hierarchy[hryArr[7]][1] != -1 || hierarchy[hryArr[7]][0] != -1) // kardesi varsa
											{
												drawContours(src, contours, i, Scalar(255, 0, 0), 2, LINE_AA, hierarchy, 0, Point());
												drawContours(corner, contours, i, Scalar(255, 255, 255), 2, LINE_AA, hierarchy, 0, Point());
												fiducialPoint(corner, contours, i, Scalar(255, 0, 0), 3);
											}
										}
										// 1 .fiducial
										else{ // cocugu yoksa
											drawContours(src, contours, i, Scalar(0, 255, 0), 2, LINE_AA, hierarchy, 0, Point());
											drawContours(corner, contours, i, Scalar(255, 255, 255), 2, LINE_AA, hierarchy, 0, Point());
											fiducialPoint(corner, contours, i, Scalar(0, 255, 0), 1);
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
void fiducialPoint(Mat &corner, vector<vector<Point> > contours, int cIndex, Scalar color, int fid){
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

	if (fid == 1)
		cout << "1. Fiducial: " << x << "," << y << endl;	
	else if (fid == 2)
		cout << "2. Fiducial: " << x << "," << y << endl;	
	else if (fid == 3)
		cout << "3. Fiducial: " << x << "," << y << endl;	
}
