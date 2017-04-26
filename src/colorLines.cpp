#include "colorLines.h"

colorLines::colorLines(){}
colorLines::~colorLines(){}


cv::Mat low_pass(cv::Mat src)
{
	Point anchor = Point( -1, -1 );
  	double delta = 0;
  	int ddepth = -1;
	int kernel_size = 3;
	Mat kernel = Mat::ones( kernel_size, kernel_size, CV_32F )/ (float)(kernel_size*kernel_size);
	Mat dst;
    /// Apply filter
	filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
	// bilateralFilter( src, dst, 15, 40, 40 );
	return dst;
}


std::vector<cv::Point2f> local_maxima(cv::Mat src)
{
	const int border = 2;
	std::vector<cv::Point2f> ret;

	int nbx[8] = {-1 , -1, -1, 0 ,0 , 1 , 1, 1}; 
	int nby[8] = {-1 , 0 , 1 , 1 ,-1, -1, 0, 1}; 
	
	for(int y = border; y < src.rows - border; y++){
		for(int x = border; x < src.cols - border; x++){
			bool maxima = true;
			for (int k = 0; k < 8; ++k){
				if ( src.at<float>(y+nby[k], x+nbx[k]) >= src.at<float>(y, x) ){
					maxima = false;
					break;
				}
			}
			// for(int k = -1; k <= 1;k++){
			// 	for(int j = -1; j <=1; j++){
			// 		if(src.at<float>(y - j, x - k) >= src.at<float>(y, x)){
			// 			maxima = false;
			// 			break;
			// 		}
			// 	}
			// }
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
			count[where] += hist.at<float>(y,x);
			// if(hist.at<float>(y,x) != 0.0)
			// 	std::cout << where << " " << hist.at<float>(y,x) << " " << ret[where].x << " " << ret[where].y << " " << count[where] << std::endl;
		}
	}

	for (int i = 0; i < maximas.size(); ++i)
	{
		if(count[i] > 0.0)
		{
			ret[i].x /= count[i];
			ret[i].y /= count[i];
			ret[i].z /= count[i];
			ret[i].z -= (ret[i].x*ret[i].x + ret[i].y*ret[i].y);
		}
	}
	// for (int j = 0; j < ret.size(); ++j){
	// 	std::cout << j << " " << (int)ret[j].y << " " << ret[j].y << " " << (int)ret[j].x << " " << ret[j].x << std::endl;
	// }
	return ret;
}

// #include <stdlib.h>
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
		// imwrite("hist"+to_string(i)+".png",hist);
		cv::Mat dst = low_pass(hist);
		Mat ucharMatScaled;
		dst.convertTo(ucharMatScaled, CV_8UC1, 255, 0); 
		imwrite("dstScaled"+to_string(i)+".png",ucharMatScaled);
		// imwrite("dst"+to_string(i)+".png",dst);

		std::vector<cv::Point2f> maximas = local_maxima(dst);

		Mat temp = Mat::zeros(360,360,CV_8UC1);
		for (int j = 0; j < maximas.size(); ++j){
			temp.at<uchar>((int)maximas[j].y , (int)maximas[j].x) = 255;
		}
		imwrite("temp"+to_string(i)+".png",temp);
		cout<<"Here "<< maximas.size() <<endl;
		// assert(maximas.size() > 0);
		if(maximas.size() > 0)
		{
			cv::Mat belongs = affiliation(hist, maximas);
			std::vector<cv::Point3f> gaussians = calc_gaussians(hist,belongs,maximas);
			
			Mat result = Mat::zeros(360,360,CV_8UC1);
			for (int j = 0; j < gaussians.size(); ++j){
				// std::cout << i << " " << (int)gaussians[j].y << " " << gaussians[j].y << " " << (int)gaussians[j].x << " " << gaussians[j].x << std::endl;
				result.at<uchar>((int)gaussians[j].y , (int)gaussians[j].x) = (uchar)((int)gaussians[j].z);
			}

			// imwrite("img"+to_string(i)+".png",result);
			// std::cout << belongs << std::endl;
			// belongs.convertTo(ucharMatScaled, CV_8UC1, 255, 0); 
			result.convertTo(ucharMatScaled, CV_8UC1, 255, 0); 
			imwrite("resultScaled"+to_string(i)+".png",ucharMatScaled);

		}


		//gaussians in std::vector<cv::Point3f> gaussians
		// each Point3f represents a gaussian
		// x is mean in x and
		// y is mean in y and
		// z is variance

	}

}
