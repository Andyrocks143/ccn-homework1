#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 5432
#define MAX_LINE 80

int main(int argc, char * argv[])
{
	FILE *fp;
	struct hostent *hp;
	struct sockaddr_in sin;
	struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec=0;
	char *host;
	char *fname;
	char second_countasc;
	char buf[MAX_LINE];
	char buff[MAX_LINE];
	char ACKbuf[MAX_LINE];
	int len;
	int loop;
	int s;
	int slen;
	int second_count;
	
	if (argc==3) {
		host = argv[1];
		fname= argv[2];
	}
	else {
		fprintf(stderr, "Usage: ./client_udp host filename\n");
		exit(1);
	}
	hp = gethostbyname(host);
	
	if (!hp) {
		fprintf(stderr, "Unknown host: %s\n", host);
		exit(1);
	}

	fp = fopen(fname, "r");
	
	if (fp==NULL){
		fprintf(stderr, "Can't open file: %s\n", fname);
		exit(1);
	}

	
	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(SERVER_PORT);

	/* active open */
	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}

	socklen_t sock_len= sizeof sin;
	if (setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv)) < 0){
			perror("ERROOORRRR");
		}
	
	second_count=0;
	loop=0;

	/* main loop: get and send lines of text */
	while(fgets(buf, 80,fp) != NULL){
			
	while(loop==0){	
		slen = strlen(buf);
		second_countasc=second_count;
		buf[0]=second_countasc;
		buf[slen+2] ='\0';
		if(sendto(s, buf, slen+1, 0, (struct sockaddr *)&sin, sock_len)<0){
			perror("SendTo Error\n");
			//exit(1);	
		}

		//*Check for ACK 	
	
		len=recvfrom(s, ACKbuf, sizeof(ACKbuf), 0, (struct sockaddr *)&sin, &sock_len);
		if(len>0){
		second_count=second_count+1;
		loop=1;
		}
				
	}
		loop=0;
		printf("Packet being sent %d \n",second_count);
	}
	*buf = 0x02;	
        if(sendto(s, buf, 1, 0, (struct sockaddr *)&sin, sock_len)<0){
		perror("SendTo Error\n");
		exit(1);
	}
	fclose(fp);
}
