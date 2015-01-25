#include<iostream>
#include<cstdio>
#include<cstring>
#include<algorithm>
using namespace std;
#define MS_SYSTEMERROR -1
#define MS_ACCEPT 0
#define MS_WRONGANSWER 1
#define MS_PRESENTATIONERROR 2
#define MS_MISSINGSTD 3
#define MS_MISSINGOUT 4
#define BUFLEN 1000
char buf1[BUFLEN],buf2[BUFLEN];
char* rbuf1,*rbuf2;
int trans(char*& str,int &len)
{
		int res=0;
		while (*str==' ' || *str=='\t')str++,res++;
		len=(int)strlen(str);
		while ((len) && str[len-1]==' ' || str[len-1]=='\n' || str[len-1]=='\r')str[--len]='\0';
		return res;
}

int main(int argc,const char* args[])
{
		if (argc!=4)
		{
				fprintf(stderr,"Wrong Argument!\n");
				return MS_SYSTEMERROR;
		}
		if (strcmp(args[1],"Normal"))
		{
				fprintf(stderr,"Normal Type Only\n");
				return MS_SYSTEMERROR;
		}
		FILE *fres,*fstd;
		fres=fopen(args[2],"r");
		fstd=fopen(args[3],"r");
		if (!fres)
		{
				return MS_MISSINGSTD;
		}
		if (!fstd)
		{
				return MS_MISSINGOUT;
		}
		bool r1,r2;
		int l1,l2;
		int i,j;
		int cnt=0;
		bool PE_flag=false;
		int c1,c2;
		while (true)
		{
				cnt++;
				r1=(bool)fgets(buf1,sizeof(buf1),fres);
				r2=(bool)fgets(buf2,sizeof(buf2),fstd);
				rbuf1=buf1,rbuf2=buf2;
				c1=trans(rbuf1,l1);
				c2=trans(rbuf2,l2);
				if (!(r1+r2))
				{
										printf(PE_flag?"Presentation Error\n":"Accept\n");
						return MS_ACCEPT+2*PE_flag;
				}
				else if (!r1)
				{
						do
						{
								if (l2)
								{
										rbuf2[10]='\0';
										printf("At position (%d,%d):\n",cnt,1);
										printf("Read:<End Of File>\n");
										printf("But Expect:[%s]\n",rbuf2);
										return MS_WRONGANSWER;
								}
								r2=fgets(buf2,sizeof(buf2),fstd);
								rbuf2=buf2;
								c2=trans(rbuf2,l2);
								if (!r2)
								{
										printf(PE_flag?"Presentation Error\n":"Accept\n");
										return MS_ACCEPT+2*PE_flag;
								}
								cnt++;
						}while (true);
				}else if (!r2)
				{
						do
						{
								if (l1)
								{
										rbuf1[10]='\0';
										printf("At position (%d,%d):\n",cnt,1);
										printf("Read:[%s]\n",rbuf1);
										printf("But Expect:<End Of File>\n");
										return MS_WRONGANSWER;
								}
								r1=fgets(buf1,sizeof(buf1),fres);
								rbuf1=buf1;
								c1=trans(rbuf1,l1);
								if (!r1)
								{
										printf(PE_flag?"Presentation Error\n":"Accept\n");
										return MS_ACCEPT+2*PE_flag;
								}
								cnt++;
						}while (true);
				}else
				{
						if (rbuf1-buf1!=rbuf2-buf2)PE_flag=true;
						int ll=min(l1,l2);
						for (i=0;i<min(10,ll);i++,rbuf1++,rbuf2++)
								if (*rbuf1!=*rbuf2)
										break;
						if (*rbuf1!=*rbuf2)
						{
								*(rbuf1-i+10)=*(rbuf2-i+10)='\0';
								printf("At position (%d,%d):\n",cnt,c1+1);
								printf("Read:[%s]\n",rbuf1-i);
								printf("But Expect:[%s]\n",rbuf2-i);
								return MS_WRONGANSWER;
						}
						while (*rbuf1 || *rbuf2)
						{
								if (*rbuf1!=*rbuf2)
								{
										*(rbuf1+5)=*(rbuf2+5)='\0';
										printf("At postion(%d,%d):\n",cnt,c1+i-5+1);
										printf("Read:[%s]\n",rbuf1-5);
										printf("But Expect:[%s]\n",rbuf2-5);
										return MS_WRONGANSWER;
								}
								i++;
								rbuf1++,rbuf2++;
						}
				}
		}
		printf("System Error\n");
		return MS_SYSTEMERROR;
}
