#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>

#define SERVER_PORT 5432
#define MAX_LINE 256

int main(int argc, char * argv[])
{
	char *fname;
    char buf[MAX_LINE];	
char counter2;
	
	struct sockaddr_in sin;
        int length;
	int a;
        int s, i;
	int slen;
	int c1;
	int c2;
    struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec=0;
	char seq_num = 1; 
	FILE *fp;

        if (argc==2) {
                fname = argv[1];
        }
        else {
                fprintf(stderr, "usage: ./server_udp filename\n");
        	exit(1);
        }


        /* build address data structure */
        bzero((char *)&sin, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(SERVER_PORT);

        /* setup passive open */
        if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
              	perror("simplex-talk: socket");
                exit(1);
        }
        if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
              	perror("simplex-talk: bind");	
                exit(1);
        }

	socklen_t sock_len = sizeof sin;
	srandom(time(NULL));

	fp = fopen(fname, "w");
        if (fp==NULL){
                printf("Can't open file\n");
                exit(1);
        }

	if (setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv)) < 0){
			perror("PError");
		}
	a=0;
	c1=0;
	while(1){
		length = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &sock_len);
		counter2=buf[0];
		c2=counter; 
		//printf("%d\n", c2);
		//printf("from sending side %s \n",buf);
		if(length == -1){
        	    perror("PError");
		}	
		else if(length == 1){
			if (buf[0] == 0x03){			
        	    		printf("Transmission Complete\n");
				break;
			}
        	    	else{
				perror("Error: Short packet\n");
			}
		}	
		else if(length > 1){
			//Send ACK
			if(c1==c2){
			buf[0]=' ';
			if(fputs((char *) buf, fp) < 1){
				printf("fputs() error\n");
			}
			c1=c1+1;
			if(sendto(s, "ACK", 1, 0, (struct sockaddr *)&sin, sock_len)<0){
			perror("SendTo Error\n");
			exit(1);	
			}
			}

		

	else if(c1>c2){

			if(sendto(s, "ACK", 1, 0, (struct sockaddr *)&sin, sock_len)<0){
			perror("SendTo Error\n");
			exit(1);
			}
			}
		}	
        }
	fclose(fp);

	
close(s);
}