#include <winsock.h>
#include <iostream>
#include <string>
#include <mysql.h>
#include<fstream>
#include <stdlib.h>


using namespace std;


//执行sql语句
int mysql_query_do(string sqlstr);
//从文件读取sql语句，并执行,更新操作
void File_sql_update(const char * filename);
//将sql查询结果写入到文件中
//void File_sql_query(const char * sqlfilename, const char * resultfilename);