//下面的代码是一个实现C++连接MYSQL数据库的很好的例子
//这里用了建表，插入，检索，删表等常用功能
//我用VC++6.0生成，已经成功连接了。
//在VC++6.0中要想把做一下两步准备工作才可以。
//(1)Tools->Options->Directories->Include files中添加C:\Program Files\MySQL\MySQL Server 6.0\include
//(2)Tools->Options->Directories->Library files中添加C:\Program Files\MySQL\MySQL Server 6.0\lib\opt
//其实就是将头文件和库文件包含进来
//我觉得这个例子写的很好，再结合自己的试验，特地介绍给大家！

#include"stdmy.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")

//单步执行，不想单步执行就注释掉
#define STEPBYSTEP

//必备的一个数据结构

MYSQL mydata;
int mysqlinit()
{
	//初始化数据库
	if (0 == mysql_library_init(0, NULL, NULL)) {
		cout << "mysql_library_init() succeed" << endl;
	}
	else {
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
#ifdef STEPBYSTEP
	system("pause");
#endif
	//初始化数据结构
	if (NULL != mysql_init(&mydata)) {
		cout << "mysql_init() succeed" << endl;
	}
	else {
		cout << "mysql_init() failed" << endl;
		return -1;
	}
	//在连接数据库之前，设置额外的连接选项
	//可以设置的选项很多，这里设置字符集，否则无法处理中文
#ifdef STEPBYSTEP
	system("pause");
#endif
	if (0 == mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk")) {
		cout << "mysql_options() succeed" << endl;
	}
	else {
		cout << "mysql_options() failed" << endl;
		return -1;
	}
	cout << "****************************************" << endl;
	//连接数据库
	if (NULL
		!= mysql_real_connect(&mydata, "localhost", "root", "root", "AOI",
		3306, NULL, 0))
		//这里的地址，用户名，密码，端口可以根据自己本地的情况更改
	{
		cout << "mysql_real_connect() succeed" << endl;
	}
	else {
		cout << "mysql_real_connect() failed" << endl;
		return -1;
	}
	return 0;
}
//sql字符串
string sqlstr;

//创建一个表
int createtable()
{
	sqlstr = "CREATE TABLE IF NOT EXISTS userinfo ";
	sqlstr += "(";
	sqlstr +=
		"user_id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY COMMENT 'Unique User ID',";
	sqlstr +=
		"user_name VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'Name Of User',";
	sqlstr +=
		"user_second_sum INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'The Summation Of Using Time'";
	sqlstr += ");";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "mysql_query() create table succeed" << endl;
	}
	else {
		cout << "mysql_query() create table failed" << endl;
		mysql_close(&mydata);
		return -1;
	}
	return 0;
}

int insertintoTable()
{
	//向表中插入数据
	sqlstr =
		"INSERT INTO user_info(user_name) VALUES('公司名称'),('一级部门'),('二级部门'),('开发小组'),('姓名');";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "mysql_query() insert data succeed" << endl;
	}
	else {
		cout << "mysql_query() insert data failed" << endl;
		mysql_close(&mydata);
		return -1;
	}
	return 0;
}

int droptable()
{
	//删除表
	sqlstr = "DROP TABLE user_info";
	return 0;
}
int mysql_query_do(string sqlstr)
{
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "mysql_query()  succeed " <<"by:" <<sqlstr<< endl;
		//mysql_store_result(&mydata);
	}
	else {
		cout << "mysql_query()  failed " << "by:" << sqlstr << endl;
		mysql_close(&mydata);
		return -1;
	}
	return 0;

}
int truncatetable()
{
	sqlstr = "truncate aoi.pcbtable";
	mysql_query_do(sqlstr);
	return 0;
}

