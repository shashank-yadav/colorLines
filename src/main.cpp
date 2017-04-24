#include "colorLines.h"

using namespace std;
using namespace cv;

int main(int argc, char const *argv[])
{

	Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	
    colorLines c;
    
    c.init(image,10);

	// imwrite("output.png",m);

	// std::cout<<labels;



	return 0;
}