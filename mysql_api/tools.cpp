#include "stdafx.h"
#include "tools.h"
#include <stdarg.h>

using namespace _TOOLS_;


bool CTools::initmysql(const char* pIP, 
	const char* pUser, 
	const char* pPwd, 
	const char* pDatabase, 
	unsigned short nPort /* = 3306 */ )
{
	//mysql初始化
	mysql_init(&Mysql);
	//连接数据库
	if (!mysql_real_connect(&Mysql, pIP, pUser, pPwd, pDatabase, nPort, NULL, CLIENT_MULTI_STATEMENTS))
	{
		m_bOpen = false;
		return m_bOpen;
	}
	mysql_query(&Mysql,"SET NAMES 'GBK'");
	mysql_query(&Mysql, "set global max_connections=4000");//设置最大连接数量（mysql服务器默认为151  所以这里找到配置文件 只有这个办法咯）
	mysql_options(&Mysql,MYSQL_READ_DEFAULT_GROUP,"IMServer");
	m_bOpen = true;
	return m_bOpen;
	
}

bool CTools::select_sql_beack(__in const char *pSQL, __out ...)
{
	va_list ap;
	va_start( ap, pSQL );
	bool bRet = SelectBeack( pSQL,e_char,ap );
	va_end( ap );
	return bRet;
}

bool CTools::select_sql_beack_s(__in const char *pSQL, __out ...)
{
	va_list ap;
	va_start( ap, pSQL );
	bool bRet = SelectBeack( pSQL,e_cstring,ap );
	va_end( ap );
	return bRet;
}

bool CTools::SelectBeack(__in const char *pSQL, e_strType nstrType, __out va_list ap)
{ 
	MYSQL_ROW row;				//数据
	MYSQL_RES *pResult = NULL;	//记录集指针
	unsigned int num_fields;	//字段个数
	MYSQL_FIELD* pFields;		//字段类型信息

	if (!m_bOpen)
	{
		return false;
	}
	try
	{
		if (m_strOldSql.CompareNoCase(pSQL) != 0)
		{
			//没有查询过SQL新建查询
			ClearResult();//清理记录集数据
			g_mtxmysql.Lock();
			if(!(mysql_query(&Mysql, pSQL) == 0) )
			{
				//查询失败
				g_mtxmysql.Unlock();
				return false;
			}
			//查询数据
			//获取当前记录集中的数据总数 不过这里不能同时与下面同时使用 这里我暂时没有找出解决办法
// 			pResult = mysql_store_result(_pMysql);
// 			int ncount = mysql_num_rows(pResult);

			pResult = mysql_use_result(&Mysql);
			g_mtxmysql.Unlock();
			int nCount = mysql_field_count(&Mysql);
			if (!pResult)
			{
				return false;
			}
			m_strOldSql.Format("%s", pSQL);
		}
		else
		{
			pResult = m_pResult;
		}

		int *pnType = NULL;
		CString *pstrType = NULL;
		char * pszType = NULL;

		num_fields = mysql_num_fields(pResult);
		//获取一条数据
		if(!(row = mysql_fetch_row(pResult)))
		{
			ClearResult();
			return false;
		}
		for (unsigned int i=0; i<num_fields; i++)
		{
			pFields = mysql_fetch_field_direct(pResult,i);//获取当前字段信息
			enum_field_types nfieldtype = pFields->type;
			switch(nfieldtype)
			{
				//转整型  //这里有小数点的还没有做处理 需要进一步处理
			case MYSQL_TYPE_SHORT:
			case MYSQL_TYPE_LONG:
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_FLOAT:
			case MYSQL_TYPE_DOUBLE:
				{
					pnType = va_arg(ap, int *);
					if (pnType == NULL)
					{
						break;
					}
					if (row[i] == NULL)
					{
						*pnType = 0;
					}
					else
					{
						*pnType = atoi(row[i]);
					}

				}
				break;
				//转字符类型
			case MYSQL_TYPE_DATE:
			case MYSQL_TYPE_TIME:
			case MYSQL_TYPE_DATETIME:
			case MYSQL_TYPE_STRING:
			case MYSQL_TYPE_TINY:
			case MYSQL_TYPE_VAR_STRING:
				{
					if (nstrType == e_cstring)
					{
						pstrType = va_arg(ap,CString *);
						if (pstrType == NULL)
						{
							break;
						}
						if (row[i] == NULL)
						{
							*pstrType = "";
						}
						else
						{
							*pstrType = row[i];
						}

					}
					else
					{
						pszType = va_arg(ap,char *);
						if (pszType == NULL)
						{
							break;
						}
						if (row[i] == NULL)
						{
							pszType[0] = 0;
						}
						else
						{
							strcpy(pszType,row[i]);
						}
						/**pszType = strData.c_str();*/

					}

				}
				break;
			default:
				{
					// 我未预料到的类型。把参数指针向后移；
					pszType = va_arg( ap, char *);

				}
				break;
			}
		}
	}
	catch (...)
	{
		ClearResult();
		return false;
	}
	m_pResult = pResult;
	return true;
}