int showdata()
{

	//获取数据，并打印出来
	ofstream myfile_result("result.txt", ios::out);
	sqlstr = "SELECT * from aoi.defectstable ";
	MYSQL_RES *result = NULL;
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "mysql_query() select data succeed" << endl;

		//一次性取得数据集
		result = mysql_store_result(&mydata);
		//取得并打印行数
		int rowcount = mysql_num_rows(result);
		cout << "row count: " << rowcount << endl;

		//取得并打印各字段的名称
		unsigned int fieldcount = mysql_num_fields(result);
		MYSQL_FIELD *field = NULL;
		for (unsigned int i = 0; i < fieldcount; i++) {
			field = mysql_fetch_field_direct(result, i);
			cout << field->name << "\t\t";
			myfile_result << field->name << ",";
		}
		cout << endl;

		//打印各行 并写入文本文件
		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(result);
		while (NULL != row) {
			for (int i = 0; i < fieldcount; i++) {
				cout << row[i] << "\t\t";
				myfile_result << row[i] << ",";
			}
			cout << endl;
			myfile_result << endl;
			row = mysql_fetch_row(result);
		}
		myfile_result.close();
	}
	else {
		cout << "mysql_query() select data failed" << endl;
		mysql_close(&mydata);
		return -1;
	}
	mysql_free_result(result);
	return 0;
}

int add(int x, int y)
{	
	return x + y;
}

void File_sql_query(const char * sqlfilename, const char * resultfilename)
 {
	char buffer[256];
	ifstream FileSql(sqlfilename);
	ofstream myresultfile(resultfilename);
	if (!FileSql.is_open())
	{
		cout << "Error opening file"; exit(1);
	}
	if (!myresultfile.is_open())
	{
		cout << "Error opening file"; exit(1);
	}
	while (!FileSql.eof())
	{
		FileSql.getline(buffer, 100);
		//for (int i = 0; i < strlen(buffer); i++)
		//{
	//		mystr += buffer[i];
	//	}
		cout << buffer << endl;

		MYSQL_RES *result = NULL;
		if (0 == mysql_query(&mydata, buffer)) {
			cout << "mysql_query() select data succeed" << endl;

			//一次性取得数据集
			result = mysql_store_result(&mydata);
			//取得并打印各字段的名称
			unsigned int fieldcount = mysql_num_fields(result);
			MYSQL_FIELD *field = NULL;
			for (unsigned int i = 0; i < fieldcount; i++) {
				field = mysql_fetch_field_direct(result, i);
				cout << field->name << "\t\t";
				myresultfile << field->name << ",";
			}
			myresultfile << endl;
			//打印各行 并写入文本文件
			MYSQL_ROW row = NULL;
			row = mysql_fetch_row(result);
			while (NULL != row) {
				for (int i = 0; i < fieldcount; i++) {
					cout << row[i] << "\t\t";
					myresultfile << row[i] << ",";
				}
				cout << endl;
				myresultfile << endl;
				row = mysql_fetch_row(result);
			}
		}
		else {
			cout << "mysql_query() select data failed" << endl;
			mysql_close(&mydata);
		}
		mysql_free_result(result);
	}
	system("pause");

	FileSql.close();
	myresultfile.close();
}
void File_sql_update(const char * filename)
{
	//ofstream myfile("sql.txt", ios::in,0);
	char buffer[256];
	ifstream FileSql(filename);
	if (!FileSql.is_open() )
	{
		cout << "Error opening file"; exit(1);
	}
	while (!FileSql.eof())
	{
		FileSql.getline(buffer, 255);
		mysql_query_do(buffer);
		/*if (0 == mysql_query(&mydata, buffer)) {
			mysql_store_result(&mydata);
		}
		else
		{
			cout << "mysql_query(): " << buffer << "failed"<<endl;
		}*/
	}
	system("pause");
	FileSql.close();
	mysql_close(&mydata);
}

int main() {
	mysqlinit();

	File_sql_update("initdatabase.txt");
	mysql_close(&mydata);
	mysql_server_end();
	system("pause");
	return 0;

}