//����Ĵ�����һ��ʵ��C++����MYSQL���ݿ�ĺܺõ�����
//�������˽������룬������ɾ��ȳ��ù���
//����VC++6.0���ɣ��Ѿ��ɹ������ˡ�
//��VC++6.0��Ҫ�����һ������׼�������ſ��ԡ�
//(1)Tools->Options->Directories->Include files�����C:\Program Files\MySQL\MySQL Server 6.0\include
//(2)Tools->Options->Directories->Library files�����C:\Program Files\MySQL\MySQL Server 6.0\lib\opt
//��ʵ���ǽ�ͷ�ļ��Ϳ��ļ���������
//�Ҿ����������д�ĺܺã��ٽ���Լ������飬�صؽ��ܸ���ң�

#include"stdmy.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")

//����ִ�У����뵥��ִ�о�ע�͵�
#define STEPBYSTEP

//�ر���һ�����ݽṹ

MYSQL mydata;
int mysqlinit()
{
	//��ʼ�����ݿ�
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
	//��ʼ�����ݽṹ
	if (NULL != mysql_init(&mydata)) {
		cout << "mysql_init() succeed" << endl;
	}
	else {
		cout << "mysql_init() failed" << endl;
		return -1;
	}
	//���������ݿ�֮ǰ�����ö��������ѡ��
	//�������õ�ѡ��ܶ࣬���������ַ����������޷���������
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
	//�������ݿ�
	if (NULL
		!= mysql_real_connect(&mydata, "localhost", "root", "root", "AOI",
		3306, NULL, 0))
		//����ĵ�ַ���û��������룬�˿ڿ��Ը����Լ����ص��������
	{
		cout << "mysql_real_connect() succeed" << endl;
	}
	else {
		cout << "mysql_real_connect() failed" << endl;
		return -1;
	}
	return 0;
}
//sql�ַ���
string sqlstr;

//����һ����
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
	//����в�������
	sqlstr =
		"INSERT INTO user_info(user_name) VALUES('��˾����'),('һ������'),('��������'),('����С��'),('����');";
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
	//ɾ����
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

	//��ȡ���ݣ�����ӡ����
	ofstream myfile_result("result.txt", ios::out);
	sqlstr = "SELECT * from aoi.defectstable ";
	MYSQL_RES *result = NULL;
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "mysql_query() select data succeed" << endl;

		//һ����ȡ�����ݼ�
		result = mysql_store_result(&mydata);
		//ȡ�ò���ӡ����
		int rowcount = mysql_num_rows(result);
		cout << "row count: " << rowcount << endl;

		//ȡ�ò���ӡ���ֶε�����
		unsigned int fieldcount = mysql_num_fields(result);
		MYSQL_FIELD *field = NULL;
		for (unsigned int i = 0; i < fieldcount; i++) {
			field = mysql_fetch_field_direct(result, i);
			cout << field->name << "\t\t";
			myfile_result << field->name << ",";
		}
		cout << endl;

		//��ӡ���� ��д���ı��ļ�
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

			//һ����ȡ�����ݼ�
			result = mysql_store_result(&mydata);
			//ȡ�ò���ӡ���ֶε�����
			unsigned int fieldcount = mysql_num_fields(result);
			MYSQL_FIELD *field = NULL;
			for (unsigned int i = 0; i < fieldcount; i++) {
				field = mysql_fetch_field_direct(result, i);
				cout << field->name << "\t\t";
				myresultfile << field->name << ",";
			}
			myresultfile << endl;
			//��ӡ���� ��д���ı��ļ�
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