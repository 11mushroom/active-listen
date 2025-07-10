#include <algorithm>
#include <cstdint>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>


//======= structure for messages =======
struct Mess {
  char* data;
  uint64_t data_len=0;

  Mess(){
    this->data=new char[1024+1];
    this->data_len=1024;
  }

  Mess (uint64_t size) {
    if(size>=0){
      this->data = new char[size+1];
      this->data_len=size;
    }
  }

  //===destructor===
  ~Mess(){
    delete[] data;
  }

  //===copy constructor===
  Mess(const Mess& other){
    data=new char[other.data_len+1];
    data_len=other.data_len;
    std::copy(other.data, other.data + other.data_len, data);
  }

  //===copy assign operator===
  Mess& operator=(const Mess& other){
    if (this != &other) {
      delete[] data;
      data=new char[other.data_len+1];
      data_len=other.data_len;
      std::copy(other.data, other.data + other.data_len, data);
    }
    return *this;
  }

  //===move constructor===
  Mess(Mess&& other) noexcept {
    data=other.data;
    data_len=other.data_len;
    other.data=nullptr;
  }

  //===move assign operator===
  Mess& operator=(Mess&& other) noexcept {
    if (this != &other) {
      delete[] data;
      data = other.data;
      data_len=other.data_len;

      other.data_len=0;
      other.data=nullptr;
    }
    return *this;
  }

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
int send(Msock* sock, char* mess, uint64_t len){
  return send((sock->cl ? sock->sockfd : sock->clientfd), mess, len, 0);
};

//=========Recieve function for Msock==========
Mess recv(Msock* sock, uint64_t size){
  Mess res(size);
  //char* buff= new char[size];
  res.data_len=read((sock->cl ? sock->sockfd : sock->clientfd), res.data, size);
  if (res.data_len < 0) {
    std::cerr << "can't recieve data!" << std::endl;
  }
  return res;
};

//=========Recieve function using pointer, for Msock==========
void recv(Mess* buff, Msock* sock, uint64_t size){
  //char* buff= new char[size];
  delete[] buff->data;
  buff->data=new char[size+1];
  buff->data_len=read((sock->cl ? sock->sockfd : sock->clientfd), buff->data, size);
  if (buff->data_len < 0) {
    std::cerr << "can't recieve data!" << std::endl;
  }
};

//=========Close function for Msock==========
void close(Msock* sock){
  close(sock->sockfd);
  if(!sock->cl) close(sock->clientfd);
};
//}
