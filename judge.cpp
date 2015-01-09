#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<algorithm>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<sys/wait.h>
#include<sys/signal.h>
#include<signal.h>
#include<unistd.h>
using namespace std;
#define STRLEN 150
#define TOTCASE 55
#define RS_SYE 10
#define RS_NOR 0
#define RS_AC 1
#define RS_TLE 2
#define RS_RE 3
#define RS_MLE 4
#define RS_WA 5
#define ROOT_PATH "/home/toby/Program"
//PrintLog{{{
inline void PrintLog(const char *x,int y,int z)
{
		FILE *Log=fopen("log.txt","a");
		fprintf(Log,x,y,z);
		fclose(Log);
		//				printf(x,y,z);
}
inline void PrintLog(const char *x,int y)
{
		FILE *Log=fopen("log.txt","a");
		fprintf(Log,x,y);
		fclose(Log);
		//				printf(x,y);
}
inline void PrintLog(const char *x)
{
		FILE *Log=fopen("log.txt","a");
		fprintf(Log,"%s",x);
		fclose(Log);
		//				printf("%s",x);
}

void Print(const char *cc)
{
		printf("\033[1m");
		if (strstr(cc,"Accept"))
		{
				printf("\033[32m%s",cc);
		}else if (strstr(cc,"Wrong Answer"))
		{
				printf("\033[31m%s",cc);
		}else if (strstr(cc,"Time Limit Exceed"))
		{
				printf("\033[33m%s",cc);
		}else if (strstr(cc,"Memory Limit Exceed"))
		{
				printf("\033[34m%s",cc);
		}else if (strstr(cc,"Compile Error"))
		{
				printf("\033[35m%s",cc);
		}else if (strstr(cc,"Runtime Error"))
		{
				printf("\033[36m%s",cc);
		}else
				printf("%s\n",cc);
		printf("\033[0m");
}
//}}}

class Task;
class TestCase
{
		private:
				const Task* task;
				int TimeLimit;
				int MemoryLimit;
				int Id;
				int Score;
				int Time,Memory;
				int Status;
		public:
				TestCase(const Task* task,int TimeLimit,int MemoryLimit,int Id);
				int Run();
				void Print_Status();
				void SetTimeLimit(int TimeLimit);
				void SetMemoryLimit(int MemoryLimit);
				void SetStatus(int Status);
				int GetStatus();
				const Task* GetTask();
				void SetTask(const Task* task);
};
class Task
{
		private:
				static const int max_case=TOTCASE;
				static const int max_length=STRLEN;
				char DataIn[max_length],DataOut[max_length];
				int LeftRange,RightRange;
				char SourceName[max_length];//Source File Name
				char ExecuteName[max_length];
				char FileIn[max_length],FileOut[max_length];//Input
				TestCase *testc[max_case];
				int TotalCase;
		public:
				~Task();
				const char *GetExecuteName()const;
				bool Compile()const;
				int Judge();
				void Init(const char *cfg_name);

};

Task::~Task()
{
		for (int i=0;i<TotalCase;i++)delete testc[i];
}
const char *Task::GetExecuteName()const
{
		return ExecuteName;
}
bool Task::Compile()const
{
		char cc[STRLEN];
		system("mkdir ./tmp");
		sprintf(cc,"g++ %s -o ./tmp/%s >Compile.log",SourceName,ExecuteName);
		if (system(cc))return true;
		return false;
}
int Task::Judge()
{
		char in[STRLEN],out[STRLEN],cc[STRLEN],LogDir[STRLEN];
		sprintf(cc,"mkdir %s_difflog",ExecuteName);
		sprintf(LogDir,"%s_difflog",ExecuteName);
		system(cc);
		for (int i=LeftRange;i<=RightRange;i++)
		{
				PrintLog("Case %d:\n",i);
				sprintf(in,DataIn,i);
				sprintf(out,DataOut,i);
				sprintf(cc,"cp %s ./tmp/%s",in,FileIn);
				system(cc);
				testc[i]->Run();
				if (!testc[i]->GetStatus())
				{
						sprintf(cc,"diff %s ./tmp/%s -w >%s/DiffLog%d.txt",out,FileOut,LogDir,i);
						if (system(cc))
						{
								//Print("Wrong Answer\n");
								testc[i]->SetStatus(RS_WA);
						}else
						{
								//Print("Accept\n");
								sprintf(cc,"echo Accept >%s/DiffLog%d.txt",LogDir,i);
								system(cc);
								testc[i]->SetStatus(RS_AC);
						}
				}
				testc[i]->Print_Status();
		}
		return 0;
}
void Task::Init(const char *cfg_name="log.txt")
{
		FILE *Log=fopen("log.txt","w");
		fclose(Log);
		fprintf(Log,"Judging Time:--\n");
		freopen("/dev/null","w",stderr);
		FILE *gs=fopen(cfg_name,"r");
		fscanf(gs,"%s\n",ExecuteName);//Project Name
		fscanf(gs,"%s\n",DataIn);//Data Input
		fscanf(gs,"%s\n",DataOut);//Data Output
		fscanf(gs,"%d %d\n",&LeftRange,&RightRange);//Index Range
		fscanf(gs,"%s\n",SourceName);//Source File Name
		fscanf(gs,"%s\n",FileIn);//Input File Name
		fscanf(gs,"%s\n",FileOut);//Output File Name
		int TimeLimit,MemoryLimit;
		fscanf(gs,"%d%d",&TimeLimit,&MemoryLimit);
		TotalCase=RightRange-LeftRange+1;
		for (int i=LeftRange;i<=RightRange;i++)
		{
				testc[i-LeftRange]=new TestCase(this,TimeLimit,MemoryLimit,i);
		}
}

