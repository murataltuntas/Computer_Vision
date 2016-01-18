/**
*	111044043
*	Murat ALTUNTAS
*	Computer Vision HW1
*/
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

// mouse ile pencereye tikladigimda calisan fonksiyon
static void onMouse(int event, int x, int y, int, void*);
// Histogram calculate
void myCalcHist();
// Threshold hesaplayan fonksiyon
void myThreshold(Mat& src, Mat& dst, double thresholdValue);
// part 1
int part_1();
//part 2
int part_2();

/// Global variables
char* window_name = "Murat Altuntas HW1"; // pencere adi
char* window_threshold = "Grey";          // pencere adi
char* window_hist = "Histogram";          // pencere adi
int threshold_value = 0;                  // threshold degeri
int const max_value = 255;                // maksimum track bar degeri
int threshold_type = 0;                   // Binary Threshold
int const max_BINARY_value = 255;         // maksimum threshold degeri
int arrayHist[256];                       // histogram dizisi
Mat src,			// orjinal frame
	src_gray,		// grey'e donusturulen frame
	dst;			// src_gray'den threshold'a donusturulen frame

int main(int argc, char** argv)
{
	int c;
	while (true){
		part_1();
		while (true)
		{
			c = waitKey(20);
			if ((char)c == 27)        //esc basilinca biter
			{
				destroyAllWindows();  // Butun pencereleri kapatir
				cout << "Part 1 kapandi.\n";
				break;
			}
			else if ((char)c == 13){  // Enter'a basilinca basa doner
				destroyAllWindows();  // Butun pencereleri kapatir
				break;
			}
		}
		if ((char)c == 27) //esc basilinca biter
		{
			break;
		}
	}

	while (true){
		part_2();
		while (true)
		{
			c = waitKey(20);
			if ((char)c == 27) //esc basilinca biter
			{
				cout << "Part 2 kapandi.\n";
				return 0;
			}
		}
	}
	return 0;
}

static void onMouse(int event, int x, int y, int, void*){
	if (event != EVENT_LBUTTONDOWN) // mause'in sol butonuna basildigini algilar
		return;
	cout << "Threshold value: " << (x / 2) << endl;
	//Pencereye tiklandiginda alina X konumunun degerine gore threshold alinir.
	myThreshold(src_gray, dst, (x / 2));
	// threshold'u alinan frame pencereye verilir ve pencere gosterilir.
	imshow(window_name, dst);
}

// Threshold fonksiyonu
void myThreshold(Mat& src, Mat& dst, double thresholdValue){
	for (int j = 0; j < src.rows; j++){
		uchar*srcRow = src.ptr<uchar>(j);
		uchar*dstRow = dst.ptr<uchar>(j);
		for (int i = 0; i < src.cols; i++){
			if (srcRow[i] > thresholdValue)  // threshold degerinin ustundekiler beyaz, gerisi siyah olur. (Binary threshold)
				dstRow[i] = 255;  // beyaz 
			else
				dstRow[i] = 0;	  // siyah
		}
	}
}

// histogram hesaplayan fonksiyon
void myCalcHist(Mat hist){
	memset(arrayHist, 0, 256);     // arrayin ici temizlenir.
	for (int j = 0; j < hist.rows; j++){
		uchar*row = hist.ptr<uchar>(j);
		for (int i = 0; i < hist.cols; i++){
			arrayHist[row[i]] += 1;   // gelen deger array de o indeksin degerini 1 arttirir.
		}
	}	
}

int part_1(){
	int maxValue = 0;    // arraydaki maksimum deger.
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened()) // check if we succeeded
		return -1;
	for (;;)
	{
		cap >> src;               // kameradan yeni bir frame alinir
		flip(src, src, 1);        // kamera goruntusu ters oldugu icin goruntu cevrilir
		imshow(window_name, src); // pencerede gosterilir
		if (waitKey(30) >= 0)     // bir tusa basilinca  donguden cikar
			break;
	}

	// Convert the image to Gray
	cvtColor(src, src_gray, CV_RGB2GRAY);
	cvtColor(src, dst, CV_RGB2GRAY);
	// Create a windows to display results
	cvResizeWindow(window_name, 640, 480);         // frame penceresi
	cvResizeWindow(window_threshold, 640, 480);    // frame penceresi

	namedWindow(window_hist, CV_WINDOW_AUTOSIZE);  // histogram grafigi pencersi

	// mouse tiklama fonksiyonu 
	setMouseCallback(window_hist, onMouse, 0);

	int histSize = 256;         // histogram size'i
	float range[] = { 0, 256 }; // deger araligi
	const float* histRange = { range };

	Mat hist;
	fill_n(arrayHist, 256, 0); // arrayi sifirlar.
	myCalcHist(src_gray);      // Histogram hesaplayan fonksiyonum
	// Bulunan pixel degerlerini konsol ekranina basiyorum.
	for (int j = 0; j < 256; j++){
		cout << j << ":\t" << arrayHist[j] << endl;
	}
	
	// grey'e cevrilen frame gosterilir.
	imshow(window_name, src_gray);

	// Draw the histograms for B, G and R
	int hist_w = 512;  // genislik
	int hist_h = 600;  // yukseklik
	
	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));
	maxValue = 0;   // max deger sifirlanir.
	// maksimum degeri bulan dongu
	for (int i = 0; i<histSize; i++)
	{
		if (arrayHist[i]>maxValue)
			maxValue = arrayHist[i];
	}
	// grafigi cizen dongu
	for (int i = 0; i < histSize - 1; i++){
		line(histImage, Point(2 * i, hist_h - ((double)(arrayHist[i]) / (double)maxValue * hist_h)), Point(2 * (i + 1), hist_h - ((double)(arrayHist[i + 1]) / (double)maxValue * hist_h)), Scalar(255, 255, 255), 2, 8, 0);
	}

	imshow(window_hist, histImage);
	imshow(window_threshold, dst);
}

int part_2(){
	char* trackbar_type =  "Type "; // track bar adi 
	char* trackbar_value = "Value"; // track bar adi
	int const max_type = 4;         // maksimum Threshold tipi
	/*
		0: Binary
		1: Binary Inverted
		2: Threshold Truncated
		3: Threshold to Zero
		4: Threshold to Zero Inverted
	*/
	VideoCapture capture;      
	capture.open(0);           // default olarak kamera acilir

	if (!capture.isOpened()) { // kamera kontrol edilir
		return -1;
	}

	// Sonuclari goruntulemek icin bir pencere olusturulur
	namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	/// Create Trackbar to choose type of Threshold
	createTrackbar(trackbar_type, window_name, &threshold_type, max_type);
	// Threshold degerini secmek icin bir Trackbar olusturulur
	createTrackbar(trackbar_value, window_name, &threshold_value, max_value);

	while (true) {
		capture >> src;    // kameradan yeni bir frame alinir
		flip(src, src, 1); // kamerada ayna sorununu duzeltir

		// Convert the image to Gray
		cvtColor(src, src_gray, CV_RGB2GRAY);

		// Call the function to threshold process
		threshold(src_gray, dst, threshold_value, max_BINARY_value, threshold_type);

		// dst Frame'i null degilse goster
		if (!dst.empty())
		{
			imshow(window_name, dst);
		}

		/// Wait until user finishes program
		int c;
		c = waitKey(20);
		if ((char)c == 27) { // esc ye basilinca biter
			break;
		}
	}
}