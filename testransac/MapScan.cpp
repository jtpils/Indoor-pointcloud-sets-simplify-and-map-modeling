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

#define F_PATH "o0.xyz" //文件路径,此处注意\具有转义性，要用双斜杠表示

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


struct Plane
{
	Plane() { a = b = c = d = 0.0f; }
	float a, b, c, d;
};

struct CoorPlane {
	CoorPlane() { num = 0; a = b = c = d = 0.0f; }
	int num;
	float a, b, c, d;
};

struct Map//最终输出的东西
{
	Map() { vecPtfin.clear(); }//要不要初始化
	Plane tested;
	std::vector<Point3d> vecPtfin;
};


int main()
{
	FILE *ifp;
	const int BUFSIZE = 51200;//used to be 512
	char buf[BUFSIZE];
	fopen_s(&ifp, "MapPoints1.xyz", "r");
	int cnt = 0;		//counter
	int Pt_num = 0;

	Point3d pointCoor;  //3d点坐标
	Point3d pointCoortemp;
	Point3d originCoor;

	vector<Point3d>::iterator itor;//删除器

	std::vector<Point3d> vecPtsel;         //均匀采样点坐标
	std::vector<Point3d> vecPtCoor;        //测试抽样点坐标

	std::vector<Point3d> vecPttemp;        //临时内点点坐标集
	std::vector<Point3d> vecPtfinl;        //最终内点点坐标

	Plane planeC;
	std::vector<Plane> vecPlaneCoor;	   //预估平面方程

	CoorPlane planeCo;
	std::vector<CoorPlane> vecBiggestPl;	   //最大的5个平面

	Map m;
	std::vector<Map> vecMap;

	//测试点云集获取，改良版需要进行比例修改
	float aa, bb, cc;//之前还有个dd，ee，ff
	while (fgets(buf, BUFSIZE, ifp) != NULL)  /// read info line by line 标注一下 这个读法可以进行优化 这样waste time! PS:尝试一口气BUFSIZE=51200读100个 然后返回读一个这样 就是不清楚scanf可以不
	{
		sscanf_s(buf, "%f %f %f %f %f %f\n", &(pointCoor.x), &(pointCoor.y), &(pointCoor.z), &aa, &bb, &cc);
		if (cnt % 10 == 0)
		{
			vecPtCoor.push_back(pointCoor);//隔10个取1个,(点云量30w可以这个比例，2000w的要更大)，这里可以改.GPU的话可以全部载入吧
			Pt_num++;//点云集采样数量
		}
		cnt++;
	}

	int circum = 0;
	cnt = Pt_num;
	//大循环起点，十次或者剩余点数量不足10%为终止条件,循环次数可以修改
	for ( ; circum < 10 && vecPtCoor.size() > cnt/10 ; circum++)
	{

		//随机3点生成平面方程（小于10w的点云样本集取1/3数量的线）同样改良版中需要修改比例
		int count = 0;
		int ccount = 0;
		int r1, r2, r3;
 		int rowNumber = vecPtCoor.size();
		float a, b, c, d;

		double disnumber, distance;

		for (; count < 15000;)		//估计平面的数量，这里可以改
		{
			r1 = rand() % (rowNumber + 1);
			r2 = rand() % (rowNumber + 1);
			r3 = rand() % (rowNumber + 1);

			if ((r1 != r2) && (r1 != r3) && (r2 != r3) && r1<rowNumber && r2< rowNumber && r3<rowNumber)
			{
				//算平面方程的参数
				a = (vecPtCoor[r2].y - vecPtCoor[r1].y)*(vecPtCoor[r3].z - vecPtCoor[r1].z) - (vecPtCoor[r2].z - vecPtCoor[r1].z)*(vecPtCoor[r3].y - vecPtCoor[r1].y);
				b = (vecPtCoor[r2].z - vecPtCoor[r1].z)*(vecPtCoor[r3].x - vecPtCoor[r1].x) - (vecPtCoor[r2].x - vecPtCoor[r1].x)*(vecPtCoor[r3].z - vecPtCoor[r1].z);
				c = (vecPtCoor[r2].x - vecPtCoor[r1].x)*(vecPtCoor[r3].y - vecPtCoor[r1].y) - (vecPtCoor[r2].y - vecPtCoor[r1].y)*(vecPtCoor[r3].x - vecPtCoor[r1].x);
				d = 0 - (a*vecPtCoor[r1].x + b*vecPtCoor[r1].y + c*vecPtCoor[r1].z);
				//存入平面
				planeC.a = a; planeC.b = b; planeC.c = c; planeC.d = d;
				vecPlaneCoor.push_back(planeC);

				count++;
			}
		}

		//匹配抽样点集（1000-2000）
		
		//pointCoor.x = 0.0f;
		for (count = 0; count < vecPtCoor.size(); count++)
		{
			if (count % 20 == 0)//标准测试点数量，这里可以改
			{
				pointCoor.x = vecPtCoor[count].x;
				pointCoor.y = vecPtCoor[count].y;
				pointCoor.z = vecPtCoor[count].z;
				vecPtsel.push_back(pointCoor);
			}
		}

		//选前5大的平面,抽样点集中点投影到平面方程的内点数大于5即可，需要一个预选平面集
		count = 0;
		while (count < vecPlaneCoor.size() )
		{
			int inpointNum = 0;//预估内点数量
			a = vecPlaneCoor[count].a;
			b = vecPlaneCoor[count].b;
			c = vecPlaneCoor[count].c;
			d = vecPlaneCoor[count].d;
			disnumber = sqrt(a*a + b*b + c*c);
			for (ccount = 0; ccount < vecPtsel.size(); ccount++)
			{
				distance = fabs(a*vecPtsel[ccount].x + b*vecPtsel[ccount].y + c*vecPtsel[ccount].z + d) / disnumber;
				if (distance <= 0.02)//内点比例，小于1cm，可修改
				{
					inpointNum++;
				}
			}
			if (inpointNum >= 5)//内点数大于等于5？？会不会太小，可修改,revise enabled
			{
				planeCo.num = inpointNum;
				planeCo.a = a;
				planeCo.b = b;
				planeCo.c = c;
				planeCo.d = d;

				vecBiggestPl.push_back(planeCo);//写入
			}
			count++;
		}
		//前五大平面排序
		count = 0;
		ccount = 0;
		for (; count < vecBiggestPl.size(); count++)
		{
			ccount = count + 1;

			for (; ccount < vecBiggestPl.size(); ccount++)
			{
				if (vecBiggestPl[count].num < vecBiggestPl[ccount].num)
				{
					planeCo = vecBiggestPl[ccount];
					//交换
					vecBiggestPl[ccount] = vecBiggestPl[count];
					vecBiggestPl[count] = planeCo;
				}
			}
		}

		count = 0;
		ccount = 0;
		for (; count <= 4; count++)
		{
			//与其他平面的关系估计，是否为同一平面，平行,垂直
			if (vecBiggestPl[count].num != 0)
			{
				for (ccount = count + 1; ccount <= 4; ccount++)
				{
					if (fabs(vecBiggestPl[count].a - vecBiggestPl[ccount].a) <= 0.03 && fabs(vecBiggestPl[count].b - vecBiggestPl[ccount].b) <= 0.03 && fabs(vecBiggestPl[count].c - vecBiggestPl[ccount].c) <= 0.03)//同一平面或平行
					{
						if (fabs(vecBiggestPl[count].d - vecBiggestPl[ccount].d) <= 0.03)//同一平面则跳过后面的平面（即将其内点数设成0）,此处设的是1cm，会不会太小了？
						{
							vecBiggestPl[count].num = 0;
						}
						else//平行平面则用多平面一维聚类法，间隔如何界定，隔1cm还是多少为一个面的范围，10cm？,所有点对当前面进行投影
						{
							;
						}
					}
					else if (fabs(vecBiggestPl[count].a * vecBiggestPl[ccount].a + vecBiggestPl[count].b * vecBiggestPl[ccount].b + vecBiggestPl[count].c * vecBiggestPl[ccount].c) <= 0.03)//垂直
					{

					}
				}
			}
			//平面关系算完了再对面进行计算
			a = vecBiggestPl[count].a;
			b = vecBiggestPl[count].b;
			c = vecBiggestPl[count].c;
			d = vecBiggestPl[count].d;
			disnumber = sqrt(a*a + b*b + c*c);

			planeC.a = a;
			planeC.b = b;
			planeC.c = c;
			planeC.d = d;



			//DLT(直接线性变换)？还是用ransac审核是否为最大平面
			m.tested.a = a;
			m.tested.b = b;
			m.tested.c = c;
			m.tested.d = d;
			//内点删除,保留100左右，存入最终的Map集里,计算平面边界以便画外包络
			for (count = 0; count < vecPtCoor.size();)
			{
				if (vecPtCoor[count].x != 0.0 && vecPtCoor[count].y != 0.0 && vecPtCoor[count].z != 0.0)
				{
					distance = fabs(a*vecPtCoor[count].x + b*vecPtCoor[count].y + c*vecPtCoor[count].z + d) / disnumber;
					if (distance <= 0.02)
					{
						if (count % 50 == 0)
						{
							pointCoor.x = vecPtCoor[count].x;
							pointCoor.y = vecPtCoor[count].y;
							pointCoor.z = vecPtCoor[count].z;
							m.vecPtfin.push_back(pointCoor);//存入最终集合
						}

						vecPtCoor[count].x = vecPtCoor[count].y = vecPtCoor[count].z = 0.0;//某种意义上的删除，这里做的不好，内存都没释放掉
					}
					count++;
				}
			}
			
			vecMap.push_back(m);
			vector<Point3d>().swap(m.vecPtfin);

			for (count = 0; count < vecPtCoor.size(); count++)
			{
				if (vecPtCoor[count].x != 0.0 || vecPtCoor[count].y != 0.0 || vecPtCoor[count].z != 0.0)
				{
					vecPttemp.push_back(vecPtCoor[count]);
				}
			}
			vector<Point3d>().swap(vecPtCoor);//全清
			for (count = 0; count < vecPttemp.size(); count++)
				vecPtCoor.push_back(vecPttemp[count]);
			vector<Point3d>().swap(vecPttemp);
		}
		vector<Point3d>().swap(vecPtsel);//真正意义上的重置
		vector<CoorPlane>().swap(vecBiggestPl);
		vector<Plane>().swap(vecPlaneCoor);
	}

	//导出文件
	FILE *fp;
	FILE *tfp;
	fopen_s(&fp, "Mapdata1.txt", "w+");
	fopen_s(&tfp, "Mapdata1.txt", "w+");
	//	cout<<"抽样的点数目："<<rowNumber<<endl;
	for (int i = 0; i<vecMap.size(); i++)
	{
		//	cout<<vecPtfinl[i].x<<" "<<vecPtfinl[i].y<<" "<<vecPtfinl[i].z<<endl;
		for (int j = 0; j < vecMap[i].vecPtfin.size(); j++)
		{
			fprintf(fp, "%f %f %f\n", vecMap[i].vecPtfin[j].x, vecMap[i].vecPtfin[j].y, vecMap[i].vecPtfin[j].z);
			fprintf(tfp, "%f %f %f\n", vecMap[i].vecPtfin[j].x, vecMap[i].vecPtfin[j].y, vecMap[i].vecPtfin[j].z);
		}
		//fprintf(fp, "%f %f %f %f\n", vecMap[i].tested.a, vecMap[i].tested.b, vecMap[i].tested.c, vecMap[i].tested.d);
	}
	fclose(fp);
	fclose(tfp);
	return 0;
}