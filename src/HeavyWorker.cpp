#include "HeavyWorker.h"



HeavyWorker::HeavyWorker()
{
	for (int d = 0; d < 5 + 1; d++)
	{
		array2D[d][0] = 0;
		array2D[d][1] = 0;


	}
}


HeavyWorker::~HeavyWorker()
{
}
std::vector<std::pair<cv::Mat, float>> HeavyWorker::Stereo(cv::Mat imgLeft, cv::Mat imgRight)
{
	std::vector<std::pair<cv::Mat, float>> potholeImageListWithDepth;
	/*sprintf(filename_L, "image_Left_%02i.bmp", currFrame);
	sprintf(filename_R, "image_Right_%02i.bmp", currFrame);

	imgLeft = cv::imread(filename_L);
	imgRight = cv::imread(filename_R);
*/
	// ************** Rectify the images **********
	cv::Mat img1 = imgLeft;
	cv::Mat rimg1; //, img1;
	cv::Mat rimg2; //, img2;
	cv::Mat img2 = imgRight;

	cv::Mat rmap[2][2];
	cv::Rect validRoi[2];
	MvStereoRectify(rmap, validRoi);

	cv::remap(img1, rimg1, rmap[0][0], rmap[0][1], CV_INTER_LINEAR);
	cv::remap(img2, rimg2, rmap[1][0], rmap[1][1], CV_INTER_LINEAR);

	cv::Rect iroi1(cvRound(validRoi[0].x), cvRound(validRoi[0].y),
		cvRound(validRoi[0].width), cvRound(validRoi[0].height));
	cv::Rect iroi2(cvRound(validRoi[1].x), cvRound(validRoi[1].y),
		cvRound(validRoi[1].width), cvRound(validRoi[1].height));

	cv::Mat roi1(rimg1, iroi1);
	cv::Mat roi2(rimg2, iroi2);

	imgLeft = roi1;
	imgRight = roi2;

	cv::Size size(1024, 768);

	resize(imgLeft, imgLeft, size);//resize image
	resize(imgRight, imgRight, size);//resize image

	/*imwrite("imgLeft_Image.jpg", imgLeft);
	imwrite("imgRight_Image.jpg", imgRight);*/
	this->imgLeft = imgLeft;
	this->imgRight = imgRight;
	// ************** End Rectify the images ******

	// ******** PNet Recognition ********
	int num_pothole=Pothole_Recognition();
	//array2D.erase(std::remove(array2D.begin(), array2D.end(), 0), array2D.end());

	//int num_pothole = (sizeof(array2D) / sizeof(*array2D));

	// count the number od detected potholes: 
	int pot_count = 0;
	for (int d = 0; d < num_pothole + 1; d++)
	{
		if (array2D[d][0] != NULL && array2D[d][1] != NULL)
		{
			pot_count = pot_count + 1;
		}

	}

	// ******** End PNet Recognition ********

	// call the localisation methods if the pot_count>0
	if (pot_count > 0)
	{

		// ******** Calculate the Disparity Map ********

		cvtColor(imgLeft, greyMat_L, CV_BGR2GRAY);
		cvtColor(imgRight, greyMat_R, CV_BGR2GRAY);
		//-- And create the image in which we will save our disparities
		cv::Mat imgDisparity16S = cv::Mat(greyMat_L.rows, greyMat_L.cols, CV_16S);
		cv::Mat imgDisparity8U = cv::Mat(greyMat_L.rows, greyMat_L.cols, CV_8UC1);
		cv::Mat colour_patch;
		if (imgLeft.empty() || imgRight.empty())
		{
			std::cout << " --(!) Error reading images " << std::endl; 
		}

		//-- 2. Call the constructor for StereoBM
		//int ndisparities = 16 * 5;   /**< Range of disparity */
		int ndisparities = 16 * 1;   /**< Range of disparity */
									 //int SADWindowSize = 5; /**< Size of the block window. Must be odd */

		SADWindowSize = 5;
		numberOfDisparities = 224;
		preFilterCap = 31;
		minDisparity = 0;
		uniquenessRatio = 15;
		speckleWindowSize = 0;
		speckleRange = 0;
		disp12MaxDiff = -1;
		fullDP = false;
		P1 = 200;
		P2 = 800;
		cv::Ptr<cv::StereoSGBM> sbm = cv::StereoSGBM::create(minDisparity, numberOfDisparities, SADWindowSize, P1, P2, disp12MaxDiff, preFilterCap, uniquenessRatio, speckleWindowSize, speckleRange, cv::StereoSGBM::MODE_SGBM);

		//-- 3. Calculate the disparity image
		sbm->compute(greyMat_L, greyMat_R, imgDisparity16S); // check the size of greyMat_L and greyMat_R images.

															 //-- Check its extreme values
		double minVal; double maxVal;

		minMaxLoc(imgDisparity16S, &minVal, &maxVal);

		printf("Min disp: %f Max value: %f \n", minVal, maxVal);

		//-- 4. Display it as a CV_8UC1 image
		imgDisparity16S.convertTo(imgDisparity8U, CV_8UC1, 255 / (maxVal - minVal));

		cv::namedWindow(windowDisparity, cv::WINDOW_NORMAL);
		/*	imshow(windowDisparity, imgDisparity8U);
			cv::waitKey(1);*/

			// ******** End Calculate the Disparity Map ********

			// ******** Reconstruct the 3-D Scene ********

		imgDisparity8U_Patch = cv::Mat(imgDisparity8U, patch);

		for (int d = 0; d < num_pothole + 1; d++)
		{
			if (array2D[d][0] != NULL && array2D[d][1] != NULL)
			{
				patch = cv::Rect(array2D[d][0], array2D[d][1], 299, 299);
				imgDisparity8U_Patch = cv::Mat(imgDisparity8U, patch);
				colour_patch = cv::Mat(imgLeft, patch);
				float depth=Depth_Estimation(imgDisparity8U_Patch);
				//float depth = 1;
				std::pair<cv::Mat, float> tmpPair(colour_patch, depth);
				potholeImageListWithDepth.push_back(tmpPair);
			}

		}

		// ******** End Reconstruct the 3-D Scene ****

		//reset array2D
		for (int d = 0; d < num_pothole + 1; d++)
		{
			array2D[d][0] = 0;
			array2D[d][1] = 0;


		}
		
	}
	////////////*****************
	//////////pImageData = pBuffer_R->getPtr();
	//////////smart_ptr<BITMAPINFOHEADER> pInf = pBuffer_R->getBitmapInfoHeader();
	//////////iImageSize = pInf->biWidth * pInf->biHeight * pInf->biBitCount / 8;
	//////////for (int i = 0; i < iImageSize; i++)
	//////////{
	//////////	//pImageData[i] = 255 - pImageData[i];
	//////////	pImageData[i] = pImageData[i];
	//////////}
	//////////cv::Mat RGB_img_R(pInf->biHeight, pInf->biWidth, CV_8UC3, pImageData);

	//////////RGB_img_R = RGB_img_R.clone();
	//////////cv::flip(RGB_img_R, RGB_img_R, 0);

	//////////imshow("RIGHT", RGB_img_R);                   // Show our image inside it.
	//////////cv::waitKey(1);
	////////////****************
	//////////pImageData_L = pBuffer_LR[0]->getPtr();
	//////////pInf_L = pBuffer_L->getBitmapInfoHeader();
	//////////iImageSize = pInf_L->biWidth * pInf_L->biHeight * pInf_L->biBitCount / 8;
	//////////for (int i = 0; i < iImageSize; i++)
	//////////{
	//////////	//pImageData[i] = 255 - pImageData[i];
	//////////	pImageData_L[i] = pImageData_L[i];
	//////////}
	//////////cv::Mat RGB_img_L(pInf_L->biHeight, pInf_L->biWidth, CV_8UC3, pImageData_L);

	//////////RGB_img_L = RGB_img_L.clone();
	//////////cv::flip(RGB_img_L, RGB_img_L, 0);
	//////////imshow("LEFT", RGB_img_L);                   // Show our image inside it.
	//////////cv::waitKey(1);

	//return true;
	return potholeImageListWithDepth;
}

