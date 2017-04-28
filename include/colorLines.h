#include "point.h"
#include <vector>
#include <math.h>
#include <string>
#include <opencv2/opencv.hpp>

#define PI 3.14159265
#define NB_K 5

using namespace cv;
using namespace std;

typedef std::vector<point> colorLine;

class colorLines
{

public:
	
	std::vector<colorLine> lines;
	int radius;
	
	colorLines();
	~colorLines();

	void init(cv::Mat img , const int r);
	
};