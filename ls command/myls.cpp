#include<stdio.h>
#include<fcntl.h>
#include<utime.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#include<iostream>
#include<dirent.h>
#include<vector>
#include<grp.h>
#include<pwd.h>
#include <map>
#include <algorithm> 
#include <iomanip>  
using namespace std;
void recursiveFunction(char * dir, vector<char> options, int a);
void recursivemultiple(vector<char *> dirs, vector<char> options, int a);
string getfilepermission(mode_t mode);
char getFiletype(mode_t mode);
string getOwnername(uid_t uid);
string getGroupname(gid_t gid);
char* getTime(time_t t);
int getlink(string path, char * linkbuff);
vector<string> retrieveFileList(char * s, int hidden);
int myalphasort(const struct dirent** a, const struct dirent** b);
vector<struct fileproperty> retrieveFileListWithProperty(string path, vector<string> filelist, int hidden);
vector<struct fileproperty> retrieveFileListWithProperty(string path, int hiddenordir);
int sumblocks(vector<struct fileproperty> filelistprop);
void printFilelistWithProperty(vector<struct fileproperty> filelistproperty);
void printfilelist(vector<string> filelist);
vector<string> retrieveFileListFromProperty(vector<struct fileproperty> filelistproperty, int hidden);
bool compareBySize(const fileproperty &a, const fileproperty &b);
bool compareByLastModSec(const fileproperty &a, const fileproperty &b);
vector<struct fileproperty> sortfilelist(vector<struct fileproperty> filelist,int fieldno);
struct fileproperty createfileproperty(string path, string filen);
struct fileproperty createfileproperty(string path);
int isDirectory(char * dir);
void printfileproperty(struct fileproperty file);

/* This structure represents the different properties of a file */
struct fileproperty {
char filetype;
string filepermission;
int nooflinks;
string owner;
string group;
int size;
char * lastmod;
int lastmodsec;
int blocks;
string filename;
char * link; 
};

/* To get the file permission in the string form */
string getfilepermission(mode_t mode) {
char access[9];	
string accessvalue;
if(mode & S_IRUSR) {
		access[0] = 'r';
	} else {
		access[0] = '-';
	}
	if(mode & S_IWUSR) {
		access[1] = 'w';
	} else {
		access[1] = '-';
	}
	if(mode & S_IXUSR) {
		access[2] = 'x';
	} else {
		access[2] = '-';
	}
	if(mode & S_IRGRP) {
		access[3] = 'r';
	} else {
		access[3] = '-';
	}
	if(mode & S_IWGRP) {
		access[4] = 'w';
	} else {
		access[4] = '-';
	}
	if(mode & S_IXGRP) {
		access[5] = 'x';
	} else {
		access[5] = '-';
	}
	if(mode & S_IROTH) {
		access[6] = 'r';
	} else {
		access[6] = '-';
	}
	if(mode & S_IWOTH) {
		access[7] = 'w';
	} else {
		access[7] = '-';
	}
	if(mode & S_IXOTH) {
		access[8] = 'x';
	} else {
		access[8] = '-';
	} 
	accessvalue.append(access);
return accessvalue;
}

/* To get the file type in the character form */
char getFiletype(mode_t mode) {
	if(S_ISREG(mode)) {
		return '-';
	} else if (S_ISDIR(mode)) {
		return 'd';
	} else if (S_ISLNK(mode)) {
		return 'l';
	} else if (S_ISSOCK(mode)) {
		return 's';
	}
}

/* To retrieve the owner name */
string getOwnername(uid_t uid) {
	struct passwd * user= getpwuid(uid);
return user->pw_name;
}

/* To retrieve the group name */
string getGroupname(gid_t gid) {
	struct group * user= getgrgid(gid);
return user->gr_name;
}

/* To get the time in the desired format */
char* getTime(time_t t) {
char *timebuff = (char *)malloc(sizeof(char)*100);
struct tm *time = localtime(&t);
strftime (timebuff,100,"%h %e %H:%M ",time);
return timebuff; 
}

/* To get the information about the link*/
int getlink(string path, char * linkbuff) {
char * charpath = const_cast<char*>(path.c_str());
size_t buffsize = 100;
return readlink(charpath,linkbuff,buffsize);
}

