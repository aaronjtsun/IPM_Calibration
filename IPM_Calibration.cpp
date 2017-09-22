#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

//#define linear

double average(double* arr, int size)
{
	double sum = 0;
	for (int i = 0; i < size; ++i) {
		sum += arr[i];
	}
	return sum/size;
}

int main( int argc, char* argv[])
{
	//dimensions of image, 4000 x 3000 pixels
	int m, n;
	
	//predetermined cx cy cz camera distance from origin, measured in field test
	double cx, cy, cz;
	
	//diagonal, horizontal, and vertical FOV (half of given value)
	double alpha, alpha_h, alpha_v;

	//center left, center right, far left, far right x-coordinates in field test calibration image
	double cl, cr, fl, fr;

	//number of points in field test calibration image
	int points = 32;

#ifdef linear
	//fill in values for m, n, alpha, alpha_h, alpha_v, in pixels and radians respectively
	m = 4000;
	n = 3000;
	alpha = (110.0/2)*M_PI/180;
	alpha_h = (98.7/2)*M_PI/180;
	alpha_v = (82.3/2)*M_PI/180;
	
	//if only given diagonal FOV (alpha), use this instead to approximate alpha_h and alpha_v
	/*
	double den = sqrt(pow(m-1, 2) + pow(n-1, 2));
	alpha_h = atan((m-1)/den * tan(alpha));
	alpha_v = atan((n-1)/den * tan(alpha));
	*/

	//fill in values for cx, cy, cz, cl, cr, fl, fr, in terms of feet
	cx = 0/12.0;
	cy = -95/12.0;
	cz = 61.5/12.0;
	cl = -21.5/12.0;
	cr = 21.5/12.0;
	fl = (-21.5-186.0)/12.0;
	fr = (21.5+196.0)/12.0; 
#else
	//fill in values for m, n, alpha, alpha_h, alpha_v, in pixels and radians respectively
	m = 4000;
	n = 3000;
	alpha = (129.5/2)*M_PI/180;
	alpha_h = (122.5/2)*M_PI/180;
	alpha_v = (94.6/2)*M_PI/180;
	
	//if only given diagonal FOV (alpha), use this instead to approximate alpha_h and alpha_v
	/*
	double den = sqrt(pow(m-1, 2) + pow(n-1, 2));
	alpha_h = atan((m-1)/den * tan(alpha));
	alpha_v = atan((n-1)/den * tan(alpha));
	*/

	//fill in values for cx, cy, cz, cl, cr, fl, fr, in terms of feet
	cx = 0/12.0;
	cy = -95/12.0;
	cz = 61.5/12.0;
	cl = -21.5/12.0;
	cr = 21.5/12.0;
	fl = (-21.5-186.0)/12.0;
	fr = (21.5+196.0)/12.0; 
#endif	

	//fill in actual x,y coordinates of points in calibration image as measured in field test, in terms of feet
	double xy[points][2] = {{fl, 16+1/12.0},{fl, 32+2/12.0},{fl,48+1/12.0},{fl,64+1/12.0},{fl,80+1/12.0},{fl,96+1/12.0},{fl,112+1/12.0},{fl,128+1/12.0},{cl, 16+1/12.0},{cl, 32+2/12.0},{cl,48+1/12.0},{cl,64+1/12.0},{cl,80+1/12.0},{cl,96+1/12.0},{cl,112+1/12.0},{cl,128+1/12.0},{cr,15+11/12.0},{cr,32.0},{cr,48.0},{cr,63+11/12.0},{cr,79+11/12.0},{cr,95+11/12.0},{cr,112.0},{cr,128+1/12.0},{fr,15+11/12.0},{fr,32.0},{fr,48.0},{fr,63+11/12.0},{fr,79+11/12.0},{fr,95+11/12.0},{fr,112.0},{fr,128+1/12.0}};

#ifdef linear
	//fill in u,v pixel coordinates of points in calibration image, measured through any photo editing/viewing software
	double uv[points][2] = {{791,1699},{1265,1577},{1473,1523},{1591,1489},{1669,1469},{1721,1455},{1762,1446},{1793,1437},{1879,1741},{1939,1599},{1957,1535},{1971,1500},{1978,1479},{1984,1464},{1987,1453},{1991,1443},{2131,1747},{2085,1601},{2065,1535},{2056,1503},{2048,1481},{2045,1465},{2040,1454},{2037,1444},{3297,1773},{2791,1617},{2569,1549},{2446,1514},{2368,1488},{2314,1471},{2274,1457},{2243,1449}};	
#else
	//fill in u,v pixel coordinates of points in calibration image, measured through any photo editing/viewing software
	double uv[points][2] = {{911,1685},{1295,1575},{1488,1523},{1597,1490},{1671,1470},{1722,1456},{1763,1446},{1793,1438},{1879,1741},{1937,1600},{1957,1535},{1971,1501},{1978,1479},{1984,1464},{1988,1453},{1992,1444},{2131,1747},{2085,1601},{2065,1537},{2055,1503},{2049,1481},{2044,1465},{2040,1454},{2037,1444},{3149,1747},{2753,1615},{2558,1550},{2439,1514},{2364,1489},{2312,1472},{2273,1459},{2242,1449}};	
#endif

	//initialize arrays for gamma and theta values for each point.
	double gammas[points];
	double thetas[points];

	//calculate gamma and theta values for each point using derived equations
	for (int i = 0; i < points; ++i) {
		double xp = xy[i][0];
		double yp = xy[i][1];
		double u = uv[i][0];
		double v = uv[i][1];
		double atan_val1 = atan((xp-cx)/(yp-cy));	
		double atan_val2 = atan(cz * sin(atan_val1) / (xp-cx));
		gammas[i] = (atan_val1 - u*(2*alpha_h)/(m-1) + alpha_h)*180/M_PI;
		thetas[i] = (atan_val2 - v*(2*alpha_v)/(n-1) + alpha_v)*180/M_PI;	
	}

	//average gamma and theta values for all points
	double gamma = average(gammas, points);
	double theta = average(thetas, points);

	//print out all gamma and theta values for debugging purposes
	/*
	for (int i = 0; i < points; ++i){
		cout << "gamma: " << gammas[i] << " theta: " << thetas[i] << endl;
	}
	*/

	//print out final gamma and theta value to use in IPM_ReverseMap
	cout << "gamma: " << gamma << endl;
	cout << "theta: " << theta << endl;	

	return 0;
}
