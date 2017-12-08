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

#define F_PATH "o0.xyz" //�ļ�·��,�˴�ע��\����ת���ԣ�Ҫ��˫б�ܱ�ʾ

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

struct Map//��������Ķ���
{
	Map() { vecPtfin.clear(); }//Ҫ��Ҫ��ʼ��
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

	Point3d pointCoor;  //3d������
	Point3d pointCoortemp;
	Point3d originCoor;

	vector<Point3d>::iterator itor;//ɾ����

	std::vector<Point3d> vecPtsel;         //���Ȳ���������
	std::vector<Point3d> vecPtCoor;        //���Գ���������

	std::vector<Point3d> vecPttemp;        //��ʱ�ڵ�����꼯
	std::vector<Point3d> vecPtfinl;        //�����ڵ������

	Plane planeC;
	std::vector<Plane> vecPlaneCoor;	   //Ԥ��ƽ�淽��

	CoorPlane planeCo;
	std::vector<CoorPlane> vecBiggestPl;	   //����5��ƽ��

	Map m;
	std::vector<Map> vecMap;

	//���Ե��Ƽ���ȡ����������Ҫ���б����޸�
	float aa, bb, cc;//֮ǰ���и�dd��ee��ff
	while (fgets(buf, BUFSIZE, ifp) != NULL)  /// read info line by line ��עһ�� ����������Խ����Ż� ����waste time! PS:����һ����BUFSIZE=51200��100�� Ȼ�󷵻ض�һ������ ���ǲ����scanf���Բ�
	{
		sscanf_s(buf, "%f %f %f %f %f %f\n", &(pointCoor.x), &(pointCoor.y), &(pointCoor.z), &aa, &bb, &cc);
		if (cnt % 10 == 0)
		{
			vecPtCoor.push_back(pointCoor);//��10��ȡ1��,(������30w�������������2000w��Ҫ����)��������Ը�.GPU�Ļ�����ȫ�������
			Pt_num++;//���Ƽ���������
		}
		cnt++;
	}

