/*
  g++ -Wall myMemoryFS.cpp `pkg-config fuse --cflags --libs` -o myMemoryFS
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <vector>
#include<cstring>
#include<string>
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
using namespace std;

struct file {
	int type;
	char * name;
	vector<struct file> childs;
	char * path;
	char* value;
	file() {
		name=(char *)malloc(sizeof(char)*100);
		path=(char *)malloc(sizeof(char)*1000);
		value=(char *)malloc(sizeof(char)*1000);
	}
};
struct file root;
void init() {
	root.type=0;
	//root.name="/".c_str();
	strcpy(root.name,"/");
	strcpy(root.path,"/");
// files and folder inside root
	struct file dir1;
	dir1.type=0;
	strcpy(dir1.name,"dir1");
	dir1.path=(char *)malloc(sizeof(char)*100);
	strcpy(dir1.path,root.path);
	strcat(dir1.path,dir1.name);
	struct file dir2;
	dir2.type=0;
	strcpy(dir2.name,"dir2");
	strcpy(dir2.path,root.path);
	strcat(dir2.path,dir2.name);
	struct file file1;
	file1.type=1;
	strcpy(file1.name,"file1");
	strcpy(file1.value,"This is file1");
	strcpy(file1.path,root.path);
	strcat(file1.path,file1.name);
	struct file dir11;
	dir11.type=0;
	strcpy(dir11.name,"dir11");
	dir11.path=(char *)malloc(sizeof(char)*100);
	strcpy(dir11.path,dir1.path);
	strcat(dir11.path,"/");
	strcat(dir11.path,dir11.name);
	struct file dir12;
	dir12.type=0;
	strcpy(dir12.name,"dir12");
	strcpy(dir12.path,dir1.path);
	strcat(dir12.path,"/");
	strcat(dir12.path,dir12.name);
	struct file file21;
	file21.type=1;
	strcpy(file21.name,"file21");
	strcpy(file21.value,"This is file21");
	strcpy(file21.path,dir2.path);
	strcat(file21.path,"/");
	strcat(file21.path,file21.name);
	dir1.childs.push_back(dir11);
	dir1.childs.push_back(dir12);
	dir2.childs.push_back(file21);
	root.childs.push_back(dir1);
	root.childs.push_back(dir2);
	root.childs.push_back(file1);
} 
int locate=0;
static int search_file(const char *path, struct file node,struct file * currentfile) {
	printf("searching.... %s\n",node.path);
	fflush(stdout);
	if(strcmp(path,node.path)==0) {
		*currentfile=node;
		locate=1;
		return 1;
		} else {
			for(vector<struct file>::iterator it=node.childs.begin();it!=node.childs.end();++it) {
				if(!locate)
			 	search_file(path,*it,currentfile);
			}
		}
	return 0;
}
static int myMemoryFS_getattr(const char *path, struct stat *stbuf) {
	int res=0;
	memset(stbuf, 0, sizeof(struct stat));
	printf("myMemoryFS_getattr 1 %s\n",path);
	if (strcmp(path, "/") == 0) {
		printf("myMemoryFS_getattr 1 %s\n",path);
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		res=0;
	} else {
	fflush(stdout);
	struct file * currentfile = (struct file *)malloc(10000*sizeof(struct file));
	locate=0;
	search_file(path,root,currentfile);
	if(locate&&currentfile->type==0) {
		printf("Assiging stat to the directory\n");
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		} else if (locate&&currentfile->type==1) {
				printf("Assiging stat to the file\n");
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_size = strlen(currentfile->value);
		} else {
			res = -ENOENT;
		}
	}
	printf("\n\nmyMemoryFS_getattr -->%s\n",path);
	printf("<--myMemoryFS_getattr %s\n\n",path);
	//}
	return res;
}

static int myMemoryFS_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;
	printf("\n\nmyMemoryFS_readdir--> %s\n",path);
	if(strcmp(path, root.path) == 0) {
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	for(vector<struct file>::iterator it=root.childs.begin();it!=root.childs.end();++it) {
				printf("adding to the buffer %s\n",(*it).name);
			 	filler(buf,(*it).name,NULL,0);
			}
	} else {
		locate=0;
		struct file * currentfile = (struct file *)malloc(10000*sizeof(struct file));
		search_file(path,root,currentfile);
		if(locate) {
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		for(vector<struct file>::iterator it=currentfile->childs.begin();it!=currentfile->childs.end();++it) {
				printf("adding to the buffer %s\n",(*it).name);
			 	filler(buf,(*it).name,NULL,0);
			}
		}
	}
	printf("<--- myMemoryFS_readdir\n\n");
	return 0;
}

static int myMemoryFS_open(const char *path, struct fuse_file_info *fi)
{
	printf("\n\nmyMemoryFS_open --->> %s\n",path);
	locate=0;
	struct file * currentfile = (struct file *)malloc(10000*sizeof(struct file));
	search_file(path,root,currentfile);
	if (!locate) {
		printf(" not a file  --->>");
		return -ENOENT;
	}
	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;
	return 0;
}

static int myMemoryFS_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi) {
	printf(" myMemoryFS_read--->> %s\n",path);
	locate=0;
	struct file * currentfile = (struct file *)malloc(10000*sizeof(struct file));
	search_file(path,root,currentfile);
	size_t len;
	(void) fi;
	if(locate) {
	printf(" myMemoryFS_read 1--->> %s\n",path);
	len = strlen(currentfile->value);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, currentfile->value + offset, size);
	} else
		size = 0;
	}
	printf("<--myMemoryFS_read %s\n\n",path);
	return size;
}

struct temp:fuse_operations{

temp(){
getattr= myMemoryFS_getattr;
readdir= myMemoryFS_readdir;
open= myMemoryFS_open;
read= myMemoryFS_read;
//mkdir = myMemoryFS_mkdir;
//rmdir = myMemoryFS_rmdir;
//rename = myMemoryFS_rename;//write = myMemoryFS_write;
}};
static struct temp myMemoryFS_oper;

int main(int argc, char *argv[])
		
{
	init();
	//printf("myMemoryFS_read");
	return fuse_main(argc, argv, &myMemoryFS_oper, NULL);
}
