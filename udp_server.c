#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

struct msg_echo {
	unsigned short seq;
	unsigned short reserve;
	char msg[32];
};

int main(int argc, char *argv[])
{
  int s;
  in_port_t myport;
  socklen_t sktlen;
  struct sockaddr_in myskt;
  struct sockaddr_in skt;
  char recv_buf[100];
  int count;
  struct msg_echo send_struct;
  struct msg_echo recv_struct;

  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  myport = atoi(argv[1]);
  memset(&myskt, 0, sizeof myskt);
  myskt.sin_family = AF_INET;
  myskt.sin_port = htons(myport);
  myskt.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(s, (struct sockaddr *)&myskt, sizeof myskt) < 0) {
    perror("bind");
    exit(1);
  }
  
  for (;;) {
    sktlen = sizeof skt;
    if ((count = recvfrom(s, &recv_struct, sizeof(struct msg_echo), 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
      perror("recvfrom");
      exit(1);
    }
    //printf("partner's ip address: %s\n", inet_ntoa(skt.sin_addr));
    //printf("partner's port number: %hu\n", ntohs(skt.sin_port));
    printf("received message: %s, sequence number = %d\n", recv_struct.msg, recv_struct.seq);
    strcpy(send_struct.msg, recv_struct.msg);
    send_struct.seq = recv_struct.seq + 1;
    if ((count = sendto(s, &send_struct, sizeof(struct msg_echo), 0, (struct sockaddr *) &skt, sizeof skt)) < 0) {
      perror("sendto");
      exit(1);
    }
  }
  close(s);
  return 0;
}