void HeavyWorker::MvStereoRectify(cv::Mat _rmap[2][2], cv::Rect vRoi[2]) {

	cv::FileStorage fs("intrinsics_Exp27032018.yml", cv::FileStorage::READ); // intrinsicsC1l
																			 /*cv::Size imageSize(1280, 960); *///1200, 548 - 752, 480 - 1280, 400 - 1280, 900
	cv::Size imageSize(1024, 768);
	cv::Mat R1, R2, P1, P2; // output 

	cv::Mat cameraMatrix[2], c[2], distCoeffs[2], R, T;
	c[0] = cv::Mat::eye(3, 3, CV_64F);
	c[1] = cv::Mat::eye(3, 3, CV_64F);
	distCoeffs[0] = cv::Mat::eye(1, 5, CV_64F);
	distCoeffs[1] = cv::Mat::eye(1, 5, CV_64F);

	//
	fs["M1"] >> cameraMatrix[0]; //M0; //
	fs["D1"] >> distCoeffs[0];   //D0; //
	fs["M2"] >> cameraMatrix[1]; //M1; //
	fs["D2"] >> distCoeffs[1]; //D1; //
	fs["R"] >> R;
	fs["T"] >> T;

	try
	{
		cv::stereoRectify(cameraMatrix[0], distCoeffs[0],
			cameraMatrix[1], distCoeffs[1],
			imageSize, R, T, R1, R2, P1, P2, Q,
			cv::CALIB_ZERO_DISPARITY, 0, cv::Size(0, 0), &vRoi[0], &vRoi[1]); // cv::CALIB_ZERO_DISPARITY

																			  //cv::stereoRectify(cameraMatrix[0], distCoeffs[0],
																			  //			  cameraMatrix[1], distCoeffs[1],
																			  //			  imageSize, R, T, R1, R2, P1, P2, Q,
																			  //			  0,0, cv::Size(0,0), &vRoi[0], &vRoi[1]); // cv::CALIB_ZERO_DISPARITY
	}
	catch (int e)
	{
		std::cout << "An exception occurred. Exception Nr. " << e << '\n';
	}

	//cv::FileStorage fse("extrinsics_cs01112017.yml", CV_STORAGE_WRITE);
	//if (fse.isOpened())
	//{
	//	fse << "R" << R << "T" << T << "R1" << R1 << "R2" << R2 << "P1" << P1 << "P2" << P2 << "Q" << Q;
	//	fse.release();
	//}
	//else
	//	std::cout << "Error: can not save the intrinsic parameters\n";

	//Precompute maps for cv::remap()
	cv::initUndistortRectifyMap(cameraMatrix[0], distCoeffs[0], R1, P1, imageSize, CV_32FC1, _rmap[0][0], _rmap[0][1]); // CV_16SC2 CV_32FC1
	cv::initUndistortRectifyMap(cameraMatrix[1], distCoeffs[1], R2, P2, imageSize, CV_32FC1, _rmap[1][0], _rmap[1][1]);

}

