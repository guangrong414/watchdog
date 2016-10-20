// watchdog.cpp : �������̨Ӧ�ó������ڵ㡣
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
 * @desc        ���̽ṹ��
 * @todo        []
 */
struct proc 
{
    pid_t pid;					//����id
	string strName;				//������
	int timestamp;				//����ʱ��
	string strPath;				//�����ļ��ľ���·��
	string param1;				//����ִ�в���1
	string param2;				//����ִ�в���2
	string param3;				//����ִ�в���3
	string param4;				//����ִ�в���4
	string param5;				//����ִ�в���5
};

vector<proc> proc_vec;

/*
����: strName �ǽ���������strName Ϊ""��NULL ��ʾ�������������н���,
	��������ָ���������Ľ��̣�������������û��ָ�����������С��ɹ����أ�NULL��ʧ�ܷ��أ�������Ϣ
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
				memcpy(pError, "�ӽ��̴���ʧ��",sizeof(pError));
				return pError;
			}
			//�ӽ����д���
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
					memcpy(pError, "�ӽ��̴���ʧ��",sizeof(pError));
					return pError;
				}
				//�ӽ����д���
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
		perror("���̴���ʧ��");
		return 0;
	}
	//�������˳�
	else if(pid > 0)
	{
		return 0;
	}
	
	//�����µĻỰ
	setsid();

	//�ı乤��Ŀ¼
	if(chdir("/") < 0)
	{
		perror("chdir error");
		return 0;
	}

	//�����ļ���дģʽ
	umask(0);


	//����׼���롢��׼�������׼������ļ��������ر�
	/*close(0);
	open("/dev/null", 0_RDWR);
	dup2(0, 1);
	dup2(0, 2);*/


	//��ʼ����������
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