/* To create the fileproperty on the basis of the path argument and filename for the file */
struct fileproperty createfileproperty(string path, string filen) {
struct stat *statbuffer = (struct stat*)malloc(sizeof(struct stat));
path.append("/");
path.append(filen);
char * charfilename = const_cast<char*>(path.c_str());
lstat(charfilename,statbuffer);
struct fileproperty file; 
file.filetype = getFiletype(statbuffer->st_mode);
file.filepermission = getfilepermission(statbuffer->st_mode);
file.nooflinks = statbuffer->st_nlink;
file.owner = getOwnername(statbuffer->st_uid);
file.group = getGroupname(statbuffer->st_gid);
file.size = (int)statbuffer->st_size;
file.lastmod = getTime(statbuffer->st_atime);
file.lastmodsec = statbuffer->st_atime;
file.filename = filen;
file.blocks = statbuffer->st_blocks;
char * linkbuff = (char *)malloc(sizeof(char)*100);
if(getFiletype(statbuffer->st_mode) == 'l') {
	if(getlink(path,linkbuff)!=-1) {
		file.link = linkbuff;
	}
}
return file;
}

/* To create the fileproperty on the basis of the path argument for the file */
struct fileproperty createfileproperty(string path) {
struct stat *statbuffer = (struct stat*)malloc(sizeof(struct stat));
char * charfilename = const_cast<char*>(path.c_str());
lstat(charfilename,statbuffer);
struct fileproperty file;
file.filetype = getFiletype(statbuffer->st_mode);
file.filepermission = getfilepermission(statbuffer->st_mode);
file.nooflinks = statbuffer->st_nlink;
file.owner = getOwnername(statbuffer->st_uid);
file.group = getGroupname(statbuffer->st_gid);
file.size = (int)statbuffer->st_size;
file.lastmod = getTime(statbuffer->st_atime);
file.lastmodsec = statbuffer->st_atime;
file.filename = path;
char * linkbuff = (char *)malloc(sizeof(char)*100);
if(getFiletype(statbuffer->st_mode) == 'l') {
	getlink(path,linkbuff);
}
file.link = linkbuff;
return file;
}

/* To determine if the current path is the directory or not */
int isDirectory(char * dir)  {
struct fileproperty fileprop = createfileproperty(dir);

if(fileprop.filetype == 'd') {
return 1;
}

return 0;
}

/* To print the fileproperty */
void printfileproperty(struct fileproperty file) {
	cout << file.filetype << file.filepermission << " "<< setw(3) << right << file.nooflinks << " " <<  file.owner << " " <<  file.group << " " 
<< setw(6)<< right << file.size;
	cout << " " << file.lastmod << " " << file.filename;
	if(file.filetype == 'l') {
		cout << " -> " << file.link << endl; 
	} else {
		cout << endl;
	}
}

/* To sort the filename alphabatically*/
int myalphasort(const struct dirent** a, const struct dirent** b)
 {
    return strcasecmp((*b)->d_name, (*a)->d_name);
}


/* To retrieve the list of the file*/
vector<string> retrieveFileList(char * s, int hidden) {
	struct dirent **namelist; 
	vector<string> filelist;
	int nooffile = scandir(s,&namelist,0,myalphasort);
	vector<string> outputfile;
	while(nooffile--) {
		filelist.push_back(namelist[nooffile]->d_name);
	}
        
	int it1;
	  for (vector<string>::iterator it=filelist.begin(); it!=filelist.end(); ++it) {
		string filename = *it;
		char * charfilename = const_cast<char*>(filename.c_str());
		if(!hidden) {
			if(!((charfilename[0]=='.')&&charfilename[1]!='/')) {
				outputfile.push_back(filename);
			}
		} else {
			outputfile.push_back(filename);
		}	
	}
	return outputfile;
 }

/* To retrieve the list of the fileproperty for the path with the list of file*/
vector<struct fileproperty> retrieveFileListWithProperty(string path, vector<string> filelist, int hidden) {
	vector<struct fileproperty> filelistwithprop;
	map<string,struct fileproperty> mapfile;
	int it1;
	for(it1 = 0;it1 < filelist.size();it1++) {
		string filename = filelist[it1];
		char * charfilename = const_cast<char*>(filename.c_str());
		if(!hidden) {
				if(!((charfilename[0]=='.')&&charfilename[1]!='/')) {
					struct fileproperty fileprop = createfileproperty(path, filename);
					filelistwithprop.push_back(fileprop);
				}
		} else {
				struct fileproperty fileprop = createfileproperty(path, filename);
				filelistwithprop.push_back(fileprop);
		 }
	}
return filelistwithprop;
 }