/// DCNN Code
void HeavyWorker::getMaxClass(const cv::Mat &probBlob, int *classId, double *classProb)
{
	cv::Mat probMat = probBlob.reshape(1, 1); //reshape the blob to 1x1000 matrix

	cv::Point classNumber;
	minMaxLoc(probMat, NULL, classProb, NULL, &classNumber);
	*classId = classNumber.x;
}


std::vector<cv::String> HeavyWorker::readClassNames(const char *filename = "label_alaa_project.txt")
{
	std::vector<cv::String> classNames;
	std::ifstream fp(filename);
	if (!fp.is_open())
	{
		std::cerr << "File with classes labels not found: " << filename << std::endl;
		exit(-1);
	}
	std::string name;
	while (!fp.eof())
	{
		std::getline(fp, name);
		if (name.length())
			classNames.push_back(name.substr(name.find(' ') + 1));
	}
	fp.close();
	return classNames;
}

/// Pothole Recognition takes left and right rectified images and return an array of the patch with pothole
int HeavyWorker::Pothole_Recognition()
{

	int c1 = 0;
	std::vector<cv::String> classNames = readClassNames();

	//imgLeft = imread("left_657.png");
	//imgLeft = imread("image_4065.jpg");
	int M = imgLeft.size().height; //768; // pInf->biHeight;
	int N = imgLeft.size().width; //1024; // pInf->biWidth;

								  //imshow("LEFT", imgLeft);
								  //cv::waitKey(1);
	try
	{
		/*int ccc = 1;*/
		for (int jj = 9; jj < M - 299; jj += 150)
		{
			for (int ii = 62; ii < N - 299; ii += 150)
			{


				patch = cv::Rect(ii, jj, 299, 299);


				inputBlob = cv::dnn::blobFromImage(cv::Mat(imgLeft, patch), 1, cv::Size(227, 227),
					cv::Scalar(104, 117, 123));   //Convert Mat to batch of images

				CV_TRACE_REGION("forward");
				net.setInput(inputBlob, "data");        //set the network input
				prob = net.forward("prob");

				getMaxClass(prob, &classId, &classProb);//find the best class

				class_name = classNames.at(classId);

				//imshow("Patch", imgLeft);
				//cv::waitKey(1);

				if (class_name == "potholes")
				{
					int ccc = 1;
					cv::Mat roi = cv::Mat(imgLeft, patch);
					std::stringstream ss;
					ss << ccc;
					imageName = "image_" + ss.str();
					FullPath = "./patches/" + imageName; // save to the current directory
					FullPath = FullPath + ".jpg";
					cv::imwrite(FullPath, roi);
					ccc += 1;

					//fill the array
					array2D[c1][0] = ii;
					array2D[c1][1] = jj;

					c1 += 1;
				}


			}
		}
	}
	catch (int e)
	{
		std::cout << "An exception occurred. Exception Nr. " << e << '\n';
	}
	return c1;

}

