#include"stdmy.h"

//ofstream         //�ļ�д���� �ڴ�д��洢�豸   
//ifstream         //�ļ����������洢�豸�������ڴ���  
//fstream          //��д�������Դ򿪵��ļ��ɽ��ж�д����




void File_write(const char * filename)
{
	ofstream myfile_result("../result/result.txt", ios::out, 0);
	myfile_result << "this is a test for write into the file" << endl;
	myfile_result.close();
}
void exp1()
{
	char my[256];
	string quearstr = "";
	ifstream FileSql("query.txt");
	ofstream myresultfile("result.txt");
	if (!FileSql.is_open())
	{
		cout << "query.txt opened failed" << endl;  exit(1);
	}
	if (!myresultfile.is_open())
	{
		cout << "query.txt opened failed" << endl;  exit(1);
	}

	while (!FileSql.eof())
	{
		FileSql.getline(my, 100);
		cout << my << endl;
	}

	myresultfile << "test" << endl;
	//File_sql_query("query.txt","result.txt");


	FileSql.close();
	myresultfile.close();
}