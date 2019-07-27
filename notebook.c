#include "notebook.h"

ssize_t readln (int fildes, void *buf, size_t nbyte){

  char *b = buf;
  ssize_t res=0;
  while(read(fildes,b,nbyte) > 0){
    res++;
    if(*b == '\n') return res;
    b++;
  }
  return res;
}
int strcopy(char* str, char* str2, int bits){
  //copia 'n' bits da str para str2
  //retorna quantas palavras existem
  int count = 1; //como existe uma linha, existe obrigatoriamente uma palavra
  while(bits>=0){
    if(str[bits] == ' ') count++;
    str2[bits] = str[bits];
    bits--;
  }
  return count;
}
int split(char *str, char** cmds){
  //copia todas as palavras de str para cmds excepto os espaços em branco
  //retorna o numero de palavras em cada linha
  char* pch;
  int k;

  pch = strtok (str," ");

  for (k=0; pch != NULL; k++){
    cmds[k] = malloc( PIPE_BUF );
    sprintf(cmds[k], "%s", pch);
    pch = strtok (NULL, " ");
  }

  return k;
}
int strToArray(char* buf, char** cmds, int n){
  //passa o conteudo da string do 'buf' para um array de strings/palavras (cmds)
  buf[n-1] = '\0';
  char linha[n-1];
  int words = strcopy(buf,linha,n-1);
  split(linha,cmds);
  return 0;
}
int processCmds(char** cmds,int cmdsLidos){
  int fd2[2];
  pipe(fd2);
  if(!fork()){
    dup2(fd2[1],1);
    close(fd2[1]);
    close(fd2[0]);
    if(execvp(cmds[1],&cmds[1]) == -1) _exit(1);
  }
  int status;
  wait(&status);
  if(WEXITSTATUS(status) == 1){write(1,"ERROR\n",6); exit(0);}
  close(fd2[1]);
  int fd = open("temp.txt", O_WRONLY| O_APPEND, 0666);
  int fd3 = open("outputs/lastOutput", O_RDWR| O_TRUNC, 0666);
  char name[16];
  sprintf(name,"outputs/%d",cmdsLidos);
  int fd4 = open(name,O_CREAT| O_RDWR| O_TRUNC, 0666);
  write(fd,">>>\n",4);
  char buf;
  while(read(fd2[0],&buf,1)>0){
    write(fd,&buf,1);
    write(fd3,&buf,1);
    write(fd4,&buf,1);
  }
  write(fd,"<<<\n",4);
  close(fd2[0]);
  close(fd3);
  close(fd4);
  close(fd);
  return 0;
}
int processPipedCmds(char **cmds, int cmdsLidos){
  int fd2[2];
  pipe(fd2);
  if(!fork()){
    dup2(fd2[1],1);
    close(fd2[1]);
    close(fd2[0]);
    int fdin = open("outputs/lastOutput", O_RDWR, 0666);
    dup2(fdin,0);
    close(fdin);
    if(execvp(cmds[1],&cmds[1]) == -1) _exit(1);
  }
  int status;
  wait(&status);
  if(WEXITSTATUS(status) == 1){write(1,"ERROR\n",6); exit(0);}
  close(fd2[1]);
  int fd = open("temp.txt", O_WRONLY| O_APPEND, 0666);
  int fd3 = open("outputs/lastOutput", O_RDWR|O_TRUNC, 0666);
  char name[16];
  sprintf(name,"outputs/%d",cmdsLidos);
  int fd4 = open(name,O_CREAT| O_RDWR| O_TRUNC, 0666);
  write(fd,">>>\n",4);
  char buf;
  while(read(fd2[0],&buf,1)>0){
    write(fd,&buf,1);
    write(fd3,&buf,1);
    write(fd4,&buf,1);
  }
  write(fd,"<<<\n",4);
  close(fd2[0]);
  close(fd3);
  close(fd4);
  close(fd);
  return 0;
}
int processNumPipedCmds(char** cmds, int cmdsLidos, int cmdAler){
  int fd2[2];
  pipe(fd2);
  if(!fork()){
    dup2(fd2[1],1);
    close(fd2[1]);
    close(fd2[0]);
    char name[16];
    sprintf(name,"outputs/%d",cmdAler);
    int fd = open(name,O_RDONLY);
    dup2(fd,0);
    close(fd);
    if(execvp(cmds[1],&cmds[1]) == -1) _exit(2);
  }
  int status;
  wait(&status);
  if(WEXITSTATUS(status) == 2) {write(1,"ERROR\n",6); exit(0);}
  close(fd2[1]);
  int fd = open("temp.txt", O_WRONLY| O_APPEND, 0666);
  int fd3 = open("outputs/lastOutput", O_RDWR| O_TRUNC, 0666);
  char name[16];
  sprintf(name,"outputs/%d",cmdsLidos);
  int fd4 = open(name,O_CREAT| O_RDWR| O_TRUNC, 0666);
  write(fd,">>>\n",4);
  char buf;
  while(read(fd2[0],&buf,1)){
    write(fd,&buf,1);
    write(fd3,&buf,1);
    write(fd4,&buf,1);
  }
  close(fd2[0]);
  close(fd3);
  close(fd4);
  write(fd,"<<<\n",4);
  close(fd);
  return 0;
}
int isEmpty(char* fileName){
  /*
    abre um ficheiro e vê se tem conteudo
    se nao tiver conteudo retorna 1
    se tiver conteudo retorna 0
  */
  int fd = open(fileName,O_RDONLY,0666);
  int flag=1;
  char buf;
  while(read(fd,&buf,1)>0){
    flag=0;
    break;
  }
  return flag;
}

