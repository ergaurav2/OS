#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<iostream>
#include<sys/wait.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include<fstream>
#include<sstream>
#include <string>
#include<map>
using namespace std;
void pipehandling(int noofcomm,char ** commarr[], char * infile, char * outputfile, bool append,  bool bckgrnd);
void executeCommand(char * charstr);
bool isinternalcommand(char * comm);
void executeincomm(char * comm, char **);
int countforhistory();
//char * getshellrc(char * command);
//	map<string, string> shellrcmap;
char incomm[10][10] = {"history", "!", "!-", "export"};
int main(int argc, char *argv[], char *envp[]) {
	fstream historyfs;
	fstream shellrc;
	char buf;	
	printf("MyShell:");
	int temp;
	int count = countforhistory();
	historyfs.open("history.txt", fstream::in | fstream::out | fstream::app);
	/*shellrc.open("MyShell.shellrc", fstream::in | fstream::out | fstream::app);
	string line;
	getline(shellrc, line);
	int position = line.find(" ");
	string var = line.substr(0,position);
	string val = line.substr(position+1);
	*/
	while(1) {
			char * command = (char *) malloc(sizeof(char)*100);
			string inputstr,filered;
			getline(cin,inputstr);			
			char * charstr = (char *)inputstr.c_str();
			char * countstr = (char *) malloc(sizeof(char)*100);
			stringstream ss; 
			count++;
			ss << count;
			string s = ss.str();	
			countstr = (char *)s.c_str();		
			historyfs.write(s.c_str(),strlen(countstr));			
			historyfs.write(" ",strlen(" "));			 
			historyfs.write(charstr,strlen(charstr));
			historyfs.write("\n",strlen("\n"));
			executeCommand(charstr);			
			historyfs.flush();
			      printf("MyShell:");	
	}
	historyfs.close();
	printf("\n");
	return 0;
}

