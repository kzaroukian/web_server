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

int sendHTTPresoponse(char* version, char* path, int request_code, char* file_ext, int socket) {
    printf("entered send  response\n");
    time_t  current_time  =  time(NULL);
    struct tm tm = *localtime(&current_time);
    char current_date[50];
    char date[1000];
    memcpy(current_date, "Date: ", 6);
    //char* date = asctime(&tm);
    strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    memcpy(current_date+6, date, strlen(date));
    memcpy(current_date+6+strlen(date) , "\r\n",2);


    // may need to do something else
    //printf("current date: %s\n", date);

    // get the last time our file was last modified
    struct stat last_modified_time;
    stat(path,&last_modified_time);
    char modified_date[1000];
    struct tm file_tm = *localtime(&last_modified_time.st_mtime);
    //strftime(modified_date,100, "%")
    //modified_date = asctime(&file_tm);
    strftime(modified_date, sizeof modified_date, "%a, %d %b %Y %H:%M:%S %Z", &file_tm);

    printf("modified date  %s\n", modified_date);

    char msg_to_send[100000];
    printf("1\n");
    printf("version %s\n", version);
    printf("2\n");

    if (request_code == 200) {
      printf("entered\n");
      printf("v:  %s\n", version);
      char r_code[9];
      char final_val[19];
      char* req_code = "200 OK";
      memcpy(r_code, version, strlen(version));
      //strcpy(final_val, r_code);
      int version_len = strlen(version);
      printf("r %s\n", r_code);
      printf("%d\n", version_len);
      //memcpy(r_code+strlen(version)," 200 OK\r\n", 10s);
      char code[10];
      printf("1\n" );
      memcpy(code,req_code, 6);
      //strcpy(final_val[version_len],code);
      printf("code %s\n", code);

      if (version_len == 0 ) {
        printf("0\n");
        while (version_len<0) {
          //block to make sure thishappens  first
        }
      }
      printf("here\n");

      memcpy(final_val,r_code,9);
      memcpy(final_val+9,code,10);
      printf("starting r code\n");
      printf("r_code %s\n", r_code);
      printf("final  %s\n", final_val);
      printf("2\n");

      // char header_date[120];
      // memcpy(header_date, "Date: ",6);
      // memcpy(header_date+6, date, strlen(date));
      // memcpy(header_date+strlen(header_date),"\r\n",2);
      // printf("header_date %s\n", header_date);

      // char server[14] = {0};
      // memcpy(server,"Server: GVSU", 12);
      // printf("server %s\n", server);

      char header_last_modified[120];
      memcpy(header_last_modified, "Last-Modified: ",15);
      memcpy(header_last_modified+15, modified_date, strlen(modified_date));
      memcpy(header_last_modified+strlen(header_last_modified),"\r\n",2);
      printf("header_lm %s\n", header_last_modified);


      // TODO: get last modified
      // TODO: etag>??????
      char cType[50];
      memcpy(cType, "Content-Type: ", 20);
      printf("issue with  this memcpy\n");
      printf("%s\n", file_ext );
      if (strncmp(file_ext, "html", 4) == 0) {
        printf("html\n");
        memcpy(cType+14,"text/html\r\n",11);
      } else if(strncmp(file_ext, "txt",3) ==  0) {
        memcpy(cType+14, "text/plain\r\n", 12);
      } else if(strncmp(file_ext, "jpg", 3) == 0) {
        memcpy(cType+14, "image/jpeg\r\n", 12);
      } else if (strncmp(file_ext, "pdf", 3) == 0)  {
        memcpy(cType+14, "application/pdf\r\n",17);
      }
      printf("ct %s\n", cType);

      // memcpy(server,cType, strlen(cType));

      // get content_length
      FILE *f = fopen(path, "r");
      printf("file opened\n");
      fseek(f, 0, SEEK_END);
      unsigned int path_length = ftell(f);
      fseek(f, 0, SEEK_SET);
      printf("f seek done\n");
      char* file_size[16];
      //itoa(file_size,path_length,);
      sprintf(file_size, "%d", path_length);
      char content_length[50];
      memcpy(content_length, "Content-Length: ", 16);
      memcpy(content_length+16,file_size,strlen(file_size));
      memcpy(content_length+strlen(content_length), "\r\n",2);
      printf("f l %s\n", content_length);

      // will probably need to set connection type differentlybut for now  thisw ill work
      char connection_type[24];
      memcpy(connection_type, "Connection: keep-alive\r\n",24);
      printf("ct %s\n", connection_type);

      // now lets put together the response
      // int mc_length = strlen(r_code);
      int mc_length = 17;


      printf("Putting  it all together\n");
      // memcpy(msg_to_send, r_code, strlen(r_code));
      // memcpy(msg_to_send+mc_length, code, strlen(code));
      // mc_length += strlen(code);
      memcpy(msg_to_send,"HTTP/1.1 200 OK\r\n", 17);
      memcpy(msg_to_send+mc_length, current_date, strlen(current_date));
      mc_length += strlen(current_date);
      // memcpy(msg_to_send+mc_length, "Server: GVSU\r\n", 14);
      // mc_length += 14;
      memcpy(msg_to_send+mc_length, header_last_modified, strlen(header_last_modified));
      mc_length +=  strlen(header_last_modified);
      memcpy(msg_to_send+mc_length, content_length, strlen(content_length));
      mc_length += strlen(content_length);
      memcpy(msg_to_send+mc_length, cType, strlen(cType));
      mc_length += strlen(cType);
      memcpy(msg_to_send+mc_length,  connection_type, strlen(connection_type));
      mc_length += strlen(connection_type);
      memcpy(msg_to_send+mc_length,"\r\n",2);

      printf("Msg: %s\n", msg_to_send);

      int d = send(socket, msg_to_send, strlen(msg_to_send)+path_length,  0);
      printf("sock %d\n", d);
      printf("actual socket  %d\n", socket);

    } else {


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
  //  this will need  to change but for right now its ok
  //printf("Enter port number: ");
  char port[5000];
  //fgets(port, 5000, stdin);
  memcpy(port,"8080",5000);

  char path[5000];
  // sets path to current directory by default
  getcwd(path, sizeof(path));


  FILE* file;
  char filename[5000];

  // break up string to get the first char
  // struct option longopts =  {
  //   {"docroot",2,NULL,'d'}
  //   {"logfile",2,NULL,'l'}
  // };

  int opt = 0;
  while((opt = getopt(argc, argv, "p:d:l:")) !=  -1) {
    switch(opt) {
      case 'p':
        // do something
        printf("entered  p\n");
        memcpy(port, optarg, sizeof(optarg));
        break;
      case 'd':
        // do something
        memset(path,0,5000);
        break;
      case 'l':
        // do something
        // we  will wait  on  this one
        // will do something for filr
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
  printf("start listening\n");

  // should we use select?

  fd_set sockets;
  FD_ZERO(&sockets);
  FD_SET(sockfd, &sockets);
  printf("select problem\n");

  while(1) {
    printf("whilel loop\n" );
    fd_set tmp_set=sockets;
    select(FD_SETSIZE, &tmp_set, NULL, NULL, NULL);

    printf("selct  is  the issue\n");

    int i;
    for(i=0;i<FD_SETSIZE;i++) {
      char buf[5000];
      //printf("for loop\n");
      if(FD_ISSET(i, &tmp_set)) {
        if (i==sockfd) {
          int len = sizeof(clientaddr);
          // accepting a client socket
          int clientsocket = accept(sockfd, (struct sockaddr*)&clientaddr,&len);
          // add client address to struct
          FD_SET(clientsocket, &sockets);
        } else {

          // we are receiving from the client
          printf("else while\n");
          printf(" i %d\n", i);
          int val = recv(i,buf,5000,0);
          printf("val- %d\n", val);

          printf("buf  %s\n", buf);


          if (strncmp(buf,"GET", 3) == 0) {
            printf("get\n");
            // we received a get request
           // now parse request into tokens
           char http_path[1000];
           char *req_path;
           char *http_version;
           char format_http[9];
           char *pre_file_extension;
           char *file_extension;
           char* ignore_me;
           char temp_http[7];
           char temp_file_ext[5];
           char another_test[10];
           char og_path[100];
           //this line  may be  the problem
           req_path = strtok(buf+3," ");
           printf("OG  path: %s\n", req_path);
           printf("path len %d\n",strlen(req_path) );

           int path_size = strlen(req_path);
           memcpy(og_path,req_path,path_size);

           // TODO: may need to  change 4 to something else
          // file_extension = req_path + (path_size - 4);
           http_version = strtok(NULL, " ");
           if (strlen(http_version) > 1) {
             ignore_me = strtok(NULL, "/r/n");
             printf("http_version  %s\n",http_version );

             memcpy(format_http,http_version,9);

             printf("formatted %s\n", format_http);

             strcpy(another_test, format_http);

             pre_file_extension = strtok(req_path, ".");
             file_extension = strtok(NULL,"");
             printf("file ext  %s\n", file_extension);
             printf("strtok not the problem\n");
           }


           if (strncmp(http_version, "HTTP/1.1",8) == 0) {
             // now we need to locate path location
             printf("http  version  %s\n", format_http);
             memcpy(http_path, path, strlen(path));
             printf("HTTP path: %s\n", http_path);
             if (strlen(req_path) <= 1 || strlen(file_extension) <=1) {

               printf("%s\n",file_extension );
               memcpy(http_path+strlen(path),"/index.html",11);
              // memcpy(temp_http,file_extension,7);
              // memcpy(file_extension,file_ext,4);
               memcpy(temp_file_ext,"html",4);
               printf("tmp %s\n", temp_file_ext);
               printf("using index.html\n");
               printf("path %s\n", http_path);
               printf("strlen  %d\n",strlen(http_path) );
               printf("formatted http %s\n", format_http);
               //  do  nothing

             } else {
               printf("yikes %d\n", strlen(req_path));
               memcpy(http_path+strlen(path),og_path,strlen(og_path));
               memcpy(temp_file_ext,file_extension,strlen(file_extension));

              // memcpy(http_path+strlen(path)+strlen(req_path),".",1);
               // memcpy(http_path+strlen(path)+strlen(req_path),file_extension,strlen(file_extension));
               printf("new path: %s\n",http_path );

             }

             if (access(http_path, F_OK) != -1) {
               int request_code = 200;
               //file exists
               // will want a 200 response cod
               printf("file exists\n");
               printf("f http %s\n", format_http);
               printf("file ext%s\n", file_extension);
               printf("u better work %s\n", another_test);

               sendHTTPresoponse(another_test, http_path, 200, temp_file_ext,i);
               //printf("socket %d\n", i);

             }  else  {
               printf("404 error\n");
               //404 error file not found\
             }
           }

           // } else {
           //   //  send a 400 Bad Request Error
           //
           // }


          // } else {
            // error checking  for non get  requests
            // we don't actually want to do anything here
          }

          // FD_CLR(i, &sockets);
        }
      }


    }


  }
  }
  return 0;

}
