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

/*
//======= structure for messages =======
struct Mess {
  char data[1024]={0};
  int data_len=0;
};


//{
//======== structure for sockets ========
struct Msock {
  bool cl;
  bool* conn;
  int sockfd;
  sockaddr_in sock_addr;
  socklen_t addrlen;
  int clientfd;
  Msock() {
    this->addrlen = sizeof(this->sock_addr);
  };
};

//=========function for creating Msock for server==========
Msock create(int port){
  Msock crsock;
  crsock.cl = false;
  crsock.sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(crsock.sockfd >= 0){
    crsock.sock_addr.sin_family = AF_INET;
    crsock.sock_addr.sin_port = htons(port);
    crsock.sock_addr.sin_addr.s_addr = INADDR_ANY;
  };
  return crsock;
};

//=========function for creating Msock for client==========
Msock create(char* ip, int port){
  Msock crsock;
  crsock.cl = true;
  crsock.sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(crsock.sockfd >= 0){
    crsock.sock_addr.sin_family = AF_INET;
    crsock.sock_addr.sin_port = htons(port);
    crsock.sock_addr.sin_addr.s_addr = inet_addr(ip);
  };
  return crsock;
};

//=========Bind function for Msock==========
int bind(Msock* sock){
  if((bind(sock->sockfd, (struct sockaddr *)&(sock->sock_addr), sock->addrlen)<0)){
    std::cerr << "bind fail" << std::endl;
    return -1;
  };
  return 0;
};

//=========Listen function for Msock==========
int listen(Msock* sock){
  if((listen(sock->sockfd, 1))<0){
    std::cerr << "listen error" << std::endl;
    return -1;
  };
  return 0;
}

//=========Accept function for Msock==========
int accept(Msock* sock){
  if((sock->clientfd=accept(sock->sockfd, (struct sockaddr *)&(sock->sock_addr), &(sock->addrlen)))<0){
    std::cerr << "accepting fail" << std::endl;
    return -1;
  };
  return 0;
};

//=========Connect function for Msock==========
int connect(Msock* sock){
    if (connect(sock->sockfd, (struct sockaddr*)&(sock->sock_addr), sock->addrlen) < 0) {
      std::cerr << "can't connect to server!" << std::endl;
      return -1;
    } else {
      return 0;
    };
};

//=========Send function for Msock==========
int send(Msock* sock, char* mess, int len){
  return send((sock->cl ? sock->sockfd : sock->clientfd), mess, len, 0);
};

//=========Recieve function for Msock==========
Mess recv(Msock* sock, int size){
  Mess res;
  //char* buff= new char[size];
  res.data_len=read((sock->cl ? sock->sockfd : sock->clientfd), res.data, 1024);
  if (res.data_len < 0) {
    std::cerr << "can't recieve data!" << std::endl;
  }
  return res;
};

//=========Close function for Msock==========
void close(Msock* sock){
  close(sock->sockfd);
  if(!sock->cl) close(sock->clientfd);
};
//}
*/

//{
/*
//=====pServer class=====
class pServer{
public:
  int serv_desc, client_desc;
  struct sockaddr_in addr;
  socklen_t addrlen;

  //=====Constructor=====
  pServer(int s_desc, struct sockaddr_in addr, socklen_t addrlen){
    this->serv_desc=s_desc;
    this->addr=addr;
    this->addrlen=addrlen;
  }

  //=====Start listening socket=====
  int Lis(){
    if((listen(this->serv_desc, 1))<0){
      std::cerr << "listen error" << std::endl;
      return -1;
    };
    return 0;
  };

  //=====accepting/waiting client socket connection=====
  int accCl(){
    if((this->client_desc=accept(this->serv_desc, (struct sockaddr *)&(this->addr), &addrlen))<0){
      std::cerr << "accepting fail" << std::endl;
      return -1;
    };
    return 0;
  };

};
*/
//{
//=====Server class=====
class Server{
public:
  int serv_desc, client_desc;
  //ssize_t valread;
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);

  //=====Constructor=====
  Server(int port){
    if((this->serv_desc=socket(AF_INET, SOCK_STREAM, 0))<0){
      std::cerr << "can't create sock" << std::endl;
    };

    this->addr.sin_family=AF_INET;
    this->addr.sin_addr.s_addr=INADDR_ANY;
    this->addr.sin_port=htons(port);

    if((bind(this->serv_desc, (struct sockaddr *)&(this->addr), sizeof(this->addr)))<0){
      std::cerr << "bind fail" << std::endl;
    };

  };

  //=====Start listening socket=====
  int Lis(){
    if((listen(this->serv_desc, 1))<0){
      std::cerr << "listen error" << std::endl;
      return -1;
    };
    return 0;
  };

  //=====accepting/waiting client socket connection=====
  int accCl(){
    if((this->client_desc=accept(this->serv_desc, (struct sockaddr *)&(this->addr), &addrlen))<0){
      std::cerr << "accepting fail" << std::endl;
      return -1;
    };
    return 0;
  };
  
  //=====receive data from client=====
  Mess Recv(int size){
    Mess res;
    //char* buff= new char[size];
    res.data_len=read(this->client_desc, res.data, 1024);
    return res;

  };

  //=====send data to client=====
  int Send(char* mess, int len){
    return send(this->client_desc, mess, len, 0);

  };

  //=====close client and server sockets=====
  void Close(){
    close(this->client_desc);
    close(this->serv_desc);
  };

};

//}


//=====client class=====
class Client{
public:
  int socket_desc;
  bool doing=true;
  struct sockaddr_in server_addr;
  bool conn=false;


  Client(char* ip, int port){

    //=====create socket====={
    this->socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket_desc == -1) {
      std::cerr << "socket creation failed!" << std::endl;
      //return -1;
    };
    //}


    //=====configure server address====={
    this->server_addr.sin_family = AF_INET; // IPv4
    this->server_addr.sin_addr.s_addr = inet_addr(ip); // serv ip
    this->server_addr.sin_port = htons(port);//serv port
    //}
  };

  //=====connecting to server=====
  int Connect(){
    if (connect(this->socket_desc, (struct sockaddr*)&(this->server_addr), sizeof(this->server_addr)) < 0) {
      std::cerr << "can't connect to server!" << std::endl;
      return -1;
    } else {
      this->conn=true;
      return 0;
    };
  };

  //=====send data=====
  int Send(char* mess, int len){
    if (send(this->socket_desc, mess, len, 0) < 0) {
      std::cerr << "can't send data!" << std::endl;
      return -1;
    } else {
      return 0;
    };
  };


  //=====receive data=====
  Mess Recv(int size){
    //size_t* leng=new size_t(1024);
    //char* server_reply = new char[size];
    Mess res;
    // Отримання даних
    res.data_len=recv(this->socket_desc, res.data, 1024, 0);
    if (res.data_len < 0) {
      std::cerr << "can't recieve data!" << std::endl;
    }
    //delete leng;
    return res;
  };

  //=====close socket=====
  void Close(){
    close(this->socket_desc);
    this->conn=false;
  };

};


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
  
  //std::string* inp=new std::string("");
  //int it=0;
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
  /*cl.Connect();
  while (true){
    std::cin >> *inp;
    if(*inp==":q"){break;}
    cl.Send(tochr(*inp));
    std::cout << it << cl.Recv() << std::endl;
    it++;
  };
  delete inp;
  cl.Send(tochr("end"));
  cl.Close();*/
  std::thread T(Transfer, connections);
  //std::thread C(Connector, connections);
  //C.detach();
  T.join();


  return 0;
};