void pipehandling(int noofcomm,char ** commarr[], char * infilestr, char * outfilestr, bool append, bool bckgrnd) {
				int **pipefd = (int **) malloc(sizeof(int *)*noofcomm);
				int outfiledescp = -1, infiledescp = -1;				
				if(outfilestr!=NULL) {
					if(append) {
					outfiledescp = open(outfilestr,O_RDWR | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR);
					} else {
						outfiledescp = open(outfilestr,O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
					}
				}
				if(infilestr!=NULL) {
					infiledescp = open(infilestr, O_RDONLY);
				}
				for(int j = 0; j<=noofcomm;j++) {
					pipefd[j] = (int *) malloc(sizeof(int)*2);
					pipe(pipefd[j]);	
					int pid = fork();
					int val;
					if(pid==0) {	
							if(j!=0) {								
								dup2(pipefd[j-1][0], 0);
							} else if (infiledescp!=-1) {
								dup2(infiledescp,0);						
							}
							if(j!=noofcomm)	{ 						 
								dup2(pipefd[j][1], STDOUT_FILENO);
							} else if (outfiledescp!=-1){
								dup2(outfiledescp, STDOUT_FILENO);
							}
							//strstr(commarr[j][0],incomm[1]);
							//cout << "command " << commarr[j][0] << endl; 
							if(isinternalcommand(commarr[j][0])) {
								executeincomm(commarr[j][0], commarr[j]);
							} else {
								val = execvp(commarr[j][0],commarr[j]);
							}
					}
					if(j==noofcomm&&outfiledescp!=-1)				
					close(outfiledescp);	
					if(val==-1) {			
						cout << commarr[j][0] <<": command not found" << endl;
					} 
					if(j!=0)
					close(pipefd[j-1][0]);
					if(j==0&&infiledescp!=-1)
					close(infiledescp);
					close(pipefd[j][1]);
					if(!bckgrnd)
					wait(0);
				}	


}

void executeCommand(char * charstr) {
	char * outfilestr = (char *) malloc(sizeof(char)*100);
	char * infilestr = (char *) malloc(sizeof(char)*100);
	outfilestr = NULL,infilestr = NULL;
	bool append = false;
	stringstream ss;
	int pos1 = -1,pos2 = -1;
	ss << charstr << "\0" << endl;
	string inputstr = ss.str();
	pos1 = inputstr.find(">>");
	if(pos1 != -1) {
		append = true;
	}
	if(pos1 == -1) {
		pos1 = inputstr.find(">");
	}
	if(pos1!=-1) {
			string firststr = inputstr.substr(0,pos1);
			string secondstr =  inputstr.substr(pos1+1);
			int startpos = secondstr.find_first_not_of(" ");
			secondstr = secondstr.substr(startpos);
			int endpos = secondstr.find(" ");
			secondstr = secondstr.substr(0,endpos);
			charstr = (char *)firststr.c_str();
			outfilestr = (char *)secondstr.c_str();
			outfilestr[strlen(outfilestr)] = '\0';
	}
	int pos3 = inputstr.find("&");
	bool bckgrnd = false;
	if(pos3!=-1) {
		bckgrnd = true;
	}
	char **input = (char **) malloc(sizeof(char *)*100);
	int i=0;
	char * command = (char *) malloc(sizeof(char)*100);
	command = strtok(charstr,"|");
	char  * temp = (char *) malloc(sizeof(char)*100);
	strcpy(temp,command);
	stringstream ss1;
	string inputstr1 = temp;
	pos2 = inputstr.find("<");
	if(pos2!=-1) {
			string firststr = inputstr1.substr(0,pos2);
			string secondstr =  inputstr1.substr(pos2+1);
			i=0;
			int newpos = secondstr.find_first_not_of(" ");
			secondstr = secondstr.substr(newpos);
			int endpos = secondstr.find(" ");
			secondstr = secondstr.substr(0,endpos);
			command = (char *)firststr.c_str();
			infilestr = (char *)secondstr.c_str();
	}

			while(command!=NULL) {
				input[i] = command;
				/*if(getshellrc(command)!=NULL) {
						input[i] = getshellrc(command);					
					} */
				command = strtok(NULL,"|");
				i++;
			} 
				input[i] = NULL;
				if(!strcmp(input[0],"exit")) {
					printf("bye!\n");
					exit(0);				
				}
				int noofcomm;
				char ** commarr[100];
				for(int j = 0; input[j]!=NULL;j++) {
					char **comm = (char **) malloc(sizeof(char *)*100);
					char * temp = (char *) malloc(sizeof(char)*100);
					temp = strtok(input[j]," ");
					i=0;
					while(temp!=NULL) {
						comm[i] = temp;
						temp = strtok(NULL," ");
						i++;	
					} 
					commarr[j] = comm;
					noofcomm = j;
				}
		      pipehandling(noofcomm,commarr,infilestr,outfilestr,append,bckgrnd);	
}
bool isinternalcommand(char * comm) {
	for(int i=0;i<=3;i++) {
		if(strstr(comm,incomm[i])!=NULL) {
				return true;
			}
		}
return false;
}

void executeincomm(char * comm, char** commarg) {
	if(!(strcmp(comm,incomm[0]))) {
		fstream historyfs;
		historyfs.open("history.txt", fstream::in | fstream::out | fstream::app); 
		char * line = (char *) malloc(sizeof(char *)*100);
		while(!historyfs.eof()) {
			cout << line << endl;			
			historyfs.getline(line,100);
		}
	}
	else if(strstr(comm,incomm[1])!=NULL) {
		fstream historyfs;
		bool minus = false;
		int latestcount = 1;
		historyfs.open("history.txt", fstream::in | fstream::out | fstream::app);
		char * line = (char *) malloc(sizeof(char *)*100);
		char * temp = (char *) malloc(sizeof(char *)*100);
		char * countstr = (char *) malloc(sizeof(char)*100);
		 char * search = strstr(comm,incomm[2]);
		if(search!=NULL) {
			minus = true;
		  latestcount = countforhistory();
		} else {
			search = strstr(comm,incomm[1]);		
		}
		strncpy(countstr,++search,strlen(comm));
		if(minus) {
			int counter = atoi(countstr);
			counter = latestcount+counter;
			stringstream ss; 
			ss << counter;
			string s = ss.str();	
			countstr = (char *)s.c_str();		
		}
		while(!historyfs.eof()) {
			historyfs.getline(line,100);
			strcpy(temp, line);
			char * currcount = strtok(temp," ");
			char * commandstr = (char *) malloc(sizeof(char)*100);
			if(!strcmp(currcount, countstr)) {
				search = strstr(line," ");
				strncpy(commandstr,++search,strlen(line));
				executeCommand(commandstr);
				break;
			} 
		}
	} else if(strstr(comm,incomm[3])!=NULL) {
		stringstream ss;
		ss << commarg[1];
		string s = ss.str();
		int pos1 = s.find("=");
		string var = s.substr(0,pos1);
		string value = s.substr(pos1+1);
		char * varc = (char *)var.c_str();
		char * valuec = (char *)value.c_str();
		setenv(varc, valuec, 1);
	} 
}


int countforhistory() {
	fstream historyfs;
	historyfs.open("history.txt", fstream::in | fstream::out | fstream::app);
	char * line = (char *) malloc(sizeof(char *)*100);
	char * lastline = (char *) malloc(sizeof(char *)*100);
	historyfs.getline(line,100);
int count;
	while(!historyfs.eof()) {
		count = historyfs.gcount();
		strcpy(lastline, line);
		historyfs.getline(line,100);
	}
	count = atoi(strtok(lastline," "));
	historyfs.close();
return count;
}


/*char * getshellrc(char * command) {
	stringstream ss;
		ss << command;
		string s = ss.str();
		cout << "string " << s << endl;
		//int pos = s.find(incomm[3]);
		//cout << "pos " << pos << endl;
		//pos = pos+strlen(incomm[3]);
		int pos1 = s.find("$");
		if(pos1 == -1) {
			return NULL;		
		}
		cout << "pos1 " << pos1 << endl;
		string var = s.substr(pos1+1);
		//string value = s.substr(pos1+1);
		cout << "variable " << var << endl;
		char * varc = (char *)var.c_str();
		char * valuec;
		if(shellrcmap.find(var) !=shellrcmap.end()) {
			string valuestr = shellrcmap.at(var);
			valuec = (char *)valuestr.c_str();
		} else {
			valuec = getenv(varc);	
		}	
		cout << "value " << valuec << endl;
	return valuec;
}*/
