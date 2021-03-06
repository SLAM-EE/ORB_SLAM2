/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/


#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include<iomanip>
#include<unistd.h>

#include<opencv2/core/core.hpp>
#include<opencv2/imgcodecs/legacy/constants_c.h>
#include "opencv2/opencv.hpp"

#include"System.h"

using namespace std;


static volatile bool keep_running = true;

   static void* userInput_thread(void*)
   {
       while(keep_running) {
           if (std::cin.get() == 'q')
           {
               //! desired user input 'q' received
               keep_running = false;
           }
       }
   }
//void LoadImages(const string &strSequence, vector<string> &vstrImageFilenames, vector<double> &vTimestamps);

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        cerr << endl << "Usage: ./mono_real_time path_to_vocabulary path_to_settings" << endl;
        return 1;
    }
	
	pthread_t tId;
    (void) pthread_create(&tId, 0, userInput_thread, 0);
      
	//count the number of image frames processed in real time
	//int frame_count = 0;
	
    // Retrieve paths to images
    //vector<string> vstrImageFilenames;
    //vector<double> vTimestamps;
    //LoadImages(string(argv[3]), vstrImageFilenames, vTimestamps);

    //int nImages = vstrImageFilenames.size();
	
    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::MONOCULAR,true);

    // Vector for tracking time statistics
    //vector<float> vTimesTrack;
    //vTimesTrack.resize(nImages);

    cout << endl << "-------" << endl;
    cout << "Start processing real time video input ..." << endl;
    //cout << "Images in the sequence: " << nImages << endl << endl;
	
	//open webcam video input
	cv::VideoCapture cap(0); 
    if(!cap.isOpened())  
        return -1;
        
    // Main loop
    cv::Mat im;
    while(keep_running)
    {
        // Read image from file
        //im = cv::imread(vstrImageFilenames[ni],CV_LOAD_IMAGE_UNCHANGED);
        //double tframe = vTimestamps[ni];
		
		//Read image from webcam
		cap >> im;
		double tframe = cap.get(cv::CAP_PROP_POS_MSEC) / 1000;;

        if(im.empty())
        {
            cerr << endl << "Failed to load image at " << endl;
            return 1;
        }
		
		//increment the count by one when a new image frame is loaded
		//frame_count++;
		
#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t1 = std::chrono::monotonic_clock::now();
#endif

        // Pass the image to the SLAM system
        SLAM.TrackMonocular(im,tframe);

#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t2 = std::chrono::monotonic_clock::now();
#endif
		
        double ttrack= std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();
		cout << "tracking time: " << ttrack << endl;
        //vTimesTrack[ni]=ttrack;

        // Wait to load the next frame
        /*
        double T=0;
        if(ni<nImages-1)
            T = vTimestamps[ni+1]-tframe;
        else if(ni>0)
            T = tframe-vTimestamps[ni-1];

        if(ttrack<T)
            usleep((T-ttrack)*1e6);
        */
        
       	// Save camera trajectory
       	/*
        if((frame_count % 20) == 0)
        {
        	SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");    
        }
        */
    }

    // Stop all threads
    SLAM.Shutdown();

    // Tracking time statistics
    /*
    sort(vTimesTrack.begin(),vTimesTrack.end());
    float totaltime = 0;
    for(int ni=0; ni<nImages; ni++)
    {
        totaltime+=vTimesTrack[ni];
    }
    cout << "-------" << endl << endl;
    cout << "median tracking time: " << vTimesTrack[nImages/2] << endl;
    cout << "mean tracking time: " << totaltime/nImages << endl;
	*/
    // Save camera trajectory
    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");    
	(void) pthread_join(tId, NULL);
    return 0;
}

/*
void LoadImages(const string &strPathToSequence, vector<string> &vstrImageFilenames, vector<double> &vTimestamps)
{
    ifstream fTimes;
    string strPathTimeFile = strPathToSequence + "/times.txt";
    fTimes.open(strPathTimeFile.c_str());
    while(!fTimes.eof())
    {
        string s;
        getline(fTimes,s);
        if(!s.empty())
        {
            stringstream ss;
            ss << s;
            double t;
            ss >> t;
            vTimestamps.push_back(t);
        }
    }

    string strPrefixLeft = strPathToSequence + "/images/";

    const int nTimes = vTimestamps.size();
    vstrImageFilenames.resize(nTimes);

    for(int i=0; i<nTimes; i++)
    {
        stringstream ss;
        ss << setfill('0') << setw(6) << i;
        vstrImageFilenames[i] = strPrefixLeft + ss.str() + ".png";
    }
}
*/
