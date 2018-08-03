#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <afxtempl.h>
#include <afxmt.h>
#include "mysql/mysql.h"
#include <windows.h>
#include <WinSock2.h>
#include <queue>
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib, "mysql_api/libmysql.lib ")

//mysql参考文档
//http://www.yesky.com/imagesnew/software/mysql/manual_Clients.html
/*
*本文件是提供一些经常使用接口
*时间：2014.07.31
*作者：吴建华
*邮箱:564154394@qq.com
*/


/*
*modify:
*2016.11.07
*吴建华
* 修改事项：
*     1、mysql获取记录集函数不支持多线程所以增加了锁解决了记录集被修改的bug
*	  2、增加了mysql连接池类 并希望大量插入数据库的时候使用mysql事务处理方式
*/

/*
*	MYSQL 字段类参照表  
*	整理者：吴建华
*/
/************************************************************************
	类型值									类型描述
	MYSQL_TYPE_TINY							TINYINT字段
	MYSQL_TYPE_SHORT						SMALLINT字段
	MYSQL_TYPE_LONG							INTEGER或INT字段
	MYSQL_TYPE_INT24						MEDIUMINT字段
	MYSQL_TYPE_LONGLONG						BIGINT字段
	MYSQL_TYPE_DECIMAL						DECIMAL或NUMERIC字段
	MYSQL_TYPE_NEWDECIMAL					精度数学DECIMAL或NUMERIC
	MYSQL_TYPE_FLOAT						FLOAT字段
	MYSQL_TYPE_DOUBLE						DOUBLE或REAL字段
	MYSQL_TYPE_BIT							BIT字段
	MYSQL_TYPE_TIMESTAMP					TIMESTAMP字段
	MYSQL_TYPE_DATE							DATE字段
	MYSQL_TYPE_TIME							TIME字段
	MYSQL_TYPE_DATETIME						DATETIME字段
	MYSQL_TYPE_YEAR							YEAR字段
	MYSQL_TYPE_STRING						CHAR字段
	MYSQL_TYPE_VAR_STRING					VARCHAR字段
	MYSQL_TYPE_BLOB							BLOB或TEXT字段（使用max_length来确定最大长度）
	MYSQL_TYPE_SET							SET字段
	MYSQL_TYPE_ENUM							ENUM字段
	MYSQL_TYPE_GEOMETRY						Spatial字段
	MYSQL_TYPE_NULL							NULL-type字段
	MYSQL_TYPE_CHAR							不再重视，用MYSQL_TYPE_TINY取代		                                                        
************************************************************************/


/*
*用于变参类型 如：%d %s两种类型
*
*/
#define DATA_TYPE_INT				4	//整型
#define DATA_TYPE_CHARPTR			1	//char*类型		
#define DATA_TYPE_INVALID			0	//无效类型

/*
*select接口区分字符类型和cstring类型
*/
enum e_strType{e_char=0, e_cstring};

/*
*CTools是提供一些常用的接口类
*时间：2014.07.31
*作者：吴建华
*/


namespace _TOOLS_
{
// 	typedef struct tag_MYSQL
// 	{
// 		tag_MYSQL()
// 		{
// 			initdata();
// 		}
// 		tag_MYSQL(const tag_MYSQL &mysqlinfo)
// 		{
// 			_pResult = mysqlinfo._pResult;
// 			strLastSQL = mysqlinfo.strLastSQL;
// 		}
// 
// 		void initdata()
// 		{
// 			_pResult = NULL;
// 			strLastSQL = "";
// 		}
// 
// 		MYSQL_RES				*_pResult;;	//记录集指针
// 		CString					strLastSQL;	//sql语句
// 	}MYSQL_INFO,*PMYSQL_INFO;

	static CMutex g_mtxmysql;//mysql操作对象的互斥对象
	static CMutex g_mtxMysqlPool;//mysql连接池的互斥对象

	class CTools
	{
	public:
		MYSQL		Mysql;	//mysql对象
		~CTools()
		{
			mysql_close( &Mysql );	
		};