bool CTools::execute(const char* pSQL)
{
	ASSERT(pSQL);
	try
	{
		int nRet = mysql_query(&Mysql, pSQL);
		if ( nRet != 0 )
		{ 
			const char* pError=NULL;
			pError = mysql_error(&Mysql);
			TRACE("MYSQL错误提示%s\r\n",pError);
			return false;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;

}

bool CTools::delete_sql(__in const char *pSQL)
{
	return execute(pSQL);
}

bool CTools::update_sql(const char *pSQL)
{

	return execute(pSQL);
}

bool CTools::insert_sql(const char *pSQL)
{

	return execute(pSQL);
}


void _TOOLS_::CTools::AutoCommitSQL(BOOL bAuto /*= TRUE*/)
{
	mysql_autocommit(&Mysql, bAuto);
}

CString CTools::GetBinPath()
{
	CString sPath;
	GetModuleFileName(NULL, sPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	sPath.ReleaseBuffer();
	int nPos;
	nPos = sPath.ReverseFind('\\');
	sPath = sPath.Left(nPos);
	return sPath;
}


WCHAR* CTools::ToWChar(char *str)
{
	static WCHAR buffer[1024];
	memset(buffer, 0, 1024);
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str), buffer, 1024);
	return buffer;
}






/************************************************************************/
/*							MYSQL 连接池                                */
/************************************************************************/
mysql_connection_pool::mysql_connection_pool()
{

}

mysql_connection_pool::~mysql_connection_pool()
{
	DestoryConnPool();
}

mysql_connection_pool * mysql_connection_pool::GetInstance()
{
	static mysql_connection_pool object;
	return &object;
}

CTools * mysql_connection_pool::GetConnection()
{
	g_mtxMysqlPool.Lock();
	CTools *pMysqlobject = NULL;
	if (the_queue.empty())
	{
		g_mtxMysqlPool.Unlock();
		return pMysqlobject;
	}
	pMysqlobject = the_queue.front();
	the_queue.pop();
	g_mtxMysqlPool.Unlock();
	return pMysqlobject;
}

size_t _TOOLS_::mysql_connection_pool::InitConnection(int nInitialSize/*=1*/)
{
	ASSERT(nInitialSize > 0);
	g_mtxMysqlPool.Lock();
	for ( int i=0; i<nInitialSize; i++ )
	{
		CTools *pObject = new CTools;
		if (!pObject->initmysql(m_szIP, m_szUser, m_szPwd, m_szDatabase, m_snPort))
		{
			delete pObject;
			pObject = NULL;
			continue;
		}
		/*
		*  使用者 批量执行SQL前需要执行START TRANSACTION语法使得事物开启
		*
		*  使用者 在需要批量SQL执行完成后方可执行COMMIT语法 使得数据一并提交
		*/
		pObject->AutoCommitSQL(FALSE);//关闭自动调教 采用事物提交
		the_queue.push(pObject);
	}
	size_t nSize = the_queue.size();
	g_mtxMysqlPool.Unlock();
	return nSize;
}

void _TOOLS_::mysql_connection_pool::SetMysqlInfo(const char* pIP, const char* pUser, const char* pPwd, const char* pDatabase, unsigned short nPort /* = 3306 */)
{
	ASSERT(pIP != NULL);
	ASSERT(pUser != NULL);
	ASSERT(pPwd != NULL);
	ASSERT(pDatabase != NULL);
	g_mtxMysqlPool.Lock();
	memcpy(m_szIP, pIP, strlen(pIP));
	memcpy(m_szUser, pUser, strlen(pUser));
	memcpy(m_szPwd, pPwd, strlen(pPwd));
	memcpy(m_szDatabase, pDatabase, strlen(pDatabase));
	m_snPort = nPort;
	g_mtxMysqlPool.Unlock();
}

void mysql_connection_pool::ReleaseConnection(CTools * pObject) 
{
	ASSERT(pObject);
	g_mtxMysqlPool.Lock();
	the_queue.push(pObject);
	g_mtxMysqlPool.Unlock();
}

void mysql_connection_pool::DestoryConnPool() {
	g_mtxMysqlPool.Lock();
	if (!the_queue.empty())
	{
		CTools *pMysqlobject = the_queue.front();
		the_queue.pop();
		pMysqlobject->insert_sql("COMMIT");
		delete pMysqlobject;
		pMysqlobject = NULL;
	}
	g_mtxMysqlPool.Unlock();
}