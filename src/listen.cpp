#include <iostream>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Msock.h"


int chlen(char* mess){
  int len=0;
  int i=0;
  while(mess[i]){
    i++;
    len++;
  };
  return len;
};


char* tochr(std::string in){
  int len=in.length();
  char res[len];
  for(int i=0;i<len;i++){
    res[i]=in[i];
  };
  return res;

};

//======function for searching {
bool find(char* full, int f_len, char* item, int i_len){
  bool found=false;
  for(int i = 0;i<f_len;i++){
    for(int a=0;(a<i_len) && ((i+a)<f_len);a++){
      bool eq=(full[i+a]==item[a]);
      if(!eq){
        break;
      }else if((a+1==i_len) && eq){
        found=true;
	return found;
      };
    };
  };

  return found;

};

bool find(char* full, int f_len, const char* item, int i_len){
  return find(full, f_len, const_cast<char*>(item), i_len);
};

bool find(char* full, char* item, int i_len){
  return find(full, sizeof(full), item, i_len);
};

bool find(char* full, const char* item, int i_len){
  return find(full, sizeof(full), item, i_len);
};
//}

//======function for correctly print data {
char* empty=const_cast<char*>("");
char hex[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

char* rprint(char* bytes, int len, char* pref=empty){
  int ed_len=(len*4);
  char edited[ed_len];
  int i=0;
  int a=0;

  while(i<len){
    if(bytes[i]>31 && bytes[i]<127){
      edited[a]=bytes[i];
    }else{
      edited[a]='\\';
      a++;
      edited[a]='x';
      a++;
      edited[a]=hex[(bytes[i]>>4)&((1<<4)-1)];
      a++;
      edited[a]=hex[bytes[i]&((1<<4)-1)];
    };
    i++;
    a++;
  };
  char res[ed_len+strlen(pref)];
  sprintf(res,"%s%s",pref,edited);
  //delete[] edited;

  return res;
};

char* rprint(char* bytes, int len, const char* pr){
  return rprint(bytes,len,const_cast<char*>(pr));
};

void print(char* bytes, int len, char* pref=empty){
  printf("%s\n",rprint(bytes,len,pref));
};

void print(char* bytes, int len, const char* pr){
  printf("%s\n",rprint(bytes,len,pr));

};
//}


//structure for storing pointers to connections
struct Conn {
  Msock client;
  Msock serv;
  bool* conn_cl;
  bool* conn_serv;
  std::ofstream *log_file;
};


//from server to client
void Connector(Conn c){
  Mess smess;
  while(!*(c.conn_cl)){};
  if(*(c.conn_cl)){
    //char* sdata=new char[1024];
    smess=recv(&(c.client),1024);

    while(smess.data_len>0 && *(c.conn_serv) && *(c.conn_cl)){
      if(send(&(c.serv), smess.data, smess.data_len) < 0){
        *(c.conn_cl)=false;
        break;
      };

      char* buff=rprint(smess.data, smess.data_len,"[server]");
      /*if(find(smess.data, smess.data_len, "\x04ping", 5)){
        c.client->Send(const_cast<char*>("\x0e\x00R\x01\x00\x03Say\x04pong"), 14);
        printf("%s\n",buff);
      };*/
      //*c.log_file << buff << '\n';
      printf("%s\n",buff);

      smess=recv(&(c.client),1024);
      if(smess.data_len<=0){
        *(c.conn_cl)=false;
        break;
      };
    };

    close(&(c.client));
    std::cout << "[programm]connection with client has ended" << std::endl;
    *(c.conn_cl)=false;

    
  };

};

//from client to server
void Transfer(Conn c){
  listen(&(c.serv));
  if (accept(&(c.serv))==0){
    *(c.conn_serv)=true;
    //c.serv.Send(tochr("success ^w^"));
    printf("[programm]connection with client established\n");

    if(connect(&(c.client))==0){
      *(c.conn_cl)=true;
      std::thread C(Connector, c);
      C.detach();

      printf("[programm]connection with server established\n");
      Mess cmess;
      cmess=recv(&(c.serv),1024);

      while(cmess.data_len>0 && *(c.conn_serv) && *(c.conn_cl)){
        if(send(&(c.client), cmess.data, cmess.data_len) < 0) {
          *(c.conn_serv)=false;
          break;
        };

        char* buff=rprint(cmess.data, cmess.data_len,"[client]");
	      printf("%s\n",buff);

        //*c.log_file << buff << '\n';

        cmess=recv(&(c.serv), 1024);
        if(cmess.data_len <= 0){
          *(c.conn_serv)=false;
          break;
        };
      };
      *(c.conn_serv)=false;
      printf("[programm]connection with server has ended\n");
      
    };
  };
  close(&(c.serv));
  *(c.conn_serv)=false;
  while(*(c.conn_cl)){};

  c.log_file->close();

};

int main(int argc, char** argv){

  if(argc < 4){
    return -1;
  };
  
  Msock cl = create(argv[2], std::stoi(argv[3]));
  Msock ser = create(std::stoi(argv[1]));
  std::ofstream file("logs");

  if(bind(&ser) < 0) return 1;

  bool conn_cl=false;
  bool conn_serv=false;

  Conn connections;

  connections.client=cl;
  connections.serv=ser;
  connections.conn_cl=&conn_cl;
  connections.conn_serv=&conn_serv;
  connections.log_file=&file;

  std::thread T(Transfer, connections);

  T.join();


  return 0;
};