		CTools()
		{
			//_pMysql = NULL;
			m_pResult = NULL;
			m_bOpen = false;
		};
		/*
		*数据库初始化操作
		*
		*参数：
		*	pIP:数据库IP地址
		*	pUser：数据库用户名
		*	pPwd：数据库密码
		*	pDatabase：数据库名称
		*
		*返回值：
		*	返回false表示初始化失败，true表示初始化成功
		*
		*/
		bool initmysql(const char* pIP, const char* pUser, const char* pPwd, const char* pDatabase, unsigned short nPort = 3306 );

		/*
		*执行数据库查询功能（带_s的标识支持CSting类型，不带的支持char*类型）
		*参数：
		*	pSQL：SQL语句
		*	...:变参返回查询到的数据
		*返回值：
		*	返回false表示数据库已经获取完毕，true表示数据还没有获取完毕
		*/
		bool select_sql_beack(__in const char *pSQL, __out ...);
		bool select_sql_beack_s(__in const char *pSQL, __out ...);
		/*
		*修改/插入/删除功能
		*参数：
		*	pSQL：SQL语句
		*返回值：
		*	返回false表示执行失败，true表示执行成功
		*
		*/
		bool update_sql(__in const char *pSQL);
		bool insert_sql(__in const char *pSQL);
		bool delete_sql(__in const char *pSQL);
		/*
		*执行查询SQL语句获取查询的数据
		*
		*参数：
		*	pSQL：sql语句
		*	ap：变参，是指输出查询到的数据
		*
		*返回值：
		*	返回false表示不是无效字符，非true表示是无效字符
		*/
		//打断上一次执行(意思就是不支持嵌套，嵌套可以使用上面的Select)
		bool SelectBeack(__in const char *pSQL, e_strType, __out va_list ap);
		/*
		*清理记录集指针数据
		*
		*/

		/*
		*获取 MYSQL连接状态
		*
		*返回值：
		*		返回true 表示没有断开  返回false 表示mysql连接已经断开
		*
		*/
		bool GetMysqlConnectStatus()
		{
			if (m_bOpen)
			{
				if( mysql_ping(&Mysql)  == 0)//处于连接状态中
				{
					return true;
				}
			}
			return m_bOpen;
		}
		/*
		*清理查询后有多余的数据 最好执行sql语句后获取到所有需要数据后调用次接口清理
		*/
		void ClearResult()
		{
			if ( m_pResult!= NULL )
			{
				mysql_free_result(m_pResult);
				m_pResult = NULL;
				m_strOldSql = "";
			}
			
			//m_Lastmysql.initdata();
		}

		/*
		*关闭mysql连接
		*/
		void CloseMysql(){
			//if ( _pMysql )
			{
				mysql_close( &Mysql );
				//_pMysql = NULL;
			}	
		}
	
		//是否自动提交
		void AutoCommitSQL(BOOL bAuto = TRUE);

	private:
		bool execute(const char* pSQL);
	private:
		//MYSQL_INFO m_Lastmysql;
		CString m_strOldSql;	//旧的SQL语句
		MYSQL_RES *m_pResult;//记录集
		bool m_bOpen;


	//公共接口
	public:
		static CString GetBinPath();
		static WCHAR* ToWChar(char *str);
	};


/************************************************************************/
/*							MYSQL 连接池                                */
/************************************************************************/
	class mysql_connection_pool
	{

	public:
		~mysql_connection_pool();
		//单例模式
		static mysql_connection_pool *GetInstance();
		
		//初始化连接
		size_t InitConnection(int nInitialSize=1);
		//设置mysql信息
		void SetMysqlInfo(const char* pIP,
			const char* pUser,
			const char* pPwd,
			const char* pDatabase,
			unsigned short nPort /* = 3306 */);
		//获取msyql操作对象
		CTools *GetConnection();
		//回收连接对象
		void ReleaseConnection(CTools * pObject);
		//销毁连接池
		void DestoryConnPool();
	protected:
		mysql_connection_pool();
	private:
		char m_szIP[128];
		char m_szUser[128];
		char m_szPwd[128];
		char m_szDatabase[128];
		short m_snPort;
		std::queue<CTools *> the_queue;
		CMutex the_mutex;
	};
}