//******* QSF Algorithms **********
float HeavyWorker::Depth_Estimation(cv::Mat imgDisparity8U)
{
	cv::Mat xyz(imgDisparity8U.size(), CV_32FC3);
	reprojectImageTo3D(imgDisparity8U, xyz, Q, false, CV_32F);
	/// Define RoI based on CNN Detection and Constract the 3D point for only RoI
	//cout << "Q= " << Q << "\n";
	const int  N = imgDisparity8U.cols * imgDisparity8U.rows;
	int idx = 0;
	int idx2 = 0;
	typedef Eigen::Matrix<float, Eigen::Dynamic, 3> Matrix3f;
	Eigen::MatrixXf points3D_XYZ(N, 3);

	cv::Vec3f coordinates;
	std::ofstream myfile;
	//cv::Mat pointXYZ(xyz.size(), xyz.type(), cv::Scalar::all(10));
	myfile.open("xyz.txt");

	// convert depth map into 3D points
	for (int rr = 0; rr < imgDisparity8U.rows; ++rr) {

		for (int cc = 0; cc < imgDisparity8U.cols; ++cc) {
			coordinates = xyz.at<cv::Vec3f>(rr, cc); // x,y,z
			coordinates = coordinates / 10;

			/*	if (!isinf(coordinates[0]) & !isinf(coordinates[1]) & !isinf(coordinates[2]) & coordinates[0] > (float)-400 & coordinates[0] < (float)200 & coordinates[1] >(float) - 100 & coordinates[1] <(float)100 & coordinates[2] > (float) 400 & coordinates[2] < (float)900)
			{*/
			if (!isinf(coordinates[0]) & !isinf(coordinates[1]) & !isinf(coordinates[2]))
			{
				points3D_XYZ(idx2, 0) = coordinates[0]; //x
				points3D_XYZ(idx2, 1) = coordinates[1]; //y
				points3D_XYZ(idx2, 2) = coordinates[2]; //z
				std::string temp = std::to_string(coordinates[0]) + "," + std::to_string(coordinates[1]) + "," + std::to_string(coordinates[2]) + "\n";
				myfile << temp;

				idx = idx + 1;
			}
			idx2 = idx2 + 1;
		}
		// accumulate the array
	}
	myfile.close();

	// ******** Fit the 3D Points of RoI into quadratic surface ********

	Eigen::MatrixXf points3D(idx, 6);
	Eigen::MatrixXf points_Y(idx, 1);

	// generate the N * 6 matrix from the 3D point cloud
	cv::Mat_<float> Point3D_Mat = cv::Mat_<float>::zeros(idx, 6);
	cv::Mat_<float> points_Y_Mat = cv::Mat_<float>::zeros(idx, 1);
	idx2 = 0;
	for (int rr = 0; rr < imgDisparity8U.rows; ++rr) {

		for (int cc = 0; cc < imgDisparity8U.cols; ++cc) {
			coordinates = xyz.at<cv::Vec3f>(rr, cc); // x,y,z
			coordinates = coordinates / 10;

			/*	if (!isinf(coordinates[0]) & !isinf(coordinates[1]) & !isinf(coordinates[2]) & coordinates[0] > (float)-400 & coordinates[0] < (float)200 & coordinates[1] >(float) - 100 & coordinates[1] <(float)100 & coordinates[2] > (float) 400 & coordinates[2] < (float)900)
			{*/
			if (!isinf(coordinates[0]) & !isinf(coordinates[1]) & !isinf(coordinates[2]))
			{
				Point3D_Mat(idx2, 0) = 1.0;
				Point3D_Mat(idx2, 1) = coordinates[0];
				Point3D_Mat(idx2, 2) = coordinates[2];
				Point3D_Mat(idx2, 3) = coordinates[0] * coordinates[0];
				Point3D_Mat(idx2, 4) = coordinates[0] * coordinates[2];
				Point3D_Mat(idx2, 5) = coordinates[2] * coordinates[2];

				points_Y_Mat(idx2, 0) = coordinates[1];
				idx2 = idx2 + 1;

			}


		}
		// accumulate the array
	}

	cv2eigen(Point3D_Mat, points3D);

	Eigen::MatrixXf thinQ(Eigen::MatrixXf::Identity(idx, 6)), thinR(Eigen::MatrixXf::Identity(6, 6));

	Eigen::HouseholderQR<Eigen::MatrixXf> qr(points3D);
	thinQ.setIdentity();
	thinQ = qr.householderQ() * thinQ;
	thinR = qr.matrixQR().topLeftCorner(6, 6).template triangularView<Eigen::Upper>();

	// Convert thinR & P into OpenCV Matrix

	cv::Mat_<float> thinR_Mat = cv::Mat_<float>::zeros(6, 6);
	cv::Mat_<float> thinQ_Mat = cv::Mat_<float>::zeros(idx, 6);
	cv::Mat_<float> P1_Mat = cv::Mat_<float>::zeros(6, 1);
	cv::Mat_<float> P_Mat = cv::Mat_<float>::zeros(6, 1);
	cv::Mat_<float> Residual = cv::Mat_<float>::zeros(idx, 1);

	eigen2cv(thinR, thinR_Mat);
	eigen2cv(thinQ, thinQ_Mat);

	cv::Mat P1_Mat_Transpose = cv::Mat(thinQ_Mat.cols, thinQ_Mat.rows, CV_32F);
	cv::transpose(thinQ_Mat, P1_Mat_Transpose);

	P1_Mat = P1_Mat_Transpose * points_Y_Mat;

	//P = cv::solve(thinR,(thinQ.inverse() * points_Y));
	/*Residual = points_Y - thinQ*P;*/
	cv::solve(thinR_Mat, P1_Mat, P_Mat);


	Residual = points_Y_Mat - (Point3D_Mat * P_Mat);

	float s = 0;
	float std = 0;
	float mean = 0;
	int cc = 0;
	//mean
	for (int xx = 0; xx < idx; xx++)
	{
		if (Residual(xx, 0) > 0.1 && Residual(xx, 0) < 5)
		{
			s = s + Residual(xx, 0);
			cc = cc + 1;
		}
	}
	mean = s / cc;
	//std
	cc = 0;
	for (int xx = 0; xx < idx; xx++)
	{
		if (Residual(xx, 0) > 0.1) // && Residual(xx, 0) < 7)
		{
			std = std + ((mean - Residual(xx, 0)) * (mean - Residual(xx, 0)));
			cc = cc + 1;
		}
	}
	std::cout << "Pothole Depth = " << mean << "  " << "cm" << std::endl;
	return mean;
	//cout << "std Residual = " << std << endl << endl;
	//cout << "mean + 2*std Residual = " << mean + 2 * std << endl << endl;
	//meanStdDev(InputArray src, OutputArray mean, OutputArray stddev, InputArray mask=noArray())


	// ******** End Fit the 3D Points of RoI into quadratic surface ****


}