//HW2 edge & segment   4/6
#include <opencv2//core//core.hpp>
#include <opencv2//highgui//highgui.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;


void antiThresh(Mat &src, Mat &dst, int Thresh);
void doWatershed();
void myWatershed(Mat src, Mat dist, string name, int thres);

int main()
{
	
	Mat lena, cameraman, myPic, stainedcell_large, stainedcell_small;

	cout << "Watershed result" << endl;
	doWatershed();

	waitKey(0);
	return 0;
}


void antiThresh(Mat &src, Mat &dst, int Thresh)
{
	dst = src.clone();
	for(int y = 0; y < (dst.rows); y++)
	{
		for(int x = 0; x < dst.cols; x++)
		{
			if(dst.at<uchar>(y,x) < Thresh)
				dst.at<uchar>(y,x) = 255;
			else
				dst.at<uchar>(y,x) = 0;
		}
	}
};




void doWatershed()
{
	Mat stainedcell_large, stainedcell_large_gary;

	stainedcell_large = imread("stainedcell_large.jpg", 1);

	cvtColor(stainedcell_large, stainedcell_large_gary, CV_BGR2GRAY);

	myWatershed(stainedcell_large, stainedcell_large_gary, "stainedcell_large", 110);
	
	
	waitKey(0);

}

void myWatershed(Mat src, Mat dist, string name, int thres)
{
	antiThresh(dist, dist, thres);//good
    Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    // Find total markers
    vector<vector<Point> > contours;
    findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//findContours
    // Create the marker image for the watershed algorithm

	/*************************************************************************************************************/
	double area=0.0;
	double length=0.0;
	double compact=0.0;
	double T=1;
	int count=0;
	vector<double> area_mat;
	vector<double> length_mat;
	vector<double> com_mat;
	double area_sum=0.0;
	double area_avg=0.0;
	double area_var=0.0;
	double area_temp=0.0;
	double length_sum=0.0;
	double length_avg=0.0;
	double length_var=0.0;
	double length_temp=0.0;
	double com_sum=0.0;
	double com_avg=0.0;
	double com_var=0.0;
	double com_temp=0.0;
	for(int i=0;i<contours.size();i++)
	{
		//stringstream s1,s2;
		area=contourArea(contours[i],false);
		length=arcLength(contours[i],false);
		if(area==0)
		{compact=0;}
		else
		{
			compact=pow(length,2)/area;}
		if(area<T){count++;}
		area_mat.push_back(area);
		length_mat.push_back(length);
		com_mat.push_back(compact);
		//s1<<i;
		//s2<<area;
		//putText(dist,s1.str(),contours[i][0],FONT_HERSHEY_COMPLEX ,0.3,Scalar(0,255,0));
		cout<<"number  "<<i<<endl;
		cout<<"area = "<<area<<endl;
		cout<<"length = "<<length<<endl;
		cout<<"compactness = "<<compact<<endl;
	}
		for(int j=0;j<contours.size();j++)
		{
			if((area_mat[j]>T)&&(length_mat[j]>T)&&(com_mat[j]>T))
			{
				length_sum=length_sum+length_mat[j];
				area_sum=area_sum+area_mat[j];
				com_sum=com_sum+com_mat[j];
			}
		}
		length_avg=length_sum/(contours.size()-count);
		area_avg=area_sum/(contours.size()-count);
		com_avg=com_sum/(contours.size()-count);
		
		for(int t=0;t<contours.size();t++)
		{
			if((area_mat[t]>T)&&(length_mat[t]>T)&&(com_mat[t]>T))
			{
				length_temp+=pow(length_mat[t]-length_avg,2);
				area_temp+=pow(area_mat[t]-area_avg,2);
				com_temp+=pow(com_mat[t]-com_avg,2);
			}
		}
		length_var=length_temp/(contours.size()-count);
		area_var=area_temp/(contours.size()-count);
		com_var=com_temp/(contours.size()-count);
		//cout<<"count"<<count<<endl;
		cout<<"componant 個數"<<contours.size()<<endl;
		cout<<"面積變異程度"<<(sqrt(area_var)/area_avg)*100<<endl;//得到變異數
		cout<<"周常變異程度"<<((sqrt(length_var))/length_avg)*100<<endl;
		cout<<"形狀變異程度"<<((sqrt(com_var))/com_avg)*100<<endl;
		/**********************************************************************************************************/


    Mat markers = Mat::zeros(dist.size(), CV_32SC1);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++)
        drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), -1);//drawContours
    // Draw the background marker
    circle(markers, Point(5,5), 3, CV_RGB(255,255,255), -1);
    //imshow("Markers", markers*10000);
	imshow("maker*10000", markers*10000);
	//markers = dist;

	// Perform the watershed algorithm
    watershed(src, markers);//watershed
    Mat mark = Mat::zeros(markers.size(), CV_8UC1);
    markers.convertTo(mark, CV_8UC1);
    bitwise_not(mark, mark);
    imshow("markers", markers); // uncomment this if you want to see how the mark
                                  // image looks like at that point
    // Generate random colors
    vector<Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++)
    {
        int b = theRNG().uniform(0, 255);
        int g = theRNG().uniform(0, 255);
        int r = theRNG().uniform(0, 255);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
    // Create the result image
    Mat dst = Mat::zeros(markers.size(), CV_8UC3);
    // Fill labeled objects with random colors
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i,j);
			//if(index!=255)
			//	cout << "index="<< index << "/ti=" <<i << "/tj=" << j << endl;
            if (index > 0 && index <= static_cast<int>(contours.size()))
                dst.at<Vec3b>(i,j) = colors[index-1];
            else
                dst.at<Vec3b>(i,j) = Vec3b(0,0,0);
        }
    }


	for(int i=0;i<contours.size();i++)
	{
		stringstream s1,s2;
		s1<<i;
		s2<<area;
		putText(dst,s1.str(),contours[i][0],FONT_HERSHEY_COMPLEX ,0.3,Scalar(255,0,0));
	}
	
	imshow("final",dst);











	/*
	//label num
	int Labeling[9999] = {0};
	//int LabelNum = 0;
	
	 for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i,j);
			
			char index_s[64];
			sprintf(index_s,"%d",index);
			if(index > 0)
			{
				if (Labeling[index]==0)
				{
					putText(dst,index_s, Point(j,i),FONT_HERSHEY_SIMPLEX  ,  0.25, 0, 1);
					//LabelNum++;
				}
				Labeling[index]++;
			}

			
		
			
        }
    }
	 //show area
	 cout << "*****show area******" <<endl;
	 for(int i=1500;i<1700;i++)
	 {
		 cout << "No." << i << "\tarea=" << Labeling[i] <<endl;
	 }

    // Visualize the final image
    imshow(name, dst);*/
}