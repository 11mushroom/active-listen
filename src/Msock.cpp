#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>


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
