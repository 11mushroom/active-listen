#ifndef MSOCK_H
#define MSOCK_H

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
int send(Msock* sock, char* mess, int len);

//=========Recieve function for Msock==========
Mess recv(Msock* sock, int size);

//=========Close function for Msock==========
void close(Msock* sock);

//}

#endif
