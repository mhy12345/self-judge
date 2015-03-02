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
#include<map>
#include<string>
#include<fstream>
using namespace std;
#define STRLEN 350
#define TOTCASE 55
#define RS_SYE 10
#define RS_NOR 0
#define RS_AC 1
#define RS_TLE 2
#define RS_RE 3
#define RS_MLE 4
#define RS_WA 5
#define ROOT_PATH "/home/toby/Program/"
#define HOME_PATH ROOT_PATH"self-judge/"
#define RECORD_PATH HOME_PATH"variable.rec"
char current_path[STRLEN];
bool file_exist(const char *file_name) 
{
		char cc[STRLEN];
		sprintf(cc,"if [ -f \"%s\" ] ;\nthen exit 1 \n else exit 0\nfi",file_name);
		return system(cc);
}
bool directory_exist(const char *file_name) 
{
		char cc[STRLEN];
		sprintf(cc,"if [ -d \"%s\" ] ;\nthen exit 1 \n else exit 0\nfi",file_name);
		return system(cc);
}
//PrintLog{{{
//#define STDLOG
inline void PrintLog(const char *x,int y,int z)
{
		FILE *Log=fopen(HOME_PATH"log.txt","a");
		fprintf(Log,x,y,z);
		fclose(Log);
#ifdef STDLOG
		printf(x,y,z);
#endif
}
inline void PrintLog(const char *x,int y)
{
		FILE *Log=fopen(HOME_PATH"log.txt","a");
		fprintf(Log,x,y);
		fclose(Log);
#ifdef STDLOG
		printf(x,y);
#endif
}
inline void PrintLog(const char *x,const char *y)
{
		FILE *Log=fopen(HOME_PATH"log.txt","a");
		fprintf(Log,x,y);
		fclose(Log);
#ifdef STDLOG
		printf(x,y);
#endif
}
inline void PrintLog(const char *x)
{
		FILE *Log=fopen(HOME_PATH"log.txt","a");
		fprintf(Log,"%s",x);
		fclose(Log);
#ifdef STDLOG
		printf(x);
#endif
}
class EV_t
{
		map<string,string> List;
		public:
		void Init_EV()
		{
				ifstream fin(RECORD_PATH);
				string s1,s2;
				getline(fin,s1);
				if (s1!="#Environment Variable")
				{
						return;
				}
				while (fin>>s1>>s2)
				{
						List[s1]=s2;
				}
				fin.close();
		}
		void Add_var(string s1,string s2)
		{
				List[s1]=s2;
		}
		string Find_var(string s1)
		{
				if (List.find(s1)==List.end())return string("");
				else return List[s1];
		}
		void Print_EV()
		{
				ofstream fout(RECORD_PATH);
				fout<<"#Environment Variable"<<endl;
				map<string,string> :: iterator it1;
				for (it1=List.begin();it1!=List.end();it1++)
				{
						fout<<it1->first<<" "<<it1->second<<endl;
				}
		}
}Var;

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
				bool Init(FILE *gs);
};

