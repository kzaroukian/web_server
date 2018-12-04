#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <sys/select.h>

//Project 4  by Kaylin Zaroukian

fd_set sockets;
//char* filename2;
// FILE* file;
int using_file;

void write_to_file(char* filename, char* summary) {
  FILE* file;
  file  = fopen(filename, "w");
  if (file == NULL) {
    printf("Error opening or creating file using stdout instead\n");
    memcpy(filename,"",strlen(filename));
    using_file=0;
    printf("%s\n", summary);
  } else {
    // write to file
    fprintf(file,"%s\n",summary);
  }

}

int sendHTTPresoponse(char* holder, int v, char* version, char* path, int request_code, char* file_ext, int socket) {
    char summary[5000];
    memcpy(summary, "\nREQUEST\n", 9);
    memcpy(summary+8,holder,strlen(holder));
    int sum_size = 8 + strlen(holder);

    time_t  current_time  =  time(NULL);
    struct tm tm = *localtime(&current_time);
    char current_date[37];
    char date[1000];
    memcpy(current_date, "Date: ", 6);
    strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    memcpy(current_date+ 6, date, strlen(date));
    memcpy(current_date+ 6+strlen(date) ,"\r\n",2);

    struct stat last_modified_time;
    stat(path,&last_modified_time);
    char modified_date[100];
    struct tm file_tm = *localtime(&last_modified_time.st_mtime);

    char* first_split;
    char* banana_split;
    char data[40];
    first_split = strstr(holder, "If-Modified-Since");
    if (first_split != NULL) {

      // get position
      char* t1= strtok(holder, "\r\n");

      char* t2 = strtok(NULL,"\r\n");

      char* t3 = strtok(NULL,"\r\n");

      char* t4  = strtok(NULL,"\r\n");

      char* t5  =  strtok(NULL,"\r\n");

      char* t6  = strtok(NULL,"\r\n");


      char* t7 = strtok(NULL,"\r\n");


      char* t8 = strtok(NULL,"\r\n");

      banana_split = strtok(NULL,"\r\n");

      memcpy(data, banana_split+17, 29);


      struct tm mod_tm = {0};
      strptime(data, "%a, %d %b %Y %H:%M:%S %Z", &mod_tm);

      time_t date1 =  mktime(&mod_tm);
      time_t date2  = mktime(&file_tm);

      if (difftime(date1, date2) == 0) {
        request_code  = 304;
      }
      printf("%s\n",data );
      printf("%s\n",modified_date );
      printf("date comp %d\n",strncmp(data,modified_date,29) );
      if(strncmp(data,date,29)<=0) {
        request_code = 304;
      }


    }

    strftime(modified_date, sizeof modified_date, "%a, %d %b %Y %H:%M:%S %Z", &file_tm);


    char msg_to_send[100000];
    memcpy(summary+sum_size,"\nRESPONSE\n",10);

      char r_code[9];
      char final_val[19];
      char* req_code = "200 OK";
      memcpy(r_code, version, strlen(version));
      int version_len = strlen(version);

      char code[10];

      memcpy(code,req_code, 6);

      memcpy(final_val,r_code,9);
      memcpy(final_val+9,code,10);


      char header_last_modified[48];
      memcpy(header_last_modified, "Last-Modified: ",15);
      memcpy(header_last_modified+15, modified_date, strlen(modified_date));
      memcpy(header_last_modified + strlen(modified_date)+15,"\r\n",2);

      char cType[50];
      memcpy(cType, "Content-Type: ", 20);

      if (strncmp(file_ext, "html", 4) == 0) {

        memcpy(cType+14,"text/html\r\n",11);
      } else if(strncmp(file_ext, "txt",3) ==  0) {
        memcpy(cType+14, "text/plain\r\n", 12);
      } else if(strncmp(file_ext, "jpg", 3) == 0) {
        memcpy(cType+14, "image/jpeg\r\n", 12);
      } else if (strncmp(file_ext, "pdf", 3) == 0)  {
        memcpy(cType+14, "application/pdf\r\n",17);
      }

      // get content_length
      FILE *f = fopen(path, "r");
      fseek(f, 0, SEEK_END);
      unsigned int path_length = ftell(f);
      fseek(f, 0, SEEK_SET);
      char* file_size[16];
      sprintf(file_size, "%d", path_length);
      char content_length[25];
      memcpy(content_length, "Content-Length: ", 16);
      memcpy(content_length + 16,file_size,strlen(file_size));
      memcpy(content_length+strlen(file_size)+16, "\r\n",2);

      char contents[path_length];
      fread (contents, 1, path_length, f);

      char connection_type[24];
      if  (request_code  == 200  ||  request_code  == 304) {
        memcpy(connection_type, "Connection: keep-alive\r\n",24);

      }  else {
        memcpy(connection_type, "Connection: close\r\n",24);

      }


      // now lets put together the response
      if (request_code == 200 || request_code ==  304) {

      int  mc_length;
      if (request_code == 200) {
        memcpy(msg_to_send,"HTTP/1.1 200 OK\r\n", 17);
        mc_length = 17;

      } else {
        memcpy(msg_to_send,"HTTP/1.1 304 Not Modified\r\n", 27);
        mc_length = 27;
      }
      memcpy(msg_to_send+mc_length, current_date, strlen(current_date));
      mc_length += strlen(current_date);
      memcpy(msg_to_send+mc_length, header_last_modified, strlen(header_last_modified));
      mc_length +=  strlen(header_last_modified);
      memcpy(msg_to_send+mc_length, content_length, strlen(content_length));
      mc_length += strlen(content_length);
      memcpy(msg_to_send+mc_length, cType, strlen(cType));
      mc_length += strlen(cType);
      memcpy(msg_to_send+mc_length,  connection_type, strlen(connection_type));
      mc_length += strlen(connection_type);
      memcpy(msg_to_send+mc_length,"\r\n",2);
      mc_length+=2;
      memcpy(summary+sum_size+10,msg_to_send,strlen(msg_to_send));
      memcpy(msg_to_send+mc_length,contents,path_length);
      printf("%s\n", msg_to_send);

      int d = send(socket, msg_to_send, strlen(msg_to_send)+path_length,  0);
    } else {
      if (request_code  ==  404) {
        char to_send[1000];
        int mc_length =  26;
        memcpy(to_send,"HTTP/1.1 404 NO RESPONSE\r\n", 26);
        memcpy(to_send+mc_length, current_date, strlen(current_date));
        mc_length += strlen(current_date);

        memcpy(to_send+mc_length, header_last_modified, strlen(header_last_modified));
        mc_length +=  strlen(header_last_modified);
        memcpy(to_send+mc_length, content_length, strlen(content_length)-2);
        mc_length += strlen(content_length)-2;
        memcpy(to_send+mc_length, cType, strlen(cType));
        mc_length += strlen(cType);
        memcpy(to_send+mc_length, connection_type, strlen(connection_type));
        mc_length += strlen(connection_type);
        memcpy(to_send+mc_length,"\r\n",2);
        memcpy(summary+sum_size+10,to_send,strlen(to_send));

        mc_length += 2;
        memcpy(to_send+mc_length, contents, path_length);

        int d = send(socket, to_send, strlen(to_send)+path_length,  0);
        close(socket);
        FD_CLR(socket, &sockets);

      } else if (request_code  ==  501) {
        char to_send_2[1000];
        int mc_length = 30;
        memcpy(to_send_2,"HTTP/1.1 501 NOT IMPLEMENTED\r\n", 30);
        memcpy(to_send_2+mc_length, current_date, strlen(current_date));
        mc_length += strlen(current_date);

        memcpy(to_send_2+mc_length, header_last_modified, strlen(header_last_modified));
        mc_length +=  strlen(header_last_modified);
        memcpy(to_send_2+mc_length, content_length, strlen(content_length)-1);
        mc_length += strlen(content_length)-1;
        memcpy(to_send_2+mc_length, cType, strlen(cType));
        mc_length += strlen(cType);
        memcpy(to_send_2+mc_length, connection_type, strlen(connection_type));
        mc_length += strlen(connection_type);
        memcpy(to_send_2+mc_length,"\r\n",2);
        mc_length += 2;
        memcpy(summary+sum_size+10,to_send_2,strlen(to_send_2));

        memcpy(to_send_2+mc_length, contents, path_length);
        int d = send(socket, to_send_2, strlen(to_send_2),  0);

        close(socket);
        FD_CLR(socket, &sockets);
      }
    }

    if (using_file == 1) {

      //fwrite(summary,1, sizeof(summary),file);

    } else {
      printf("%s\n", summary);
    }
    return 0;
}

