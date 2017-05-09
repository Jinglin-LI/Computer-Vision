#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;

void runOnWindow(int W1, int H1, int W2, int H2, Mat inputImage, char *outName) {
	int rows = inputImage.rows;
	int cols = inputImage.cols;

	vector<Mat> i_planes;
	split(inputImage, i_planes);
	Mat iB = i_planes[0];
	Mat iG = i_planes[1];
	Mat iR = i_planes[2];

	// dynamically allocate RGB arrays of size rows x cols
	int** R = new int*[rows];
	int** G = new int*[rows];
	int** B = new int*[rows];
	for (int i = 0; i < rows; i++) {
		R[i] = new int[cols];
		G[i] = new int[cols];
		B[i] = new int[cols];
	}

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {
			R[i][j] = iR.at<uchar>(i, j);
			G[i][j] = iG.at<uchar>(i, j);
			B[i][j] = iB.at<uchar>(i, j);
		}


	//	   The transformation should be based on the
	//	   historgram of the pixels in the W1,W2,H1,H2 range.
	//	   The following code goes over these pixels

	double a_min = 1.0;
	double b_max = 0.0;
	for (int i = H1; i <= H2; i++) {
		for (int j = W1; j <= W2; j++) {
			double r = R[i][j];
			double g = G[i][j];
			double b = B[i][j];
			r = r / 255;
			g = g / 255;
			b = b / 255;

			if (r < 0.03928) {
				r = r / 12.92;
			}
			else {
				r = pow((r + 0.055) / 1.055, 2.4);
			}

			if (g < 0.03928) {
				g = g / 12.92;
			}
			else {
				g = pow((g + 0.055) / 1.055, 2.4);
			}

			if (b < 0.03928) {
				b = b / 12.92;
			}
			else {
				b = pow((b + 0.055) / 1.055, 2.4);
			}

			// RGB => XYZ, find the a_min and b_max
			double Y = 0.212671 * r + 0.71516 * g + 0.072169 * b;

			if (Y < a_min) {
				a_min = Y;
			}
			if (Y > b_max) {
				b_max = Y;
			}
		}
	}
//	cout << a_min << ", " << b_max << "\n";

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			double r = R[i][j];
			double g = G[i][j];
			double b = B[i][j];

			r = r / 255;
			g = g / 255;
			b = b / 255;

			//invgamma
			if (r < 0.03928) {
				r = r / 12.92;
			}
			else {
				r = pow((r + 0.055) / 1.055, 2.4);
			}

			if (g < 0.03928) {
				g = g / 12.92;
			}
			else {
				g = pow((g + 0.055) / 1.055, 2.4);
			}

			if (b < 0.03928) {
				b = b / 12.92;
			}
			else {
				b = pow((b + 0.055) / 1.055, 2.4);
			}

			// RGB => XYZ
			long double X = 0.0;
			long double Y = 0.0;
			long double Z = 0.0;
			X = 0.412453 * r + 0.35758 * g + 0.180423 * b;
			Y = 0.212671 * r + 0.71516 * g + 0.072169 * b;
			Z = 0.019334 * r + 0.119193 * g + 0.950227 * b;

			double x, y;
			if (X + Y + Z == 0) {
				x = 0;
				y = 0;
			}
			else {
				x = X / (X + Y + Z);
				y = Y / (X + Y + Z);
			}
	//		cout << Y << ", ";

			if (Y < a_min) {
				Y = 0.0;
			}
			else if (Y > b_max) {
				Y = 1.0;
			}
			else {
				Y = (Y - a_min) * 1 / (b_max - a_min);
			}
			
	//		cout << Y << "\n";

			// new X2, Y2, Z2 
			double X2, Y2, Z2;
			if (y == 0) {
				X2 = 0;
				Y2 = Y;
				Z2 = 0;
			}
			else {
				X2 = (x / y) * Y;
				Y2 = Y;
				Z2 = (1.0 - x - y) * Y / y;
			}
			
	//		cout << Y << ", " << Y2 << "\n";


			// New XYZ => Linear sRGB
			long double sR = 0.0;
			long double sG = 0.0;
			long double sB = 0.0;

			sR = 3.240479 * X2 - 1.53715 * Y2 - 0.498535 * Z2;
			sG = -0.969256 * X2 + 1.875991 * Y2 + 0.041556 * Z2;
			sB = 0.055648 * X2 - 0.204043 * Y2 + 1.057311 * Z2;

			if (sR < 0.0) {
				sR = 0.0;
			}
			if (sR > 1.0) {
				sR = 1.0;
			}
			if (sG < 0.0) {
				sG = 0.0;
			}
			if (sG > 1.0) {
				sG = 1.0;
			}
			if (sB < 0.0) {
				sB = 0.0;
			}
			if (sB > 1.0) {
				sB = 1.0;
			}

			// Gamma correlation, linear sRGB => Non-linear sRGB
			if (sR < 0.00304) {
				sR = sR * 12.92;
			}
			else {
				sR = pow(sR, 1.0 / 2.4) * 1.055 - 0.055;
			}

			if (sG < 0.00304) {
				sG = sG * 12.92;
			}
			else {
				sG = pow(sG, 1.0 / 2.4) * 1.055 - 0.055;
			}

			if (sB < 0.00304) {
				sB = sB * 12.92;
			}
			else {
				sB = pow(sB, 1.0 / 2.4) * 1.055 - 0.055;
			}

			// Multiple 255 
			R[i][j] = (int)(sR * 255);
			G[i][j] = (int)(sG * 255);
			B[i][j] = (int)(sB * 255);

		}
	}

	Mat oR(rows, cols, CV_8UC1);
	Mat oG(rows, cols, CV_8UC1);
	Mat oB(rows, cols, CV_8UC1);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {
			oR.at<uchar>(i, j) = R[i][j];;
			oG.at<uchar>(i, j) = G[i][j];;
			oB.at<uchar>(i, j) = B[i][j];;
		}

	Mat o_planes[] = { oB, oG, oR };
	Mat outImage;
	merge(o_planes, 3, outImage);

	namedWindow("output", CV_WINDOW_AUTOSIZE);
	imshow("output", outImage);
	imwrite(outName, outImage);
}

