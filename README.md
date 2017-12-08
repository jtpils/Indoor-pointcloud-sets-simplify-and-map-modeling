# Indoor-pointcloud-sets-simplify-and-map-modeling
Based on C++,and may use GPU coding in the future


This project is mainly use Visual Studio 2015,and check the output by using Meshlab

Thanks to my mentor first,he offers me a great help about my project.

I'm still working on it.After collecting a indoor pointcloud set with Kinect or other sensors,I mainly simplify the set,
cause there are too much unuseful data.After this,we just need to calculate how many big planes in the set and their equations.I keep hundreds of points for each plane.

o0 and o1 are pointcloud sets of some walls of the buildings.The plane calculation is mainly use the RANSAC,I used to try the efficient RANSAC(which can evaluate the shape of each part of the set),however,it was unsuitable for my work,pre-calculation can not provide great help to a simple map set(mainly formed by a number of planes).

There are two problems now.
One is the outline of the plane,I am adding it to my program now and using OpenCV can make this solved.
The other is the speed,maybe I made too many loops for plane detecting and I am adjusting the ratio of sampling.After that if possible,I will using CUDA to accelerate my program.
