#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#define QUEUE_SIZE 50
#define THREAD_POOL_SIZE 10



int server_socket = -1; // Global server socket for cleanup
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int create_service(short *port)
{
   struct sockaddr_in6 local_addr;
   int yes=1;

   if ((server_socket = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
   {
    perror("Socket creation failed at  {create_server @ server.c}");
      return -1;
   }

   if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,&yes, sizeof(int)) == -1)
   {
    perror("Setsockopt failed at {create_server @ server.c}");
      return -1;
   }
   memset(&local_addr, 0, sizeof(local_addr));
   local_addr.sin6_family = AF_INET6;         /* host byte order */
   local_addr.sin6_port = htons(port);       /* short, network byte order */
   local_addr.sin6_addr = in6addr_any; /* automatically fill with my IP */


   if (bind(server_socket, (struct sockaddr *)&local_addr, sizeof(struct sockaddr)) == -1)
   {
      perror("Bind Failed at {create_server @ server.c}");
      return -1;
   }


   if (listen(server_socket, QUEUE_SIZE) == -1)
   {
    perror(" Listen Failed at {create_server @ server.c}");
      return -1;
   }



    if(port == 0){
        struct sockaddr_in6 bound_addr;
        socklen_t len = sizeof(bound_addr);
        if (getsockname(server_socket, (struct sockaddr *)&bound_addr, &len) == 0) {
            printf("HTTP server is using TCP port %d\n", ntohs(bound_addr.sin6_port));
    }
    }


   return server_socket;
}

int accept_connection(int fd)
{
   while(1){
    struct sockaddr_in6 client;
    socklen_t client_addr_size = sizeof(client);
    int *client_fd = malloc(sizeof(int));

    if(client_fd ==NULL){
        perror("clinet_fd malloc failed at  {accept_connection @ server.c}");
        continue;
    }


     *client_fd = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (*client_fd == -1) {
            perror("Accept failed");
            free(client_fd);
            continue;
        }

    pthread_t client_thread;
    if(pthread_create(&client_thread, NULL, handle_client, client_fd) != 0){
        perror("Thread creation failed");
            close(*client_fd);
            free(client_fd);
        } 
        
        else {
            pthread_detach(client_thread);
        }


   }
}