Task::~Task()
{
		for (int i=LeftRange;i<=RightRange;i++)delete testc[i];
}
const char *Task::GetExecuteName()const
{
		return ExecuteName;
}
bool Task::Compile()const
{
		char cc[STRLEN];
		system("mkdir ./tmp");
		sprintf(cc,"g++ %s -o "HOME_PATH"tmp/%s >Compile.log\n",SourceName,ExecuteName);
		PrintLog(cc);
		if (system(cc))
		{
				printf("%s\n",cc);
				return true;
		}
		return false;
}
int Task::Judge()
{
		if (Compile())
		{
				Print("Compile Error!\n");
				return -1;
		}
		char in[STRLEN],out[STRLEN],cc[STRLEN],LogDir[STRLEN];
		sprintf(cc,"mkdir %s_difflog",ExecuteName);
		sprintf(LogDir,"%s_difflog",ExecuteName);
		system(cc);
		printf("\033[34mTask:%s\033[0m\n",ExecuteName);
		for (int i=LeftRange;i<=RightRange;i++)
		{
				PrintLog("Case %d:\n",i);
				sprintf(in,DataIn,i);
				sprintf(out,DataOut,i);
				sprintf(cc,"cp %s "HOME_PATH"tmp/%s\n",in,FileIn);
				PrintLog(cc);
				system(cc);
				testc[i]->Run();
				if (!testc[i]->GetStatus())
				{
						//	sprintf(cc,"diff %s "HOME_PATH"/tmp/%s -w >%s/DiffLog%d.txt",out,FileOut,LogDir,i);
						//	sprintf(cc,HOME_PATH"fdiff Normal %s "HOME_PATH"/tmp/%s",out,FileOut,LogDir,i);
						//	system(cc);
						sprintf(cc,HOME_PATH"fdiff Normal %s "HOME_PATH"tmp/%s  >%s/DiffLog%d.txt",out,FileOut,LogDir,i);
						int diffres;
						if ((diffres=system(cc)))
						{
								PrintLog("fdiff_return:%d\n",diffres);
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
bool Task::Init(FILE *gs)
{
		freopen("/dev/null","w",stderr);
		if (fscanf(gs,"%s\n",ExecuteName)==EOF)return false;;//Project Name
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
				testc[i]=new TestCase(this,TimeLimit,MemoryLimit,i);
		}
		return true;
}

TestCase::TestCase(const Task* task,int TimeLimit,int MemoryLimit,int Id):
		task(task),TimeLimit(TimeLimit),MemoryLimit(MemoryLimit),Id(Id)
{
};
void TestCase::Print_Status()
{
		printf("\033[35m% 4d\033[0m   ",Id);
		switch (Status)
		{
				case RS_AC : ::Print("Accept               ");break;
				case RS_TLE: ::Print("Time Limit Exceed    ");break;
				case RS_WA : ::Print("Wrong Answer         ");break;
				case RS_MLE: ::Print("Memory Limit Exceed  ");break;
				case RS_RE : ::Print("Runtime Error        ");break;
				default:     ::Print("Unknown Result       ");break;
		}
		printf("\033[0m% 6dms   % 6dkb\n",Time,Memory);
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
		char path_old[STRLEN],path_new[STRLEN];
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

				sprintf(cc,"./%s </dev/null >/dev/null",task->GetExecuteName());
				getcwd(path_old,STRLEN);
				//strcpy(path_new,path_old);
				//strcat(path_new,"/tmp");
				strcpy(path_new,HOME_PATH"tmp");

				chdir(path_new);
				PrintLog("Change directory into %s\n",path_new);

				int status=system(cc);
				PrintLog("%s\n",cc);
				chdir(path_old);
				PrintLog("Change directory into %s\n",path_old);
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
								if (Time>TimeLimit*1000)
								{
										Status=RS_TLE;
								}
								else
								{
										if (Time<5)
												Status=RS_MLE;
										else
												Status=RS_RE;
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

Task T[11];
int main(int argc,const char* args[])
{
		FILE *Log=fopen("log.txt","w");
		fclose(Log);
		char cfg_path[STRLEN];
		char str[STRLEN];
		fclose(fopen(HOME_PATH"log.txt","w"));
		Var.Init_EV();
		printf("\033[0m\n");
		strcpy(cfg_path,ROOT_PATH);
		chdir(cfg_path);
		int t;
		if (args[1])
		{
				strcpy(cfg_path,args[1]);
		}else
		{
				printf("\033[33mConfigure Files:\033[0m\n");
				while (fgets(str,STRLEN,stdin))
				{
						if (strstr(str,"old")==str)
						{
								string str=Var.Find_var("cfg_path");
								if (str.size())
								{
										strcpy(cfg_path,str.c_str());
										PrintLog("Set cfg_path = %s\n",cfg_path);
										break;
								}else
								{
										printf("Cannot Find Record \"cfg_path\"");
										PrintLog("Cannot Find Record \"cfg_path\"");
										continue;
								}
						}else if (strstr(str,"ls")==str)
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
						}else if (strstr(str,"cd ")==str)
						{
								system(str);
								str[t=(int)strlen(str)-1]='\0';
								strcat(cfg_path,str+3);
								strcat(cfg_path,"/");
								if (!directory_exist(cfg_path))
								{
										printf("Cannot find directory:%s\n",cfg_path);
										cfg_path[t]='\0';
								}
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
		Var.Add_var("cfg_path",cfg_path);
		FILE *cfg_file=fopen(cfg_path,"r");
		if (!cfg_file)
		{
				printf("Cannot Find File: %s\n",cfg_path);
				PrintLog("Cannot Find File: %s\n",cfg_path);
				return 0;
		}
		strcpy(current_path,cfg_path);
		for (int i=strlen(current_path)-1;i>=0;i--)
		{
				if (current_path[i]=='/')
				{
						current_path[i]='\0';
						break;
				}
		}
		chdir(current_path);
		int topt=0;
		while (T[topt++].Init(cfg_file));
		topt--;
		char cc[30];
		if (topt>1)
				fgets(cc,30,stdin);
		if (!strcmp(cc,"\n"))
		{
				for (int i=0;i<topt;i++)
						T[i].Judge();
		}else 
		{
				cc[strlen(cc)-1]='\0';
				for (int i=0;i<topt;i++)
						if (!strcmp(T[i].GetExecuteName(),cc))
								T[i].Judge();
		}
		Var.Print_EV();
}
