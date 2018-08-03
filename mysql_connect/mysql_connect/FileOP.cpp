#include"stdmy.h"

//ofstream         //文件写操作 内存写入存储设备   
//ifstream         //文件读操作，存储设备读区到内存中  
//fstream          //读写操作，对打开的文件可进行读写操作




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