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

struct Point3d  //3d����
{
	Point3d() { x = y = z = 0.0f; }
	float x;
	float y;
	float z;
	bool operator<(const Point3d& p2) const  //�ض����С�ڷ���
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

	Point3d pointCoor;  //3d������
	std::vector<Point3d> vecPtCoor;        //���Գ���������

	float aa, bb, cc;//֮ǰ���и�dd��ee��ff
	while (fgets(buf, BUFSIZE, ifp) != NULL)  /// read info line by line ��עһ�� ����������Խ����Ż� ����waste time! PS:����һ����BUFSIZE=51200��100�� Ȼ�󷵻ض�һ������ ���ǲ����scanf���Բ�
	{
		sscanf_s(buf, "%f %f %f %f %f %f\n", &(pointCoor.x), &(pointCoor.y), &(pointCoor.z), &aa, &bb, &cc);
		if (cnt % 20 == 0)
		{
			vecPtCoor.push_back(pointCoor);//��10��ȡ1��,(������30w�������������2000w��Ҫ����)��������Ը�.GPU�Ļ�����ȫ�������
			Pt_num++;//���Ƽ���������
		}
		cnt++;
	}

	//�����ļ�
	FILE *fp;
	fopen_s(&fp, "MapPoints1.xyz", "w+");
	//	cout<<"�����ĵ���Ŀ��"<<rowNumber<<endl;
	for (int i = 0; i<vecPtCoor.size(); i++)
	{
		//	cout<<vecPtfinl[i].x<<" "<<vecPtfinl[i].y<<" "<<vecPtfinl[i].z<<endl;
			fprintf(fp, "%f %f %f\n", vecPtCoor[i].x, vecPtCoor[i].y, vecPtCoor[i].z);
	}
	fclose(fp);
	fclose(ifp);

	return 0;
}

