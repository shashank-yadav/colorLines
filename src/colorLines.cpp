#include "colorLines.h"

colorLines::colorLines(){}
colorLines::~colorLines(){}


cv::Mat low_pass(cv::Mat src)
{
	// define the kernel
	float Kernel[3][3] = {
						{1/9.0, 1/9.0, 1/9.0},
						{1/9.0, 1/9.0, 1/9.0},
						{1/9.0, 1/9.0, 1/9.0}
					   }; 
	
	cv::Mat dst = src.clone();
	float sum;
	for(int y = 0; y < src.rows; y++)
		for(int x = 0; x < src.cols; x++)
			dst.at<float>(y,x) = 0.0;
	//convolution operation
	for(int y = 1; y < src.rows - 1; y++){
		for(int x = 1; x < src.cols - 1; x++){
			sum = 0.0;
			for(int k = -1; k <= 1;k++){
				for(int j = -1; j <=1; j++){
					sum = sum + Kernel[j+1][k+1]*src.at<float>(y - j, x - k);
				}
			}
			dst.at<float>(y,x) = sum;
		}
	}
	return dst;
}


std::vector<cv::Point2f> local_maxima(cv::Mat src)
{
	const int border = 2;
	std::vector<cv::Point2f> ret;
	for(int y = border; y < src.rows - border; y++){
		for(int x = border; x < src.cols - border; x++){
			bool maxima = true;
			for(int k = -1; k <= 1;k++){
				for(int j = -1; j <=1; j++){
					if(src.at<float>(y - j, x - k) >= src.at<float>(y, x)){
						maxima = false;
						break;
					}
				}
			}
			if(maxima){
				ret.push_back(cv::Point2f((float)x,(float)y));
			}
		}
	}
	return ret;
}

cv::Mat affiliation(cv::Mat src, std::vector<cv::Point2f> maximas)
{
	cv::Mat ret = src.clone();
	for(int y = 0; y < src.rows; y++){
		for(int x = 0; x < src.cols; x++){
			float mindist = ((float)x-maximas[0].x)*((float)x-maximas[0].x) + ((float)y-maximas[0].y)*((float)y-maximas[0].y);
			ret.at<float>(y,x) = 0.0;
			for (int i = 1; i < maximas.size(); ++i)
			{
				float temp = ((float)x-maximas[i].x)*((float)x-maximas[i].x) + ((float)y-maximas[i].y)*((float)y-maximas[i].y);
				if(temp < mindist){
					mindist = temp;
				ret.at<float>(y,x) = float(i);
				}
			}
		}
	}
	return ret;
}

std::vector<cv::Point3f> calc_gaussians(cv::Mat hist, cv::Mat belongs, std::vector<cv::Point2f> maximas)
{
	std::vector<cv::Point3f> ret;
	std::vector<float> count;
	for (int i = 0; i < maximas.size(); ++i){
		ret.push_back(cv::Point3f(0.0,0.0,0.0));
		count.push_back(0.0);
	}

	for (int y = 0; y < hist.rows; ++y){
		for (int x = 0; x < hist.cols; ++x){
			int where = (int)belongs.at<float>(y,x);
			ret[where].x += hist.at<float>(y,x)*(float)x;
			ret[where].y += hist.at<float>(y,x)*(float)y;
			ret[where].z += hist.at<float>(y,x)*(float)x*(float)x + hist.at<float>(y,x)*(float)y*(float)y;
			count[where] += 1.0;
		}
	}

	for (int i = 0; i < maximas.size(); ++i)
	{
		ret[i].x /= count[i];
		ret[i].y /= count[i];
		ret[i].z /= count[i];
		ret[i].z -= (ret[i].x*ret[i].x + ret[i].y*ret[i].y);
	}
	return ret;
}


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
		cv::Mat dst = low_pass(hist);
		std::vector<cv::Point2f> maximas = local_maxima(dst);
		assert(maximas.size() > 0);
		cv::Mat belongs = affiliation(hist, maximas);
		std::vector<cv::Point3f> gaussians = calc_gaussians(hist,belongs,maximas);
		
		//gaussians in std::vector<cv::Point3f> gaussians
		// each Point3f represents a gaussian
		// x is mean in x and
		// y is mean in y and
		// z is variance

	}

	// Mat ucharMat, ucharMatScaled;
	// hist.convertTo(ucharMat, CV_8UC1);

	// scale values from 0..1 to 0..255
	// hist.convertTo(ucharMatScaled, CV_8UC1, 255, 0); 
	// return ucharMatScaled;
}
