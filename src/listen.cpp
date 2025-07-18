#include <cstdint>
#include <ios>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
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

void rprint(char** out, char* bytes, int len, char* pref=empty){
  int ed_len=(len*4);
  char* edited=new char[ed_len+1];
  int i=0;
  int a=0;

  while(i<len){
    if(((uint8_t)bytes[i])>31 && ((uint8_t)bytes[i])<127){
      edited[a]=bytes[i];
    }else{
      edited[a]='\\';
      a++;
      edited[a]='x';
      a++;
      edited[a]=hex[(bytes[i]>>4)&15];
      a++;
      edited[a]=hex[bytes[i]&15];
    };
    i++;
    a++;
  };
  delete[] *out;
  *out=new char[a+strlen(pref)+1];
  sprintf(*out,"%s%s",pref,edited);
  delete[] edited;
};

void rprint(char** out, char* bytes, int len, const char* pr){
  rprint(out, bytes,len,const_cast<char*>(pr));
};

void print(char* bytes, int len, char* pref=empty){
  char* s=nullptr;
  rprint(&s, bytes,len,pref);
  printf("%s\n", s);
  delete[] s;
};

void print(char* bytes, int len, const char* pr){
  char* s=nullptr;
  rprint(&s, bytes,len,pr);
  printf("%s\n", s);
  delete[] s;
};
//}


//structure for storing pointers to connections
struct Conn {
  Msock client;
  Msock serv;
  bool* conn_cl;
  bool* conn_serv;
  bool logging;
  bool stdout_logs;
  std::ofstream *cl_file;
  std::ofstream *serv_file;
};


//from server to client
void Connector(Conn c){
  Mess smess;
  while(!*(c.conn_cl)){};
  if(*(c.conn_cl)){
    //char* sdata=new char[1024];
    recv(&smess, &(c.client),1024);
    char* buff=nullptr;

    while(smess.data_len>0 && *(c.conn_serv) && *(c.conn_cl)){
      if(send(&(c.serv), smess.data, smess.data_len) < 0){
        *(c.conn_cl)=false;
        break;
      };

      if(c.stdout_logs){
        rprint(&buff, smess.data, smess.data_len,"[server]");
        printf("%s\n",buff);
      }
      /*if(find(smess.data, smess.data_len, "\x04ping", 5)){
        c.client->Send(const_cast<char*>("\x0e\x00R\x01\x00\x03Say\x04pong"), 14);
        printf("%s\n",buff);
      };*/
      if(c.logging){
        c.serv_file->write(smess.data, smess.data_len);
      }

      recv(&smess, &(c.client),1024);
      if(smess.data_len<=0){
        *(c.conn_cl)=false;
        break;
      };
    };
    delete[] buff;

    close(&(c.client));
    std::cout << "[programm]connection with client has ended" << std::endl;
    *(c.conn_cl)=false;
    c.serv_file->close();

    
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
      recv(&cmess, &(c.serv),1024);
      char* buff=nullptr;

      while(cmess.data_len>0 && *(c.conn_serv) && *(c.conn_cl)){
        if(send(&(c.client), cmess.data, cmess.data_len) < 0) {
          *(c.conn_serv)=false;
          break;
        };

        if(c.stdout_logs){
          rprint(&buff, cmess.data, cmess.data_len,"[client]");
	        printf("%s\n",buff);
        }

        if(c.logging){
          c.cl_file->write(cmess.data, cmess.data_len);
        }

        recv(&cmess, &(c.serv), 1024);
        if(cmess.data_len <= 0){
          *(c.conn_serv)=false;
          break;
        };
      };
      delete[] buff;
      *(c.conn_serv)=false;
      printf("[programm]connection with server has ended\n");
      
    };
  };
  close(&(c.serv));
  *(c.conn_serv)=false;
  while(*(c.conn_cl)){};

  c.cl_file->close();

};

int main(int argc, char** argv){

  if(argc < 5){
    return -1;
  };

  uint32_t flags=std::stoi(argv[1]);
  bool logging=((flags&0b01)!= 0);
  bool stdout_logs=((flags&0b10)!=0);


  Msock cl = create(argv[3], std::stoi(argv[4]));
  Msock ser = create(std::stoi(argv[2]));
  std::ofstream cl_file((!(argc<7) ? argv[5] : "listen.cl.logs") , std::ios::binary|std::ios::app);
  std::ofstream serv_file(( !(argc<7) ? argv[6] : "listen.serv.logs") , std::ios::binary|std::ios::app);

  if(bind(&ser) < 0) return 1;

  bool conn_cl=false;
  bool conn_serv=false;

  Conn connections;

  connections.client=cl;
  connections.serv=ser;
  connections.conn_cl=&conn_cl;
  connections.conn_serv=&conn_serv;
  connections.logging=logging;
  connections.stdout_logs=stdout_logs;
  connections.cl_file=&cl_file;
  connections.serv_file=&serv_file;

  std::thread T(Transfer, connections);

  T.join();


  return 0;
};
