#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include<unistd.h>


int main(){
   int server_fd = socket(AF_INET , SOCK_STREAM , 0);
   if(server_fd<0){
    perror("socket failed");
    exit(EXIT_FAILURE);
   }
   std::cout<<"SOCKET created,fd="<<server_fd<<std::endl;
   int opt =1 ;
   if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0){
    perror("setsocket failed");
    exit(EXIT_FAILURE);
   }
    sockaddr_in address{};
    address.sin_family=AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(6379);

    if(bind(server_fd,(struct sockaddr*)&address,sizeof(address))<0){
    perror("bind failed");
    exit(EXIT_FAILURE);
   }
   std::cout<<"bound to 0.0.0.0:6379"<<std::endl;
   if(listen(server_fd,10)<0){
    perror("listen failed");
    exit(EXIT_FAILURE);
   }
   std::cout<<"listening on port 6379..."<<std::endl;
   sockaddr_in client_address{};
   socklen_t client_len = sizeof(client_address);

   int client_fd = accept(server_fd,(struct sockaddr*)&client_address,&client_len);
   if(client_fd<0){
    perror("accept failed");
    exit(EXIT_FAILURE);
   }
   std::cout<<"accepted client,fd="<<client_fd<<std::endl;
   close(client_fd);
   close(server_fd);
   return 0;
}