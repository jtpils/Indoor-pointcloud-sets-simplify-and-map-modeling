#pragma warning( disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <math.h>

using namespace std;

struct Point3d  //3d点云
{
	Point3d() { x = y = z = 0.0f; }
	float x;
	float y;
	float z;
	bool operator<(const Point3d& p2) const  //重定义的小于符号
	{
		if (x == p2.x)
		{
			if (y == p2.y)
			{
				return (z < p2.z);
			}
			else return (y < p2.y);
		}
		else return (x < p2.x);
	}
};

int mainn() {
	FILE *ifp;
	const int BUFSIZE = 51200;//used to be 512
	char buf[BUFSIZE];
	fopen_s(&ifp, "MapPoints.txt", "r");
	int cnt = 0;		//counter
	int Pt_num = 0;

	Point3d pointCoor;  //3d点坐标
	std::vector<Point3d> vecPtCoor;        //测试抽样点坐标

	float aa, bb, cc;//之前还有个dd，ee，ff
	while (fgets(buf, BUFSIZE, ifp) != NULL)  /// read info line by line 标注一下 这个读法可以进行优化 这样waste time! PS:尝试一口气BUFSIZE=51200读100个 然后返回读一个这样 就是不清楚scanf可以不
	{
		sscanf_s(buf, "%f %f %f %f %f %f\n", &(pointCoor.x), &(pointCoor.y), &(pointCoor.z), &aa, &bb, &cc);
		if (cnt % 20 == 0)
		{
			vecPtCoor.push_back(pointCoor);//隔10个取1个,(点云量30w可以这个比例，2000w的要更大)，这里可以改.GPU的话可以全部载入吧
			Pt_num++;//点云集采样数量
		}
		cnt++;
	}

	//导出文件
	FILE *fp;
	fopen_s(&fp, "MapPoints1.xyz", "w+");
	//	cout<<"抽样的点数目："<<rowNumber<<endl;
	for (int i = 0; i<vecPtCoor.size(); i++)
	{
		//	cout<<vecPtfinl[i].x<<" "<<vecPtfinl[i].y<<" "<<vecPtfinl[i].z<<endl;
			fprintf(fp, "%f %f %f\n", vecPtCoor[i].x, vecPtCoor[i].y, vecPtCoor[i].z);
	}
	fclose(fp);
	fclose(ifp);

	return 0;
}

