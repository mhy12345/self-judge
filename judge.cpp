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
}
class Task;
class TestCase
{
		public:
				Task* task;
				int id;
				int TimeLimit,MemoryLimit;
				int Score;
				int Time,Memory;
				int Status;
				void Print_Status()
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
				void SetTimeLimit(int TimeLimit)
				{
						this->TimeLimit=TimeLimit;
				}
				void SetMemoryLimit(int MemoryLimit)
				{
						this->MemoryLimit=MemoryLimit;
				}
				int Run();

};
class Task
{
		private:
				char din[STRLEN],dout[STRLEN];
				int lrange,rrange;
				char sname[STRLEN];//Source File Name
				char exname[STRLEN];
				char pin[STRLEN],pout[STRLEN];//Input
				int tlim[TOTCASE],mlim[TOTCASE];
				TestCase *testc[TOTCASE];
				int totcase;
		public:
				friend int TestCase::Run();
				~Task()
				{
						for (int i=0;i<totcase;i++)
								delete testc[i];
				}
				bool Compile()
				{
						char cc[STRLEN];
						sprintf(cc,"g++ %s -o ./tmp/%s >Compile.log",sname,exname);
						if (system(cc))return true;
						return false;
				}
				int Judge()
				{
						char in[STRLEN],out[STRLEN],cc[STRLEN];
						for (int i=lrange;i<=rrange;i++)
						{
								//printf("\033[46mCase %d:",i);printf("\033[0m\n");
								PrintLog("Case %d:\n",i);
								sprintf(in,din,i);
								sprintf(out,dout,i);
								sprintf(cc,"cp %s ./tmp/%s",in,pin);
								system(cc);
								testc[i]->Run();
								if (!testc[i]->Status)
								{
										sprintf(cc,"diff %s ./tmp/%s -w >res%d_diff.log",out,pout,i);
										if (system(cc))
										{
												//Print("Wrong Answer\n");
												testc[i]->Status=RS_WA;
										}else
										{
												//Print("Accept\n");
												testc[i]->Status=RS_AC;
										}
								}
								testc[i]->Print_Status();
						}
						return 0;
				}
				void Init(const char *cfg_name="log.txt")
				{
						FILE *Log=fopen(cfg_name,"w");
						fclose(Log);
						fprintf(Log,"Judging Time:--\n");
						freopen("/dev/null","w",stderr);
						FILE *gs=fopen("judge.cfg","r");
						fscanf(gs,"%s\n",exname);//Project Name
						fscanf(gs,"%s\n",din);//Data Input
						fscanf(gs,"%s\n",dout);//Data Output
						fscanf(gs,"%d %d\n",&lrange,&rrange);//Index Range
						fscanf(gs,"%s\n",sname);//Source File Name
						fscanf(gs,"%s\n",pin);//Input File Name
						fscanf(gs,"%s\n",pout);//Output File Name
						int TimeLimit,MemoryLimit;
						fscanf(gs,"%d%d",&TimeLimit,&MemoryLimit);
						totcase=rrange-lrange+1;
						for (int i=lrange;i<=rrange;i++)
						{
								testc[i-lrange]=new TestCase;
								testc[i-lrange]->SetTimeLimit(TimeLimit);
								testc[i-lrange]->SetMemoryLimit(MemoryLimit);
								testc[i-lrange]->task=this;
								testc[i-lrange]->id=i;
						}
				}

}T;
void call()
{
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
				PrintLog("Set Memory Limit %d\n",rm_as_new.rlim_cur);
				setrlimit(RLIMIT_AS,&rm_as_new);

				sprintf(cc,"./%s </dev/null >/dev/null \n",task->exname);
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
int main(int argc,const char* args[])
{
		T.Init();
		if (T.Compile())
		{
				Print("Compile Error!\n");
				return 0;
		}
		T.Judge();
}
