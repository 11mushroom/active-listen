#ifndef MSOCK_H
#define MSOCK_H

//======= structure for messages =======
#include <algorithm>
#include <cstdint>
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
Msock create(int port);

//=========function for creating Msock for client==========
Msock create(char* ip, int port);

//=========Bind function for Msock==========
int bind(Msock* sock);

//=========Listen function for Msock==========
int listen(Msock* sock);

//=========Accept function for Msock==========
int accept(Msock* sock);

//=========Connect function for Msock==========
int connect(Msock* sock);

//=========Send function for Msock==========
int send(Msock* sock, char* mess, uint64_t len);

//=========Recieve function for Msock==========
Mess recv(Msock* sock, uint64_t size);

//=========Recieve function using pointer, for Msock==========
void recv(Mess* buff, Msock* sock, uint64_t size);

//=========Close function for Msock==========
void close(Msock* sock);

//}

#endif
