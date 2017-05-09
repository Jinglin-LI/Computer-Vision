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

	// a is h(i), b is f(i), c is (f(i - 1) + f(i)) * n / (2 * k)
	int a[101] = {0};
	int b[101] = {0};
	int c[101] = {0};

	for (int i = H1; i <= H2; i++) {
		for (int j = W1; j <= W2; j++) {
			double r = R[i][j];
			double g = G[i][j];
			double b = B[i][j];

			r = r / 255;
			g = g / 255;
			b = b / 255;

			// ingbamma correlation 
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

			// RGB => XYZ, only Y is needed
			long double Y = 0.212671 * r + 0.71516 * g + 0.072169 * b;
			
			// XYZ => Luv
			long double t = Y / 1.0;
			long double L;
			if (t > 0.008856) {
				L = pow(t, 1.0 / 3.0) * 116 - 16;
			}
			else {
				L = 903.3 * t;
			}

			// ceil L and calculate the number of L in the same range
			if (L < 100) {
				L = (int)L + 1;
			}
			a[(int)L]++; 
		}
	}

	/*
	for (int i = 0; i < 101; i++) {
		cout << "a = " << i << ", " << a[i] << "\n";
	}
	*/

	// calculate b[i], which is f(i)
	b[0] = a[0];
	for (int i = 1; i < 101; i++) {
		b[i] = b[i - 1] + a[i];
	}

	/*
	for (int i = 0; i < 101; i++) {
		cout << i << ", " << b[i] << "\n";
	}
	*/

	// calculate c[i], which is the final histogram value
	c[0] = b[0] * 101 / (2 * b[100]);
	for (int i = 1; i < 101; i++) {
		c[i] = (b[i - 1] + b[i]) * 101 / (2 * b[100]);
		if (c[i] > 100)
			c[i] = 100;
	}
	
	/*
	for (int i = 0; i < 101; i++) {
		cout << i << ", " << c[i] << "\n";
	}
	*/

	// using the a, b, c, stretch the L in the whole picture by histogram equalization
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			double r = R[i][j];
			double g = G[i][j];
			double b = B[i][j];

			r = r / 255.0;
			g = g / 255.0;
			b = b / 255.0;

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

			// XYZ => Luv
			long double uw = 0.0;
			long double vw = 0.0;

			uw = 4 * 0.95 / (0.95 + 15.0 * 1.0 + 3.0 * 1.09);
			vw = 9 * 1.0 / (0.95 + 15.0 * 1.0 + 3.0 * 1.09);

			long double L = 0.0;

			if (Y > 0.008856) {
				L = pow(Y, (long double)(1.0 / 3.0)) * 116.0 - 16.0;
			}
			else {
				L = 903.3 * Y;
			}

			long double d = 0.0;
			d = X + 15.0 * Y + 3.0 * Z;
			long double u1 = 0.0;

			u1 = 4.0 * X / d;

			long double v1 = 0.0;
			if (d != 0)
				v1 = 9.0 * Y / d;

			long double u = 0.0;
			if (d != 0)
				u = 13.0 * L * (u1 - uw);

			long double v = 0;
			v = 13.0 * L * (v1 - vw);

			// find the ceiling of L and find the new L when perform the histogram equalization 
			if (L > 100)
				L = 100;
			else {
				L = (int)L + 1;
			}
			L = c[(int)L];

	//		cout << L << "\n";

			// Luv -> XYZ using the new L 
			long double u12 = (u + 13.0 * uw * L) / (13.0 * L);
			long double v12 = (v + 13.0 * vw * L) / (13.0 * L);

			long double X2 = 0.0;
			long double Y2 = 0.0;
			long double Z2 = 0.0;

			if (L > 7.9996) {
				Y2 = pow((L + 16.0) / 116.0, 3.0);
			}
			else {
				Y2 = L / 903.3;
			}

			if (v12 == 0.0) {
				X2 = 0.0;
				Z2 = 0.0;
			}
			else {
				X2 = Y2 * 2.25 * u12 / v12;
				Z2 = Y2 * (3.0 - 0.75 * u12 - 5.0 * v12) / v12;
			}

			//	cout << X << ", " << X2 << "\n";

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
