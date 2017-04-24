#include "colorLines.h"

colorLines::colorLines(){}
colorLines::~colorLines(){}

void colorLines::init(cv::Mat img, const int r)
{
	const int num_bins = (450/r) + 1;
	std::vector<Point> imagePts[num_bins];


	// Mat hist = Mat::zeros(360,360,CV_32FC1);
	Point3d origin =  Point3d(0,0,0);
	Point3d eps =  Point3d(0.01,0.01,0.01);

	for (int i = 0; i < img.cols; ++i){
		for (int j = 0; j < img.rows; ++j){
			
			Vec3d pixel1= img.at<Vec3b>(j,i);
			Point3d pixel= (Point3d)pixel1;
			
			// cout<<i<<"   "<<j<<"  "<<pixel<<endl;
			Point3d direction_vec = pixel - origin;

			double magnitude = norm(direction_vec);
			direction_vec.x /= magnitude;
			direction_vec.y /= magnitude;
			direction_vec.z /= magnitude;

			magnitude = norm(pixel);
			
			if ( magnitude > 0 ){	
				int bin_id = magnitude/r + 1; 
				Point3d proj_pt = bin_id*direction_vec + eps;
	
				double theta = atan(proj_pt.y/proj_pt.x);
	
				double temp = sqrt(proj_pt.x*proj_pt.x + proj_pt.y*proj_pt.y);
				double phi = atan(  temp/proj_pt.z );
	
				int x = (int)(theta*180/PI);
				int y = (int)(phi*180/PI);
	
				// hist.at<float>(y,x) += 1;
				imagePts[bin_id].push_back( Point(x,y) );
				// cout<<hist.at<float>(y,x)<<endl;
			}
		}
	}

	for (int i = 0; i < num_bins; ++i)
	{
		cout<<imagePts[i].size()<<endl;
		Mat hist = Mat::zeros(360,360,CV_32FC1);

		for (int j = 0; j < imagePts[i].size(); ++j){
			hist.at<float>(imagePts[i][j].y , imagePts[i][j].x ) += 1;		
		}

		//Call your gaussian fitting function on hist

	}

	// Mat ucharMat, ucharMatScaled;
	// hist.convertTo(ucharMat, CV_8UC1);

	// scale values from 0..1 to 0..255
	// hist.convertTo(ucharMatScaled, CV_8UC1, 255, 0); 
	// return ucharMatScaled;
}
