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
	char second_countasc;
	char first_countasc;
	struct sockaddr_in sin;
        int len;
	int a;
        int s, i;
	int slen;
	int first_count;
	int second_count;
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
	first_count=0;
	while(1){
		len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &sock_len);
		second_countasc=buf[0];
		second_count=second_countasc; 
		printf("%d\n", second_count);
		printf("from sending side %s \n",buf);
		if(len == -1){
        	    perror("PError");
		}	
		else if(len == 1){
			if (buf[0] == 0x02){			
        	    		printf("Transmission Complete\n");
				break;
			}
        	    	else{
				perror("Error: Short packet\n");
			}
		}	
		else if(len > 1){
			//Send ACK
			if(first_count==second_count){
			buf[0]=' ';
			if(fputs((char *) buf, fp) < 1){
				printf("fputs() error\n");
			}
			first_count=first_count+1;
			if(sendto(s, "ACK", 1, 0, (struct sockaddr *)&sin, sock_len)<0){
			perror("SendTo Error\n");
			exit(1);	
			}
			}

			//Resend if same
			else if(first_count>second_count){
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
