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
	int seq = 0;
	struct sockaddr_in skt;
	int count;
	int port = atoi(argv[2]);
	char *ip_buf;
	char msg_buf[100];
	char recv_buf[100];
	struct msg_echo send_struct;
	struct msg_echo recv_struct;
	
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	ip_buf = argv[1];
	memset(&skt, 0, sizeof skt);
	skt.sin_family = AF_INET;
	skt.sin_port = htons(port);
	skt.sin_addr.s_addr = inet_addr(ip_buf);

	if (connect(s, (struct sockaddr *)&skt, sizeof skt) < 0) {
	  perror("connect");
	  exit(1);
	}
	
	for (;;) {
	  printf("enter message\n");
	  fgets(msg_buf, 30, stdin);
 	  
          send_struct.seq = seq;	  
	  strcpy(send_struct.msg, msg_buf);
	    if ((count = send(s, &send_struct, sizeof(struct msg_echo), 0)) < 0) {
	      perror("send");
	      exit(1);
	    }
          if (strncmp(msg_buf, "FIN", 3) == 0) {
		  exit(0);
	  }
	  if ((count= recv(s, &recv_struct, sizeof(struct msg_echo), 0)) < 0) {
	    perror("recv");
	    exit(1);
	  }
	  seq = (recv_struct.seq)++;
	  printf("received message: %s, sequence number = %d\n", recv_struct.msg, recv_struct.seq);
	  if (seq == 10) {
		  exit(0);
	  }
	} 

	close(s);
	
	return 0;
}