int main(int argc, char* argv[]){
  int fd = open(argv[1], O_RDWR, 0666);
  int dir;
  dir = mkdir("outputs",0777);
  int fd_error = open("error.txt", O_CREAT| O_RDWR| O_TRUNC, 0666);
  dup2(fd_error,2);
  close(fd_error);
  int fd1 = open("temp.txt", O_CREAT| O_TRUNC, 0666);
  close(fd1);
  int fd2 = open("outputs/lastOutput", O_CREAT| O_TRUNC| O_RDWR,0666);
  close(fd2);
  char buf[512];
  int n;
  int cmdsLidos=0;
  int flag = 1;
  while((n = readln(fd,buf,1)) > 0){
    if(buf[0]=='>' && buf[1]=='>' && buf[2]=='>') flag=0;
    if(buf[0]=='<' && buf[1]=='<' && buf[2]=='<'){
      flag =1;
      continue;
    }
    if(flag == 0) continue;

    int fd3 = open("temp.txt", O_WRONLY| O_APPEND, 0666);
    write(fd3,buf,n);
    close(fd3);

    char **cmds = malloc( (sizeof(char *)) * PIPE_BUF);
    strToArray(buf,cmds,n);

    if(!strcmp(cmds[0],"$")){
      cmdsLidos++;
      processCmds(cmds,cmdsLidos);
    }
    if(!strcmp(cmds[0],"$|")){
      cmdsLidos++;
      processPipedCmds(cmds,cmdsLidos);
    }
    if(cmds[0][0]=='$' && isdigit(cmds[0][1]) && cmds[0][2] == '|'){
      cmdsLidos++;
      int cmdAler = cmdsLidos-atoi(&cmds[0][1]);
      if(cmdAler < 1){
        write(1,"ERROR\n",6);
        write(2,"ERRO: PUSH_BACK DEMASIADO GRANDE\n",32);
        exit(0);
      }
      processNumPipedCmds(cmds,cmdsLidos,cmdAler);
    }
  }
  if(isEmpty("error.txt")){
    fd = open(argv[1], O_WRONLY| O_TRUNC, 0666);
    fd1 = open("temp.txt", O_RDONLY, 0666);
    while((n = readln(fd1,buf,1)) > 0){
      write(fd,buf,n);
    }
    write(1,"SUCCESSFUL EXECUTION\n",21);
  }else write(1,"EXECUTION ERROR\n",16);
  close(fd);
  close(fd1);
  system("rm -r ./outputs temp.txt");
  return 0;
}