TestCase::TestCase(const Task* task,int TimeLimit,int MemoryLimit,int Id):
		task(task),TimeLimit(TimeLimit),MemoryLimit(MemoryLimit),Id(Id)
{
};
void TestCase::Print_Status()
{
		switch (Status)
		{
				case RS_AC : ::Print("Accept               ");break;
				case RS_TLE: ::Print("Time Limit Exceed    ");break;
				case RS_WA : ::Print("Wrong Answer         ");break;
				case RS_MLE: ::Print("Memory Limit Exceed  ");break;
				case RS_RE : ::Print("Runtime Error        ");break;
				default:     ::Print("Unknown Result       ");break;
		}
		printf("\033[0m% 6ds   % 6dkb\n",Time,Memory);
}
void TestCase::SetTimeLimit(int TimeLimit)
{
		this->TimeLimit=TimeLimit;
}
void TestCase::SetMemoryLimit(int MemoryLimit)
{
		this->MemoryLimit=MemoryLimit;
}
void TestCase::SetStatus(int Status)
{
		this->Status=Status;
}
int TestCase::GetStatus()
{
		return Status;
}
const Task* TestCase::GetTask()
{
		return task;
}
int TestCase::Run()
{
		char cc[STRLEN];
		char path_old[440],path_new[440];
		int pid;
		pid=fork();
		if (!pid)
		{
				rlimit rm_cpu_old,rm_cpu_new;
				getrlimit(RLIMIT_CPU,&rm_cpu_old);
				rm_cpu_new.rlim_cur=TimeLimit+1;
				rm_cpu_new.rlim_max=TimeLimit+1;
				setrlimit(RLIMIT_CPU,&rm_cpu_new);

				rlimit rm_as_old,rm_as_new;
				getrlimit(RLIMIT_AS,&rm_as_old);
				rm_as_new.rlim_cur=(MemoryLimit+1)*1024*1024;
				rm_as_old.rlim_max=(MemoryLimit+1)*1024*1024;
				PrintLog("Set Memory Limit %d\n",(int)rm_as_new.rlim_cur);
				setrlimit(RLIMIT_AS,&rm_as_new);

				sprintf(cc,"./%s </dev/null >/dev/null \n",task->GetExecuteName());
				PrintLog(cc);
				getcwd(path_old,440);
				strcpy(path_new,path_old);
				strcat(path_new,"/tmp");
				chdir(path_new);
				int status=system(cc);
				chdir(path_old);
				PrintLog("Return Status:%d\n",status);
				PrintLog("WIFEXITED:\t%d\n",WIFEXITED(status));
				PrintLog("WIFSIGNALED:\t%d\n",WIFSIGNALED(status));
				PrintLog("WEXITSTATUS:\t%d\n",WEXITSTATUS(status));
				PrintLog("WTERMSIG:\t%d\n",WTERMSIG(status));
				setrlimit(RLIMIT_AS,&rm_as_old);
				setrlimit(RLIMIT_CPU,&rm_cpu_old);
				if (WIFEXITED(status))
				{
						if (!WEXITSTATUS(status))
								exit(0);
						exit(1);//Runtime Error or Interrupted
				}
				exit(1);
		}else
		{
				int status;
				rusage rusa;
				wait4(pid,&status,WUNTRACED,&rusa);
				int cur_Time=(int)rusa.ru_utime.tv_sec*1000+(int)rusa.ru_utime.tv_usec/1000;
				int cur_Mem=(int)rusa.ru_maxrss;
				PrintLog("Run Time:%dsec\n",cur_Time);
				PrintLog("Run Mem:%dkb\n",cur_Mem);
				Time=cur_Time;
				Memory=cur_Mem;
				if (status==-1)
				{
						Print("System Error\n");
						PrintLog("System Error");
						exit(0);
				}
				if (WIFEXITED(status))
				{
						if (!WEXITSTATUS(status))
						{
								if (Time>=TimeLimit*1000)
										Status=RS_TLE;
						}else
						{
								if (cur_Time)
								{
										Status=RS_TLE;
								}
								else
								{
										Status=RS_MLE;
										Memory=0;
								}
						}
				}else
				{
						Print("System Error\n");
						exit(-1);
				}
		}
		return 0;
}

Task T;
int main(int argc,const char* args[])
{
		char cfg_path[STRLEN];
		char str[STRLEN];
		printf("\033[0m\n");
		strcpy(cfg_path,ROOT_PATH);
		chdir(cfg_path);
		if (args[1])
		{
				strcpy(cfg_path,args[1]);
		}else
		{
				printf("\033[33mConfigure Files:\033[0m\n");
				while (fgets(str,STRLEN,stdin))
				{
						if (strstr(str,"ls")==str)
						{
								system("ls --color=always");
						}else if (strstr(str,"cd ..")==str)
						{
								getcwd(cfg_path,STRLEN);
								int len=(int)strlen(cfg_path);
								for (int i=len-1;i>=0;i--)
										if (cfg_path[i]=='/')
										{
												cfg_path[i]='\0';
												break;
										}
								chdir(cfg_path);
								//printf(str);
						}else if (strstr(str,"cd ")==str)
						{
								system(str);
								str[strlen(str)-1]='\0';
								strcat(cfg_path,"/");
								strcat(cfg_path,str+3);
								chdir(cfg_path);
						}else if (strstr(str,"vim ")==str)
						{
								system(str);
						}else
						{
								str[strlen(str)-1]='\0';
								getcwd(cfg_path,STRLEN);
								strcat(cfg_path,"/");
								strcat(cfg_path,str);
								break;
						}
						printf("%s\n",cfg_path);
				}
		}
		T.Init(cfg_path);
		if (T.Compile())
		{
				Print("Compile Error!\n");
				return 0;
		}
		T.Judge();
}