/* To retrieve the list of the fileproperty for the path*/
vector<struct fileproperty> retrieveFileListWithProperty(string path, int hiddenordir) {
	vector<struct fileproperty> filelistwithprop;
	char * charfilename = const_cast<char*>(path.c_str());
	if(!hiddenordir) {
			if(!((charfilename[0]=='.')&&(charfilename[1]!='/'))) {
				struct fileproperty fileprop = createfileproperty(path);
				filelistwithprop.push_back(fileprop);
			}
	} else {
				struct fileproperty fileprop = createfileproperty(path);
				filelistwithprop.push_back(fileprop);
	 }
return filelistwithprop;
 } 

/* To get the total of the blocks */
int sumblocks(vector<struct fileproperty> filelistprop) {
	int sum = 0;
	for (vector<struct fileproperty>::iterator it=filelistprop.begin(); it!=filelistprop.end(); ++it) {
		sum = sum + (*it).blocks;
	}
	return sum/2;
}

/* To print the list of the fileproperty */
void printFilelistWithProperty(vector<struct fileproperty> filelistproperty) {
	cout << "total " << sumblocks(filelistproperty) << endl;
	for (vector<struct fileproperty>::iterator it=filelistproperty.begin(); it!=filelistproperty.end(); ++it) {
			printfileproperty(*it);
	}
}

/* To print the list of the filename */
void printfilelist(vector<string> filelist) {
if (isatty(1)) {
	int count = 0;
	for (vector<string>::iterator it=filelist.begin(); it!=filelist.end(); ++it) {
		if(count!=0&&count%3==0) {
				cout << endl;
		}
			count++;
			cout  << setw(25) << left << *it;
			
	}
} else {
	for (vector<string>::iterator it=filelist.begin(); it!=filelist.end(); ++it) {
				cout << *it << endl;
			
		}
}
	cout << endl;
}

/* To retrieve the list of the filename from the list of the fileproperty */
vector<string> retrieveFileListFromProperty(vector<struct fileproperty> filelistproperty, int hidden) {
	vector<string> filelist;
	for (vector<struct fileproperty>::iterator it=filelistproperty.begin(); it!=filelistproperty.end(); ++it) {
		filelist.push_back((*it).filename);
	}
return filelist;
}
/* To sort on the basis of the size */
bool compareBySize(const fileproperty &a, const fileproperty &b) {
    return a.size > b.size;
}

/* The function to sort on the basis of the last modification time */
bool compareByLastModSec(const fileproperty &a, const fileproperty &b) {
    return a.lastmodsec > b.lastmodsec;
}

/* This function is used to sort the list of file property on the basis of the parameter */
vector<struct fileproperty> sortfilelist(vector<struct fileproperty> filelist,int fieldno) {
	if(fieldno == 6) {
		sort(filelist.begin(),filelist.end(),compareBySize);
	} else if (fieldno == 8) {
		sort(filelist.begin(),filelist.end(),compareByLastModSec);
	} 
return filelist;
}

