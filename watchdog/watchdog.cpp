// watchdog.cpp : 定义控制台应用程序的入口点。
//

#include "stdio.h"
#include "errno.h"
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <vector>

//using namespace std;
using std::vector;
using std::string;

#define PROC_SERVER "/mnt2/1/tcp_sayhello/tcp_sayhello/server"
#define PROC_CLIENT "/mnt2/1/tcp_sayhello/tcp_sayhello/client"


/**
 * @name        struct proc
 * @author      liyang
 * @date        2016-10-17 15:12:45
 * @desc        进程结构体
 * @todo        []
 */
struct proc 
{
    pid_t pid;					//进程id
	string strName;				//进程名
	int timestamp;				//启动时间
	string strPath;				//进程文件的绝对路径
	string param1;				//进程执行参数1
	string param2;				//进程执行参数2
	string param3;				//进程执行参数3
	string param4;				//进程执行参数4
	string param5;				//进程执行参数5
};

vector<proc> proc_vec;

/*
参数: strName 是进程名，若strName 为""或NULL 表示遍历容器，运行进程,
	否则运行指定进程名的进程，若进程容器中没有指定进程则不运行。成功返回：NULL，失败返回：错误信息
*/
char*  HandleProc(string strName)
{
	pid_t pid;
	char* pError = new char[1024];
	//char szError[1024] = {0}; 
	if(strName.empty())
	{
		for(int i = 0; i< proc_vec.size(); i++)
		{

			if((pid = fork()) < 0)
			{
				memcpy(pError, "子进程创建失败",sizeof(pError));
				return pError;
			}
			//子进程中处理
			else if(pid == 0)
			{
				if(-1 == execl(proc_vec[i].strPath.c_str(), 
					proc_vec[i].strName.c_str(), 
					proc_vec[i].param1.c_str(),
					proc_vec[i].param2.c_str(),
					proc_vec[i].param3.c_str(),
					proc_vec[i].param4.c_str(),
					proc_vec[i].param5.c_str(),
					(char *)0))
				{
					sprintf(pError, "%s process execl error , reason:%s", proc_vec[i].strName.c_str(), strerror(errno));
					return pError;
				}
			}
			else //------------------>ELSE.START-2016.10.18.10.24.35
			{//----------------------------------------------->ELSE.START-2016.10.18.10.24.35
				proc_vec[i].pid = pid;
			} //----------------------------------------------->ELSE.END-2016.10.18.10.24.35
			
		}
	}
	else //------------------>ELSE.START-2016.10.18.10.33.30
	{//----------------------------------------------->ELSE.START-2016.10.18.10.33.30
		for(int i = 0; i< proc_vec.size(); i++)
		{
			if(proc_vec[i].strName == strName)
			{
				if((pid = fork()) < 0)
				{
					memcpy(pError, "子进程创建失败",sizeof(pError));
					return pError;
				}
				//子进程中处理
				else if(pid == 0)
				{
					if(-1 == execl(proc_vec[i].strPath.c_str(), 
									proc_vec[i].strName.c_str(), 
									proc_vec[i].param1.c_str(),
									proc_vec[i].param2.c_str(),
									proc_vec[i].param3.c_str(),
									proc_vec[i].param4.c_str(),
									proc_vec[i].param5.c_str(),
									(char *)0))
					{
						sprintf(pError, "%s process execl error , reason:%s", proc_vec[i].strName.c_str(), strerror(errno));
						return pError;
					}
				}
				else //------------------>ELSE.START-2016.10.18.10.24.35
				{//----------------------------------------------->ELSE.START-2016.10.18.10.24.35
					proc_vec[i].pid = pid;
				} //----------------------------------------------->ELSE.END-2016.10.18.10.24.35
			}
		}
	} //----------------------------------------------->ELSE.END-2016.10.18.10.33.30
	return NULL;
}



int main()
{
	pid_t pid;
	char* pRet = NULL; 
	string strName;


	if((pid=fork())<0)
	{
		perror("进程创建失败");
		return 0;
	}
	//父进程退出
	else if(pid > 0)
	{
		return 0;
	}
	
	//创建新的会话
	setsid();

	//改变工作目录
	if(chdir("/") < 0)
	{
		perror("chdir error");
		return 0;
	}

	//设置文件读写模式
	umask(0);


	//将标准输入、标准输出、标准出错的文件描述符关闭
	/*close(0);
	open("/dev/null", 0_RDWR);
	dup2(0, 1);
	dup2(0, 2);*/


	//初始化进程容器
	proc tmp_proc;
	tmp_proc.strName = "client";
	tmp_proc.strPath = PROC_CLIENT;

	proc_vec.push_back(tmp_proc);

	tmp_proc.strName = "server";
	tmp_proc.strPath = PROC_SERVER;

	proc_vec.push_back(tmp_proc);

	if((pRet = HandleProc(strName)) != NULL)
	{
		perror(pRet);
		delete []pRet;
		return 0;
	}

	sleep(5);

	while ( 1 ) //------>while.cond-2016.10.18.10.43.39
	{//----------------------------------------------->while.start-2016.10.18.10.43.39
		if((pid = wait(NULL)) == -1)
		{
			perror("error waitid");
			return 0;
		}
		else
		{
			for ( int i=0; i< proc_vec.size(); i++ ) //--------->for.cond-2016.10.18.10.38.13
			{//----------------------------------------------->for.start-2016.10.18.10.38.13
				if(pid == proc_vec[i].pid)
				{
					sleep(3);
					if((pRet = HandleProc(proc_vec[i].strName)) != NULL)
					{
						perror(pRet);
						delete []pRet;
						break;
					}
				}
			}//----------------------------------------------->for.end-2016.10.18.10.38.13
		}
	}//----------------------------------------------->while.end-2016.10.18.10.43.39

	return 0;
}

