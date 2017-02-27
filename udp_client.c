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
  unsigned short seq = 0;
  in_port_t port;
  socklen_t sktlen;
  int count;
  struct sockaddr_in skt;
  char *ip;
  struct msg_echo send_struct; 
  struct msg_echo recv_struct;

  ip = argv[1];
  port = atoi(argv[2]);

  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  //ip = "0.0.0.0";
  memset(&skt, 0, sizeof skt);
  skt.sin_addr.s_addr = inet_addr(ip);
  skt.sin_port = htons(port);
  skt.sin_family = AF_INET;

  send_struct.seq = seq;

  for (;;) {
  printf("enter message\n");
  fgets(send_struct.msg, 31, stdin);
  send_struct.seq = seq;
  
  printf("send seq = %d\n", seq);
  if ((count = sendto(s, &send_struct, sizeof(struct msg_echo), 0, (struct sockaddr *)&skt, sizeof(skt))) < 0) {
    perror("sendto");
    close(s);
    exit(1);
  }
  sktlen = sizeof skt;
  if ((count = recvfrom(s, &recv_struct, sizeof(struct msg_echo), 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
    perror("recvfrom");
    close(s);
    exit(1);
  }
  seq = (recv_struct.seq)++;
  printf("recevived following sentence %s, sequence number = %d\n", recv_struct.msg, recv_struct.seq);
  if (seq == 10) {
	  exit(0);
  }
  }
    
  close(s);
  return 0;
}
