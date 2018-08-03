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

//mysql�ο��ĵ�
//http://www.yesky.com/imagesnew/software/mysql/manual_Clients.html
/*
*���ļ����ṩһЩ����ʹ�ýӿ�
*ʱ�䣺2014.07.31
*���ߣ��⽨��
*����:564154394@qq.com
*/


/*
*modify:
*2016.11.07
*�⽨��
* �޸����
*     1��mysql��ȡ��¼��������֧�ֶ��߳�����������������˼�¼�����޸ĵ�bug
*	  2��������mysql���ӳ��� ��ϣ�������������ݿ��ʱ��ʹ��mysql������ʽ
*/

/*
*	MYSQL �ֶ�����ձ�  
*	�����ߣ��⽨��
*/
/************************************************************************
	����ֵ									��������
	MYSQL_TYPE_TINY							TINYINT�ֶ�
	MYSQL_TYPE_SHORT						SMALLINT�ֶ�
	MYSQL_TYPE_LONG							INTEGER��INT�ֶ�
	MYSQL_TYPE_INT24						MEDIUMINT�ֶ�
	MYSQL_TYPE_LONGLONG						BIGINT�ֶ�
	MYSQL_TYPE_DECIMAL						DECIMAL��NUMERIC�ֶ�
	MYSQL_TYPE_NEWDECIMAL					������ѧDECIMAL��NUMERIC
	MYSQL_TYPE_FLOAT						FLOAT�ֶ�
	MYSQL_TYPE_DOUBLE						DOUBLE��REAL�ֶ�
	MYSQL_TYPE_BIT							BIT�ֶ�
	MYSQL_TYPE_TIMESTAMP					TIMESTAMP�ֶ�
	MYSQL_TYPE_DATE							DATE�ֶ�
	MYSQL_TYPE_TIME							TIME�ֶ�
	MYSQL_TYPE_DATETIME						DATETIME�ֶ�
	MYSQL_TYPE_YEAR							YEAR�ֶ�
	MYSQL_TYPE_STRING						CHAR�ֶ�
	MYSQL_TYPE_VAR_STRING					VARCHAR�ֶ�
	MYSQL_TYPE_BLOB							BLOB��TEXT�ֶΣ�ʹ��max_length��ȷ����󳤶ȣ�
	MYSQL_TYPE_SET							SET�ֶ�
	MYSQL_TYPE_ENUM							ENUM�ֶ�
	MYSQL_TYPE_GEOMETRY						Spatial�ֶ�
	MYSQL_TYPE_NULL							NULL-type�ֶ�
	MYSQL_TYPE_CHAR							�������ӣ���MYSQL_TYPE_TINYȡ��		                                                        
************************************************************************/


/*
*���ڱ������ �磺%d %s��������
*
*/
#define DATA_TYPE_INT				4	//����
#define DATA_TYPE_CHARPTR			1	//char*����		
#define DATA_TYPE_INVALID			0	//��Ч����

/*
*select�ӿ������ַ����ͺ�cstring����
*/
enum e_strType{e_char=0, e_cstring};

/*
*CTools���ṩһЩ���õĽӿ���
*ʱ�䣺2014.07.31
*���ߣ��⽨��
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
// 		MYSQL_RES				*_pResult;;	//��¼��ָ��
// 		CString					strLastSQL;	//sql���
// 	}MYSQL_INFO,*PMYSQL_INFO;

	static CMutex g_mtxmysql;//mysql��������Ļ������
	static CMutex g_mtxMysqlPool;//mysql���ӳصĻ������

	class CTools
	{
	public:
		MYSQL		Mysql;	//mysql����
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
		*���ݿ��ʼ������
		*
		*������
		*	pIP:���ݿ�IP��ַ
		*	pUser�����ݿ��û���
		*	pPwd�����ݿ�����
		*	pDatabase�����ݿ�����
		*
		*����ֵ��
		*	����false��ʾ��ʼ��ʧ�ܣ�true��ʾ��ʼ���ɹ�
		*
		*/
		bool initmysql(const char* pIP, const char* pUser, const char* pPwd, const char* pDatabase, unsigned short nPort = 3306 );

		/*
		*ִ�����ݿ��ѯ���ܣ���_s�ı�ʶ֧��CSting���ͣ�������֧��char*���ͣ�
		*������
		*	pSQL��SQL���
		*	...:��η��ز�ѯ��������
		*����ֵ��
		*	����false��ʾ���ݿ��Ѿ���ȡ��ϣ�true��ʾ���ݻ�û�л�ȡ���
		*/
		bool select_sql_beack(__in const char *pSQL, __out ...);
		bool select_sql_beack_s(__in const char *pSQL, __out ...);
		/*
		*�޸�/����/ɾ������
		*������
		*	pSQL��SQL���
		*����ֵ��
		*	����false��ʾִ��ʧ�ܣ�true��ʾִ�гɹ�
		*
		*/
		bool update_sql(__in const char *pSQL);
		bool insert_sql(__in const char *pSQL);
		bool delete_sql(__in const char *pSQL);
		/*
		*ִ�в�ѯSQL����ȡ��ѯ������
		*
		*������
		*	pSQL��sql���
		*	ap����Σ���ָ�����ѯ��������
		*
		*����ֵ��
		*	����false��ʾ������Ч�ַ�����true��ʾ����Ч�ַ�
		*/
		//�����һ��ִ��(��˼���ǲ�֧��Ƕ�ף�Ƕ�׿���ʹ�������Select)
		bool SelectBeack(__in const char *pSQL, e_strType, __out va_list ap);
		/*
		*�����¼��ָ������
		*
		*/

		/*
		*��ȡ MYSQL����״̬
		*
		*����ֵ��
		*		����true ��ʾû�жϿ�  ����false ��ʾmysql�����Ѿ��Ͽ�
		*
		*/
		bool GetMysqlConnectStatus()
		{
			if (m_bOpen)
			{
				if( mysql_ping(&Mysql)  == 0)//��������״̬��
				{
					return true;
				}
			}
			return m_bOpen;
		}
		/*
		*�����ѯ���ж�������� ���ִ��sql�����ȡ��������Ҫ���ݺ���ôνӿ�����
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
		*�ر�mysql����
		*/
		void CloseMysql(){
			//if ( _pMysql )
			{
				mysql_close( &Mysql );
				//_pMysql = NULL;
			}	
		}
	
		//�Ƿ��Զ��ύ
		void AutoCommitSQL(BOOL bAuto = TRUE);

	private:
		bool execute(const char* pSQL);
	private:
		//MYSQL_INFO m_Lastmysql;
		CString m_strOldSql;	//�ɵ�SQL���
		MYSQL_RES *m_pResult;//��¼��
		bool m_bOpen;


	//�����ӿ�
	public:
		static CString GetBinPath();
		static WCHAR* ToWChar(char *str);
	};


/************************************************************************/
/*							MYSQL ���ӳ�                                */
/************************************************************************/
	class mysql_connection_pool
	{

	public:
		~mysql_connection_pool();
		//����ģʽ
		static mysql_connection_pool *GetInstance();
		
		//��ʼ������
		size_t InitConnection(int nInitialSize=1);
		//����mysql��Ϣ
		void SetMysqlInfo(const char* pIP,
			const char* pUser,
			const char* pPwd,
			const char* pDatabase,
			unsigned short nPort /* = 3306 */);
		//��ȡmsyql��������
		CTools *GetConnection();
		//�������Ӷ���
		void ReleaseConnection(CTools * pObject);
		//�������ӳ�
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