/* It is the function to treat the different options for the ls command like -l, -t -S, -a, -d
* The filenames vector contains the names of the files passed in the command line */
void treat(vector<char> options, vector<char *> filenames) {
int a=0,l=0,S=0,t=0,d=0,R=0;
vector<string> filelist;
for (vector<char>::iterator it=options.begin(); it!=options.end(); ++it) {
		if(*it=='a') {
			a = 1;
		} else if (*it == 'S') {
			S= 1;
		} else if (*it == 't') {
			t= 1;
		} else if (*it == 'd') {
			d= 1;
		} else if (*it == 'l') {
			l= 1;
		} else if (*it == 'R') {
			R= 1; 
		} else {
			cout << "ls: invalid option -- '" << *it << "'" << endl;
			exit(0); 
		}
	}

	vector<struct fileproperty> filelistprop; 
	if(filenames.size() == 0) {
		if(d) {
			filelist.push_back(".");
		}else {
			char s[1];
			s[0] = '.';
			filelist = retrieveFileList(s,a);
		}		
		if(l || t || S) {

			if(d) {
				filelistprop = retrieveFileListWithProperty(".",d);
			} else {
				filelistprop = retrieveFileListWithProperty(".",filelist,a);
			}		
		}
		if(S) {
			filelistprop = sortfilelist(filelistprop, 6);
		}
		if(t) {
			filelistprop = sortfilelist(filelistprop, 8);
		}
		if(l) {
			printFilelistWithProperty(filelistprop);	
		} else if (t || S) {
			filelist = retrieveFileListFromProperty(filelistprop,a);
			printfilelist(filelist);
		} else {
			printfilelist(filelist);
		} 
	} else {
		for(int i=0;i<filenames.size();i++) {
				 if(!R && opendir(filenames[i]) == NULL && open(filenames[i], O_RDONLY) == -1) {
				cout << "ls: cannot access " << filenames[i] << ": No such file or directory"<< endl;
				continue;
				} 
				if(!d&&isDirectory(filenames[i])) {
				cout << filenames[i] << ":" << endl;
					filelist = retrieveFileList(filenames[i],a);
				} else {
					filelist.push_back(filenames[i]);
				}			
			if(l || t || S) {
				if(!d&&isDirectory(filenames[i])) {
					filelistprop = retrieveFileListWithProperty(filenames[i],filelist,a);
				} else { 
				int aOrd = a || d;
					filelistprop =  retrieveFileListWithProperty(filenames[i],aOrd);
				}
			}
			if(S) {
				filelistprop = sortfilelist(filelistprop, 6);
			}
			if(t) {
				filelistprop = sortfilelist(filelistprop, 8);
			}
			if(l) {
				printFilelistWithProperty(filelistprop);	
			} else if (t || S) {
				filelist = retrieveFileListFromProperty(filelistprop,a);
				printfilelist(filelist);
			} else {
				printfilelist(filelist);
			} 
		}	
	}

}

/* This function to iterate through the directory recursively to implement the -R options */
void recursiveFunction(char * dir, vector<char> options, int a) {
	if((access(dir,X_OK) == 0)&&isDirectory(dir)) {
		int len = strlen(dir);
		if( !((dir[len-1]=='.' && dir[len-2]=='/') || (dir[len-1]=='.'&& dir[len-2]=='.' && dir[len-3] == '/'))) {
			//cout << dir << ":" << endl;
			vector<string> filelist = retrieveFileList(dir,a);
			vector<char *> filename;
			filename.push_back(dir);
			treat(options, filename);
			for (vector<string>::iterator it=filelist.begin(); it!=filelist.end(); ++it) {
				string filename = *it;			
				string path = dir;
				path.append("/");
				path.append(filename);
				char * charfilename = const_cast<char*>(path.c_str());
				recursiveFunction(charfilename,options,a);
			}
		}
	}

	return;		
}

/** This function is used to iterate through the list of directories provided as a argument for the recursive call -R option */
void recursivemultiple(vector<char *> dirs, vector<char> options,int a) {
	for (vector<char *>::iterator it=dirs.begin(); it!=dirs.end(); ++it) {
		char * dir = *it;
		if(opendir(dir) == NULL && open(dir, O_RDONLY) == -1) {
				cout << "ls: cannot access " << dir<< ": Permission Denied"<< endl;
				continue;
				}  
		if(isDirectory(dir)) {
			recursiveFunction(dir,options,a);
		} else {
			vector<char *> filename;
			filename.push_back(dir);
			treat(options, filename);
		}	
	}
}

int main(int argc, char **argv) {
	vector<char> options;
	vector<char *> filenames;	
	for (int i=1;i<argc;i++) {
		char * st = argv[i];
		if(st[0] == '-') {
			for(int j=1;j<strlen(st);j++) {
				options.push_back(st[j]);
			}
		} else {
			filenames.push_back(argv[i]);
		}
	}
int R = 0, a = 0, d =0;
for (vector<char>::iterator it=options.begin(); it!=options.end(); ++it) {
	 if (*it == 'R') {
				R= 1;
			}
	
	 if (*it == 'a') {
				a= 1;
			}
	 if(*it == 'd') {
			
	d = 1;
		}
	}
	
		if(R && !d) {
			if(filenames.size() == 0) {
				char s[1];
				s[0] = '.';
				filenames.push_back(s);
			}
			recursivemultiple(filenames, options,a);	
		}	 
		else {
			treat(options,filenames);
		}	
 	return 0;
}