int main(int argc, char** argv) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		printf("There was an error creating the socket\n");
		return 1;
	}

  // specify port
  char port[5000];
  memcpy(port,"8080",5000);

  char path[5000];
  // sets path to current directory by default
  getcwd(path, sizeof(path));
  using_file=0;


  //FILE* file;
  char filename[5000];

  struct timeval timeout;
	timeout.tv_sec = 20;
	timeout.tv_usec = 0;

  int opt = 0;
  while((opt = getopt(argc, argv, "p:d:l:")) !=  -1) {
    switch(opt) {
      case 'p':
        memcpy(port, optarg, sizeof(optarg));
        break;
      case 'd':
        memset(path,0,5000);
        memcpy(path,optarg,sizeof(optarg));
        break;
      case 'l':
        // do something
        // we  will wait  on  this one
        // will do something for file
        memcpy(filename,optarg,sizeof(optarg));
        break;
      case '?':
        // do something
        // argument can be ignored so we can just keep going
        break;
      default:
        // do something
        // memcpy(port,"8080",5000);
        // getcwd(path, sizeof(path));
        break;
    }
  }
  printf("PORT: %s, Path: %s,  File Name: %s\n",port,  path, filename );

  int serverport = atoi(port);
  if (serverport < 70  || serverport > 65600) {
    printf("Error cannot use this port, resetting to default of 8080\n");
    serverport = 8080;
    memcpy(port,"8080",4);
  }
   if (access(path, F_OK) == -1) {
    printf("Invalid path, resetting to server home directory\n");
    getcwd(path, sizeof(path));
  }
   if(strlen(filename) > 1) {
     //using_file = 1;
     //memcpy(filename2,filename,strlen(filename));
  } else {
    using_file = 0;
  }
  using_file= 0;

	struct sockaddr_in serveraddr,clientaddr;
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(serverport);
	serveraddr.sin_addr.s_addr=INADDR_ANY;

  int b = bind(sockfd, (struct sockaddr*)&serveraddr,sizeof(serveraddr));
  if(b < 0) {
    printf("Bind error\n");
    return 3;
  }
  // listen for incoming clients on this port, 10 acts as a backlog
  listen(sockfd,10);

  FD_ZERO(&sockets);
  FD_SET(sockfd, &sockets);

  while(1) {

    fd_set tmp_set=sockets;
    int s = select(FD_SETSIZE, &tmp_set, NULL, NULL, &timeout);

    int i;
    for(i=0;i<FD_SETSIZE;i++) {
      if (s < 1)  {
        printf("timeout\n");
        close(i);
        FD_CLR(i, &sockets);
      }
      char buf[5000];
      if(FD_ISSET(i, &tmp_set)) {
        if (i==sockfd) {
          int len = sizeof(clientaddr);
          // accepting a client socket
          int clientsocket = accept(sockfd, (struct sockaddr*)&clientaddr,&len);
          // add client address to struct
          FD_SET(clientsocket, &sockets);
        } else {

          // we are receiving from the client

          int val = recv(i,buf,5000,0);

          char holder[5000];
          memcpy(holder,buf,5000);
          if(strncmp(buf,"POST", 4) == 0) {

            sendHTTPresoponse(holder, 1,"", "/Users/kaylinzaroukian/cis457/cis457-project4/501err.html", 501, "html",i);

          }

          if (strncmp(buf,"GET", 3) == 0) {

            // we received a get request
           // now parse request into tokens
           char http_path[1000]  =  {0};
           char *req_path =  {0};
           char *http_version =  {0};
           char format_http[9]  =  {0};
           char *pre_file_extension =  {0};
           char *file_extension   =  {0};
           char* ignore_me  =  {0};
           char temp_http[7]  =  {0};
           char temp_file_ext[5] =  {0};
           char another_test[10]   =  {0};
           char og_path[100]  =  {0};
           req_path = strtok(buf+3," ");

           int path_size = strlen(req_path);
           memcpy(og_path,req_path,path_size);

           http_version = strtok(NULL, " ");

           if (http_version !=  NULL) {

             ignore_me = strtok(NULL, "/r/n");

             memcpy(format_http,http_version,9);


             pre_file_extension = strtok(req_path, ".");
             file_extension = strtok(NULL,"");

           } else {

             continue;
           }

           if (strncmp(http_version, "HTTP/1.1",8) == 0) {

             memcpy(http_path, path, strlen(path));

             if (strlen(req_path) <= 1 || strlen(file_extension) <=1) {
               memcpy(http_path+strlen(path),"/index.html",11);
               memcpy(temp_file_ext,"html",4);

             } else {

               memcpy(http_path+strlen(path),og_path,strlen(og_path));
               memcpy(temp_file_ext,file_extension,strlen(file_extension));
             }

             if (access(http_path, F_OK) != -1) {


               int request_code = 200;
               sendHTTPresoponse(holder,1,"", http_path, 200, temp_file_ext,i);

             }  else  {

               sendHTTPresoponse(holder, 1,"", "/Users/kaylinzaroukian/cis457/cis457-project4/404err.html", 404, "html",i);
               //404 error file not found\
             }
           }

          }  else {
            printf("NOT GET\n");
            //sendHTTPresoponse(holder, 1,"", "/Users/kaylinzaroukian/cis457/cis457-project4/501err.html", 501, "html",i);

          }

          // FD_CLR(i, &sockets);
        }
      }


    }


  }
  }
  return 0;

}