	int circum = 0;
	cnt = Pt_num;
	//��ѭ����㣬ʮ�λ���ʣ�����������10%Ϊ��ֹ����,ѭ�����������޸�
	for ( ; circum < 10 && vecPtCoor.size() > cnt/10 ; circum++)
	{

		//���3������ƽ�淽�̣�С��10w�ĵ���������ȡ1/3�������ߣ�ͬ������������Ҫ�޸ı���
		int count = 0;
		int ccount = 0;
		int r1, r2, r3;
 		int rowNumber = vecPtCoor.size();
		float a, b, c, d;

		double disnumber, distance;

		for (; count < 15000;)		//����ƽ���������������Ը�
		{
			r1 = rand() % (rowNumber + 1);
			r2 = rand() % (rowNumber + 1);
			r3 = rand() % (rowNumber + 1);

			if ((r1 != r2) && (r1 != r3) && (r2 != r3) && r1<rowNumber && r2< rowNumber && r3<rowNumber)
			{
				//��ƽ�淽�̵Ĳ���
				a = (vecPtCoor[r2].y - vecPtCoor[r1].y)*(vecPtCoor[r3].z - vecPtCoor[r1].z) - (vecPtCoor[r2].z - vecPtCoor[r1].z)*(vecPtCoor[r3].y - vecPtCoor[r1].y);
				b = (vecPtCoor[r2].z - vecPtCoor[r1].z)*(vecPtCoor[r3].x - vecPtCoor[r1].x) - (vecPtCoor[r2].x - vecPtCoor[r1].x)*(vecPtCoor[r3].z - vecPtCoor[r1].z);
				c = (vecPtCoor[r2].x - vecPtCoor[r1].x)*(vecPtCoor[r3].y - vecPtCoor[r1].y) - (vecPtCoor[r2].y - vecPtCoor[r1].y)*(vecPtCoor[r3].x - vecPtCoor[r1].x);
				d = 0 - (a*vecPtCoor[r1].x + b*vecPtCoor[r1].y + c*vecPtCoor[r1].z);
				//����ƽ��
				planeC.a = a; planeC.b = b; planeC.c = c; planeC.d = d;
				vecPlaneCoor.push_back(planeC);

				count++;
			}
		}

		//ƥ������㼯��1000-2000��
		
		//pointCoor.x = 0.0f;
		for (count = 0; count < vecPtCoor.size(); count++)
		{
			if (count % 20 == 0)//��׼���Ե�������������Ը�
			{
				pointCoor.x = vecPtCoor[count].x;
				pointCoor.y = vecPtCoor[count].y;
				pointCoor.z = vecPtCoor[count].z;
				vecPtsel.push_back(pointCoor);
			}
		}

		//ѡǰ5���ƽ��,�����㼯�е�ͶӰ��ƽ�淽�̵��ڵ�������5���ɣ���Ҫһ��Ԥѡƽ�漯
		count = 0;
		while (count < vecPlaneCoor.size() )
		{
			int inpointNum = 0;//Ԥ���ڵ�����
			a = vecPlaneCoor[count].a;
			b = vecPlaneCoor[count].b;
			c = vecPlaneCoor[count].c;
			d = vecPlaneCoor[count].d;
			disnumber = sqrt(a*a + b*b + c*c);
			for (ccount = 0; ccount < vecPtsel.size(); ccount++)
			{
				distance = fabs(a*vecPtsel[ccount].x + b*vecPtsel[ccount].y + c*vecPtsel[ccount].z + d) / disnumber;
				if (distance <= 0.02)//�ڵ������С��1cm�����޸�
				{
					inpointNum++;
				}
			}
			if (inpointNum >= 5)//�ڵ������ڵ���5�����᲻��̫С�����޸�,revise enabled
			{
				planeCo.num = inpointNum;
				planeCo.a = a;
				planeCo.b = b;
				planeCo.c = c;
				planeCo.d = d;

				vecBiggestPl.push_back(planeCo);//д��
			}
			count++;
		}
		//ǰ���ƽ������
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
					//����
					vecBiggestPl[ccount] = vecBiggestPl[count];
					vecBiggestPl[count] = planeCo;
				}
			}
		}

		count = 0;
		ccount = 0;
		for (; count <= 4; count++)
		{
			//������ƽ��Ĺ�ϵ���ƣ��Ƿ�Ϊͬһƽ�棬ƽ��,��ֱ
			if (vecBiggestPl[count].num != 0)
			{
				for (ccount = count + 1; ccount <= 4; ccount++)
				{
					if (fabs(vecBiggestPl[count].a - vecBiggestPl[ccount].a) <= 0.03 && fabs(vecBiggestPl[count].b - vecBiggestPl[ccount].b) <= 0.03 && fabs(vecBiggestPl[count].c - vecBiggestPl[ccount].c) <= 0.03)//ͬһƽ���ƽ��
					{
						if (fabs(vecBiggestPl[count].d - vecBiggestPl[ccount].d) <= 0.03)//ͬһƽ�������������ƽ�棨�������ڵ������0��,�˴������1cm���᲻��̫С�ˣ�
						{
							vecBiggestPl[count].num = 0;
						}
						else//ƽ��ƽ�����ö�ƽ��һά���෨�������ν綨����1cm���Ƕ���Ϊһ����ķ�Χ��10cm��,���е�Ե�ǰ�����ͶӰ
						{
							;
						}
					}
					else if (fabs(vecBiggestPl[count].a * vecBiggestPl[ccount].a + vecBiggestPl[count].b * vecBiggestPl[ccount].b + vecBiggestPl[count].c * vecBiggestPl[ccount].c) <= 0.03)//��ֱ
					{

					}
				}
			}
			//ƽ���ϵ�������ٶ�����м���
			a = vecBiggestPl[count].a;
			b = vecBiggestPl[count].b;
			c = vecBiggestPl[count].c;
			d = vecBiggestPl[count].d;
			disnumber = sqrt(a*a + b*b + c*c);

			planeC.a = a;
			planeC.b = b;
			planeC.c = c;
			planeC.d = d;



			//DLT(ֱ�����Ա任)��������ransac����Ƿ�Ϊ���ƽ��
			m.tested.a = a;
			m.tested.b = b;
			m.tested.c = c;
			m.tested.d = d;
			//�ڵ�ɾ��,����100���ң��������յ�Map����,����ƽ��߽��Ա㻭�����
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
							m.vecPtfin.push_back(pointCoor);//�������ռ���
						}

						vecPtCoor[count].x = vecPtCoor[count].y = vecPtCoor[count].z = 0.0;//ĳ�������ϵ�ɾ�����������Ĳ��ã��ڴ涼û�ͷŵ�
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
			vector<Point3d>().swap(vecPtCoor);//ȫ��
			for (count = 0; count < vecPttemp.size(); count++)
				vecPtCoor.push_back(vecPttemp[count]);
			vector<Point3d>().swap(vecPttemp);
		}
		vector<Point3d>().swap(vecPtsel);//���������ϵ�����
		vector<CoorPlane>().swap(vecBiggestPl);
		vector<Plane>().swap(vecPlaneCoor);
	}

	//�����ļ�
	FILE *fp;
	FILE *tfp;
	fopen_s(&fp, "Mapdata1.txt", "w+");
	fopen_s(&tfp, "Mapdata1.txt", "w+");
	//	cout<<"�����ĵ���Ŀ��"<<rowNumber<<endl;
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