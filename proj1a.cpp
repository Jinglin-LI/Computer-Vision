#include "opencv2/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	if (argc != 3) {
		cout << argv[0] << ": "
			<< "got " << argc - 1 << " arguments. Expecting two: width height."
			<< endl;
		return(-1);
	}

	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	int** RED1 = new int*[height];
	int** GREEN1 = new int*[height];
	int** BLUE1 = new int*[height];
	int** RED2 = new int*[height];
	int** GREEN2 = new int*[height];
	int** BLUE2 = new int*[height];

	for (int i = 0; i < height; i++) {
		RED1[i] = new int[width];
		GREEN1[i] = new int[width];
		BLUE1[i] = new int[width];
		RED2[i] = new int[width];
		GREEN2[i] = new int[width];
		BLUE2[i] = new int[width];
	}

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			int r1, g1, b1;
			int r2, g2, b2;

			double x = (double)j / (double)width;
			double y = (double)i / (double)height;
			double Y = 1.0;

			double L = 90;
			double u = x * 512 - 255;
			double v = y * 512 - 255;
			
			/*
			xyY -> Non-linear sRGB
			1. xyY -> XYZ
			2. XYZ -> Linear sRGB
			3. Linear sRGB -> Non-linear sRGB
			*/

			// calculate XYZ based on xyY
			double X, Z;
			if (i == 0) {
				X = 0.0;
				Z = 0.0;
			}
			else {
				X = x / y;
				Z = (1 - x - y) / y;
			}

			// matrix multiplication
			double sR, sG, sB;
			sR = 3.240479 * X - 1.53715 * Y - 0.498535 * Z;
			sG = -0.969256 * X + 1.875991 * Y + 0.041556 * Z;
			sB = 0.055648 * X - 0.204043 * Y + 1.057311 * Z;

			if (sR < 0)
				sR = 0;
			if (sR > 1)
				sR = 1;
			if (sG < 0)
				sG = 0;
			if (sG > 1)
				sG = 1;
			if (sB < 0)
				sB = 0;
			if (sB > 1)
				sB = 1;

			// gamma correction
			if (sR < 0.00304) {
				sR = sR * 12.92;
			}
			else {
				sR = pow(sR, 1 / 2.4) * 1.055 - 0.055;
			}
			if (sG < 0.00304) {
				sG = sG * 12.92;
			}
			else {
				sG = pow(sG, 1 / 2.4) * 1.055 - 0.055;
			}
			if (sB < 0.00304) {
				sB = sB * 12.92;
			}
			else {
				sB = pow(sB, 1 / 2.4) * 1.055 - 0.055;
			}

			r1 = (int)(sR * 255);
			g1 = (int)(sG * 255);
			b1 = (int)(sB * 255);
			
/*================================================================================*/

			/*
			Luv -> Non-linear sRGB
			1. Luv -> XYZ
			2. XYZ -> Linear sRGB
			3. Linear sRGB -> Non-linear sRGB
			*/

			// Luv -> Non-linear sRGB
			double uw = 3.8 / 19.22;
			double vw = 9.0 / 19.22;

			double u1, v1;
			u1 = (u + 13 * uw * L) / (13 * L);
			v1 = (v + 13 * vw * L) / (13 * L);

			double X2, Y2, Z2;
			Y2 = pow((L + 16) / 116, 3) * 1.0;
			if (v1 == 0) {
				X2 = 0;
				Z2 = 0;
			}
			else {
				X2 = Y2 * 2.25 * u1 / v1;
				Z2 = Y2 * (3 - 0.75 * u1 - 5 * v1) / v1;
			}

			// matrix multiplication
			double sR2, sG2, sB2;
			sR2 = 3.240479 * X2 - 1.53715 * Y2 - 0.498535 * Z2;
			sG2 = -0.969256 * X2 + 1.875991 * Y2 + 0.041556 * Z2;
			sB2 = 0.055648 * X2 - 0.204043 * Y2 + 1.057311 * Z2;

			if (sR2 < 0)
				sR2 = 0;
			if (sR2 > 1)
				sR2 = 1;
			if (sG2 < 0)
				sG2 = 0;
			if (sG2 > 1)
				sG2 = 1;
			if (sB2 < 0)
				sB2 = 0;
			if (sB2 > 1)
				sB2 = 1;

			// gamma correction
			if (sR2 < 0.00304) {
				sR2 = sR2 * 12.92;
			}
			else {
				sR2 = pow(sR2, 1 / 2.4) * 1.055 - 0.055;
			}
			if (sG2 < 0.00304) {
				sG2 = sG2 * 12.92;
			}
			else {
				sG2 = pow(sG2, 1 / 2.4) * 1.055 - 0.055;
			}
			if (sB2 < 0.00304) {
				sB2 = sB2 * 12.92;
			}
			else {
				sB2 = pow(sB2, 1 / 2.4) * 1.055 - 0.055;
			}

			r2 = (int)(sR2 * 255);
			g2 = (int)(sG2 * 255);
			b2 = (int)(sB2 * 255);


			// this is the end of your code

			RED1[i][j] = r1;
			GREEN1[i][j] = g1;
			BLUE1[i][j] = b1;
			RED2[i][j] = r2;
			GREEN2[i][j] = g2;
			BLUE2[i][j] = b2;
		}


	Mat R1(height, width, CV_8UC1);
	Mat G1(height, width, CV_8UC1);
	Mat B1(height, width, CV_8UC1);

	Mat R2(height, width, CV_8UC1);
	Mat G2(height, width, CV_8UC1);
	Mat B2(height, width, CV_8UC1);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {

			R1.at<uchar>(i, j) = RED1[i][j];
			G1.at<uchar>(i, j) = GREEN1[i][j];
			B1.at<uchar>(i, j) = BLUE1[i][j];

			R2.at<uchar>(i, j) = RED2[i][j];
			G2.at<uchar>(i, j) = GREEN2[i][j];
			B2.at<uchar>(i, j) = BLUE2[i][j];
		}

	Mat xyY;
	Mat xyY_planes[] = { B1, G1, R1 };
	merge(xyY_planes, 3, xyY);
	namedWindow("xyY", CV_WINDOW_AUTOSIZE);
	imshow("xyY", xyY);

	Mat Luv;
	Mat Luv_planes[] = { B2, G2, R2 };
	merge(Luv_planes, 3, Luv);
	namedWindow("Luv", CV_WINDOW_AUTOSIZE);
	imshow("Luv", Luv);
	waitKey(0); // Wait for a keystroke
	return(0);
}
