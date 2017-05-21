#include "colorLines.h"
#include "tinydir.h"

using namespace std;
using namespace cv;



cv::Mat get_image_of_interest(cv::Mat &image, cv::Mat &image_segnet_temp){
	cv::Mat image_segnet = cv::Mat(image.size(), image_segnet_temp.type());

	for (int i = 0; i < image_segnet.rows; ++i){
		for (int j = 0; j < image_segnet.cols; ++j){
			int x = (j*image_segnet_temp.cols)/image_segnet.cols;
			int y = (i*image_segnet_temp.rows)/image_segnet.rows;
			// std::cout << x << " " << y << " " << image_segnet_temp.rows << " " << image_segnet_temp.cols << std::endl;
			if(image_segnet_temp.at<Vec3b>(y,x)[0] == 4 && i < 1115){
				image_segnet.at<Vec3b>(i,j)[0] = 1;
				image_segnet.at<Vec3b>(i,j)[1] = 1;
				image_segnet.at<Vec3b>(i,j)[2] = 1;
			}
			else{
				image_segnet.at<Vec3b>(i,j)[0] = 0;
				image_segnet.at<Vec3b>(i,j)[1] = 0;
				image_segnet.at<Vec3b>(i,j)[2] = 0;
			}
		}
	}

	image = image.mul(image_segnet);

	return image;
}


int main(int argc, char const *argv[])
{

	tinydir_dir dir;
	int i;
	tinydir_open_sorted(&dir, argv[1]);

	std::vector<std::string> all_images;

	for (i = 0; i < dir.n_files; i++)
	{
		tinydir_file file;
		tinydir_readfile_n(&dir, &file, i);

		// printf("%s", file.name);
		if (!file.is_dir)
		{
			all_images.push_back(file.name);
		}
		// printf("\n");
	}

	tinydir_close(&dir);


	tinydir_open_sorted(&dir, argv[2]);

	std::vector<std::string> all_images_segnet;

	for (i = 0; i < dir.n_files; i++)
	{
		tinydir_file file;
		tinydir_readfile_n(&dir, &file, i);

		// printf("%s", file.name);
		if (!file.is_dir)
		{
			all_images_segnet.push_back(file.name);
		}
		// printf("\n");
	}

	tinydir_close(&dir);



	Mat image;
    image = imread(argv[1] + all_images[0], CV_LOAD_IMAGE_COLOR);

    // image = cv::Mat(image.rows*10, image.cols,image.type());
    image = cv::Mat(image.rows*all_images.size(), image.cols,image.type());

    // std::cout << image.rows << " " << image.cols << std::endl;

    // for (int i = 0; i < 10; ++i)
    for (int i = 0; i < all_images.size(); ++i)
    {

    	cv::Mat temp = cv::imread(argv[1] + all_images[i], CV_LOAD_IMAGE_COLOR);

    	cv::Mat temp_segnet = cv::imread(argv[2] + all_images_segnet[i], CV_LOAD_IMAGE_COLOR);

    	temp = get_image_of_interest(temp, temp_segnet);

    	temp.copyTo(image.rowRange(i*temp.rows, (i+1)*temp.rows).colRange(0,temp.cols));
    }

    // cv::imwrite("test.png",image);

    std::cout << "step one finished " << std::endl;



	// Mat image;
	// image = imread(argv[1], CV_LOAD_IMAGE_COLOR);

	// Mat image_segnet_temp = imread(argv[2], CV_LOAD_IMAGE_COLOR);

	// cv::Mat image_segnet = cv::Mat(image.size(), image_segnet_temp.type());

	// // cv::resize(image_segnet,image_segnet_temp,image.size(),INTER_NEAREST);

	// // std::cout << image_segnet.cols << " " << image_segnet.rows << std::endl;

	// for (int i = 0; i < image_segnet.rows; ++i){
	// 	for (int j = 0; j < image_segnet.cols; ++j){
	// 		int x = (j*image_segnet_temp.cols)/image_segnet.cols;
	// 		int y = (i*image_segnet_temp.rows)/image_segnet.rows;
	// 		// std::cout << x << " " << y << " " << image_segnet_temp.rows << " " << image_segnet_temp.cols << std::endl;
	// 		if(image_segnet_temp.at<Vec3b>(y,x)[0] == 4 && i < 1115){
	// 			image_segnet.at<Vec3b>(i,j)[0] = 1;
	// 			image_segnet.at<Vec3b>(i,j)[1] = 1;
	// 			image_segnet.at<Vec3b>(i,j)[2] = 1;
	// 		}
	// 		else{
	// 			image_segnet.at<Vec3b>(i,j)[0] = 0;
	// 			image_segnet.at<Vec3b>(i,j)[1] = 0;
	// 			image_segnet.at<Vec3b>(i,j)[2] = 0;
	// 		}
	// 	}
	// }

	// image = image.mul(image_segnet);

	// cv::imwrite("test.png",image);
	
	colorLines c;
    
	c.init(image,10);

	// imwrite("output.png",m);

	// std::cout<<labels;



	return 0;
}