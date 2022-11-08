#define _GNU_SOURCE
#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#define NOPE &nope

typedef struct {
  char Name[50];
  int Pid;
  int PPid;
  int child[256];
  int top;
  int visit;
} TNode;
TNode nope;
TNode *ptrmap[50000];

typedef struct TreeLine{
  char *blocks[1024];
  int cur;
} Line;
Line t;
void putThread(int id){
	int PPid, Pid, len, cur;	
	char path[50];
	char num[10];
	sprintf(path, "/proc/%d/status", id);
	char lines[7][255];
	FILE *fp = NULL;	
	fp = fopen(path, "r");
	for(int i=0; i<7; i++) fgets(lines[i], 255, fp);
	char Name[50];
	memset(Name, '\0', sizeof(char)*50);
	cur = 6;
	len = strlen(lines[0]);
	for(int i=0; cur < len-1; i++) Name[i] = lines[0][cur++];

	cur = 5;
        len = strlen(lines[5]);
	memset(num, '\0', sizeof(char)*10);
	for(int i=0; cur < len-1; i++) num[i] = lines[5][cur++];
	Pid = atoi(num);

	cur = 6;
        len = strlen(lines[6]);
        memset(num, '\0', sizeof(char)*10);
        for(int i=0; cur < len-1; i++) num[i] = lines[6][cur++];
        PPid = atoi(num);

	TNode *node = (TNode *)malloc(sizeof(TNode));
	strcpy(node->Name, Name);
	node->Pid = Pid;
	node->PPid = PPid;
	node->top = 0;
	node->visit = 0;
	if(PPid == 0){
		ptrmap[node->Pid] = node;
		return;
	}
	if(ptrmap[PPid] == NOPE)
		putThread(PPid);
	TNode *pp = ptrmap[node->PPid];
	pp->child[pp->top++] = node->Pid;
	ptrmap[node->Pid] = node;
}
void printTree(int Pid){
	TNode *p, *pp;
	p = ptrmap[Pid];
	if(p->top==0){
		char *pad0;
		int dis;
		dis = 0;
        	char blank[50];
        	asprintf(&pad0, "%s\n", p->Name);
        	t.blocks[t.cur++] = pad0;
		pp = ptrmap[p->PPid];
        	while(p->PPid != 0){
			char *pad1;
			pp = ptrmap[p->PPid];
			memset(blank, ' ', sizeof(char)*50);
                        blank[strlen(pp->Name)] = '\0';
			if(!dis){
				if(pp->child[0] == p->Pid){
					asprintf(&pad1, "%s─┬─", pp->Name);
				}else if(pp->child[pp->top-1] == p->Pid){
                                        asprintf(&pad1, "%s └─", blank);
					dis++;
				}else{
                                        asprintf(&pad1, "%s ├─", blank);
					dis++;
				}
			}else{
				if(pp->child[0] == p->Pid){
                                 	if(pp->top == 1)
                                                asprintf(&pad1, "%s   ", blank);
					else
				 		asprintf(&pad1, "%s │ ", blank);
                                }else if(pp->child[pp->top-1] == p->Pid){
                                        asprintf(&pad1, "%s   ", blank);
					dis++;
                                }else{
                                        asprintf(&pad1, "%s │ ", blank);
					dis++;
                                }
			}
			/*
			if(pp->child[0] == p->Pid){
				if(pp->visit){
					memset(blank, ' ', sizeof(char)*30);
                                        blank[strlen(pp->Name)] = '\0';
                                        if(pp->top == 1)
						asprintf(&pad1, "%s   ", blank);
					else
						asprintf(&pad1, "%s │ ", blank);
				}else{
					pp->visit = 1;
                                        asprintf(&pad1, "%s─┬─", pp->Name);
				}
			}else{
				memset(blank, ' ', sizeof(char)*30);
				blank[strlen(pp->Name)] = '\0';
				if(pp->child[pp->top-1] == p->Pid)
					asprintf(&pad1, "%s └─", blank);
				else
					asprintf(&pad1, "%s ├─", blank);
			}*/
			t.blocks[t.cur++] = pad1;
			p = pp;
		}

		do{
			t.cur--;
			printf("%s", t.blocks[t.cur]);
			free(t.blocks[t.cur]);
		}while(t.cur!=0);
		return;
	}
	for(int i=0; i<p->top; i++){
        	printTree(p->child[i]);
	}
}
int main(){
	t.cur = 0;
	for(int i=0; i<50000; i++) ptrmap[i] = NOPE;
	char infile[50];
	struct dirent *ptr;
	DIR *dir;
	dir = opendir("/proc");
	while((ptr=readdir(dir))!=NULL){
		char ch = ptr->d_name[0];
		int id = atoi(ptr->d_name);
		if(ch >= '0' && ch <= '9' && ptrmap[id] == NOPE)
			putThread(id);
	}
	/*
	for(int i=0; i<50000; i++){
		if(ptrmap[i] != NOPE){
			TNode *p = ptrmap[i];
			if(!strcmp(p->Name, "ssh-agent"))
				printf("%d\n", p->Pid);
		}
	}
	*/
	printTree(1);
	for(int i=0; i<50000; i++){
		if(ptrmap[i] != NOPE)
			free(ptrmap[i]);
	}
	return 0;
}