int main(int argc, char** argv) {
	if (argc != 7) {
		cerr << argv[0] << ": "
			<< "got " << argc - 1
			<< " arguments. Expecting six: w1 h1 w2 h2 ImageIn ImageOut."
			<< endl;
		cerr << "Example: proj1b 0.2 0.1 0.8 0.5 fruits.jpg out.bmp" << endl;
		return(-1);
	}
	double w1 = atof(argv[1]);
	double h1 = atof(argv[2]);
	double w2 = atof(argv[3]);
	double h2 = atof(argv[4]);
	char *inputName = argv[5];
	char *outputName = argv[6];

	if (w1<0 || h1<0 || w2 <= w1 || h2 <= h1 || w2>1 || h2>1) {
		cerr << " arguments must satisfy 0 <= w1 < w2 <= 1"
			<< " ,  0 <= h1 < h2 <= 1" << endl;
		return(-1);
	}

	Mat inputImage = imread(inputName, CV_LOAD_IMAGE_UNCHANGED);
	if (inputImage.empty()) {
		cout << "Could not open or find the image " << inputName << endl;
		return(-1);
	}

	string windowInput("input: ");
	windowInput += inputName;

	namedWindow(windowInput, CV_WINDOW_AUTOSIZE);
	imshow(windowInput, inputImage);

	if (inputImage.type() != CV_8UC3) {
		cout << inputName << " is not a standard color image  " << endl;
		return(-1);
	}

	int rows = inputImage.rows;
	int cols = inputImage.cols;
	int W1 = (int)(w1*(cols - 1));
	int H1 = (int)(h1*(rows - 1));
	int W2 = (int)(w2*(cols - 1));
	int H2 = (int)(h2*(rows - 1));

	runOnWindow(W1, H1, W2, H2, inputImage, outputName);

	waitKey(0); // Wait for a keystroke
	return(0);
}
