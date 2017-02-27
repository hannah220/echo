#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void execute(int s1);

struct msg_echo {
	unsigned short seq;
	unsigned short reserve;
	char msg[32];
};

int main(int argc, char *argv[])
{
  int s, s1;
  int port;
  int pid;
  struct sockaddr_in myskt;
  struct sockaddr_in skt;
  socklen_t sktlen;
  char buf[100];
  char msg[6] = "hello\n";
  int count;
  struct msg_echo send_struct;
  struct msg_echo recv_struct;
  
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  port = atoi(argv[1]);
  myskt.sin_port = htons(port);
  myskt.sin_addr.s_addr = htonl(INADDR_ANY);
  if ((bind(s, (struct sockaddr *)&myskt, sizeof myskt)) < 0) {
    perror("bind");
    exit(1);
  }
  if (listen(s, 5) < 0) {
    perror("listen");
    exit(1);
  }
  sktlen = sizeof skt;
  
  for (;;) {
    if ((s1 = accept(s, (struct sockaddr *)&skt, &sktlen)) < 0) {
      perror("accept");
      exit(1);
    }

    if ((pid = fork()) < 0) {
          perror("fork");
    }else if (pid == 0) {
          execute(s1);
    } else {
    close(s1);
    }
    }
  return 0;
}

void execute(int s1)
{
    struct msg_echo recv_struct;
    struct msg_echo send_struct;
    for (;;) {
      if ((recv(s1, &recv_struct, sizeof(struct msg_echo), 0)) < 0) {
	perror("recv");
	exit(1);
      }
    if (recv_struct.seq == 9) {
	    close(s1);
	    exit(0);
    }	    
      printf("received message: %s, sequence number = %d\n", recv_struct.msg, recv_struct.seq);
      if (strncmp(recv_struct.msg, "FIN", 3) == 0) {
	      close(s1);
	      exit(0);
      }
     send_struct.seq = recv_struct.seq + 1; 
      if ((send(s1, &send_struct, sizeof(struct msg_echo), 0)) < 0) {
	perror("send");
	exit(1);
      }
    }
}




