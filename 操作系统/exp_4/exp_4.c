#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

struct dirent *entry;
struct stat s_buf;

void ls(char *path)
{
    DIR *dir = NULL;
    printf("%s:\n",path);
    if((dir = opendir(path))==NULL)
    {
        printf("no such file or directory: %s\n",path);
        exit(1);
    }
    while(entry=readdir(dir))
    {
        if(entry->d_name[0]=='.'){
            continue;
        }
        printf("%s\n",entry->d_name);  //输出文件或者目录的名称
    }
    printf("\n");
    closedir(dir);
}

void ls_l(char *path){
    DIR *dir = NULL;
    printf("%s:\n",path);
    if((dir = opendir(path))==NULL)
    {
        printf("no such file or directory: %s\n",path);
        exit(1);
    }
    while(entry=readdir(dir))
    {
        if(entry->d_name[0]=='.'){
            continue;
        }
        char file_path[1024];
        bzero(file_path,1024);
        strcat(file_path,path);
        strcat(file_path,"/");
        strcat(file_path,entry->d_name);
        stat(file_path,&s_buf);//获取文件信息，保存在s_buf中
            // 存储文件类型和访问权限
        char perms[11] = {0};
        // 判断文件类型
        switch(s_buf.st_mode & S_IFMT)
        {
            case S_IFLNK:
                perms[0] = 'l';
                break;
            case S_IFDIR:
                perms[0] = 'd';
                break;
            case S_IFREG:
                perms[0] = '-';
                break;
            case S_IFBLK:
                perms[0] = 'b';
                break;
            case S_IFCHR:
                perms[0] = 'c';
                break;
            case S_IFSOCK:
                perms[0] = 's';
                break;
            case S_IFIFO:
                perms[0] = 'p';
                break;
            default:
                perms[0] = '?';
                break;
        }
        // 判断文件的访问权限
        // 文件所有者
        perms[1] = (s_buf.st_mode & S_IRUSR) ? 'r' : '-';
        perms[2] = (s_buf.st_mode & S_IWUSR) ? 'w' : '-';
        perms[3] = (s_buf.st_mode & S_IXUSR) ? 'x' : '-';
        // 文件所属组
        perms[4] = (s_buf.st_mode & S_IRGRP) ? 'r' : '-';
        perms[5] = (s_buf.st_mode & S_IWGRP) ? 'w' : '-';
        perms[6] = (s_buf.st_mode & S_IXGRP) ? 'x' : '-';
        // 其他人
        perms[7] = (s_buf.st_mode & S_IROTH) ? 'r' : '-';
        perms[8] = (s_buf.st_mode & S_IWOTH) ? 'w' : '-';
        perms[9] = (s_buf.st_mode & S_IXOTH) ? 'x' : '-';

        // 硬链接计数
        int linkNum = s_buf.st_nlink;
        // 文件所有者
        char* fileUser = getpwuid(s_buf.st_uid)->pw_name;
        // 文件所属组
        char* fileGrp = getgrgid(s_buf.st_gid)->gr_name;
        // 文件大小
        int fileSize = (int)s_buf.st_size;
        // 修改时间
        char* time = ctime(&s_buf.st_mtime);
        char mtime[512] = {0};
        strncpy(mtime, time, strlen(time)-1);

        char buf[1024];
        sprintf(buf, "%s  %d  %s  %s  %d  %s  %s", perms, linkNum, fileUser, fileGrp, fileSize, mtime, entry->d_name);
        printf("%s\n",buf);
    }
    printf("\n");
    closedir(dir);
}
void ls_R(char *path){
    ls(path);
    DIR *dir = NULL;
    dir = opendir(path);
    while(entry=readdir(dir))
    {
        if(entry->d_name[0]=='.'){
            continue;
        }
        char file_path[1024];
        bzero(file_path,1024);
        strcat(file_path,path);
        strcat(file_path,"/");
        strcat(file_path,entry->d_name);
        stat(file_path,&s_buf);//获取文件信息，保存在s_buf中
        if(S_ISDIR(s_buf.st_mode))
        {
            ls_R(file_path);
        }
    }
    closedir(dir);
}
void ls_lR(char *path){
    ls_l(path);
    DIR *dir = NULL;
    dir = opendir(path);
    while(entry=readdir(dir))
    {
        if(entry->d_name[0]=='.'){
            continue;
        }
        char file_path[200];
        bzero(file_path,200);
        strcat(file_path,path);
        strcat(file_path,"/");
        strcat(file_path,entry->d_name);
        stat(file_path,&s_buf);//获取文件信息，保存在s_buf中
        if(S_ISDIR(s_buf.st_mode))
        {
            ls_lR(file_path);
        }
    }
    closedir(dir);
}
int main(int argc,char *argv[]){
    char *path;
    if(argc==1){//输出当前目录中的文件或者目录
        //path=getcwd(NULL,0);
        path=".";
        ls(path);
        exit(0);
    }
    else{
        if(strcmp(argv[1],"-l")!=0&&strcmp(argv[1],"-R")!=0&&strcmp(argv[1],"-lR")!=0){
            for(int i=1;i<argc;i++){
                path=argv[i];
                ls(path);
            }
            exit(0);
        }else{
            if(strcmp(argv[1],"-R")==0){
                if(argc==2){
                    path=".";
                    ls_R(path);
                }else{
                    for(int i=2;i<argc;i++){
                        path=argv[i];
                        ls_R(path);
                        printf("\n");
                    }
                }
            }else if(strcmp(argv[1],"-l")==0){
                if(argc==2){
                    path=".";
                    ls_l(path);
                }else{
                    for(int i=2;i<argc;i++){
                        path=argv[i];
                        ls_l(path);
                        printf("\n");
                    }
                }
            }
            else if(strcmp(argv[1],"-lR")==0){
                if(argc==2){
                    path=".";
                    ls_lR(path);
                }else{
                    for(int i=2;i<argc;i++){
                        path=argv[i];
                        ls_lR(path);
                        printf("\n");
                    }
                }
            }
            exit(0);
        }
    }
}
