#include <winsock.h>
#include <iostream>
#include <string>
#include <mysql.h>
#include<fstream>
#include <stdlib.h>


using namespace std;


//ִ��sql���
int mysql_query_do(string sqlstr);
//���ļ���ȡsql��䣬��ִ��,���²���
void File_sql_update(const char * filename);
//��sql��ѯ���д�뵽�ļ���
//void File_sql_query(const char * sqlfilename, const char * resultfilename);