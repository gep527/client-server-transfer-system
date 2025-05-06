#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <unistd.h>

//adding in all of directories I included in test.cpp in Midterm 2
#include <cstdlib> 
#include <iostream>
#include <cstring>
#include <chrono>
#include "../include/HashMap.hpp"

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno;
   socklen_t clilen; //make it compatable for c++
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5001;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }

   HashMap* fileHM = new HashMap(20); //creates empty hashMap before  waiting for client 

   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);

   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	
   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }
   
   bool comm = true; //tracks infinite loop of communication between client and server, will be set = fales when communication is ended 
   while (comm){ //enters an infinite loop of communication

      /* If connection is established then start communicating */

      bzero(buffer,256);
      n = read( newsockfd,buffer,255 ); //Question 5: message is read in buffer 

      if (n < 0) {
         perror("ERROR reading from socket");
         exit(1);
      } else if (n == 0){ //meaning nothing was read in
         printf("Client is disconnected");
         comm = false;
         break;
      }

      /*
         The message is currently decrypted and serizalized, so must decrypt and deseralize
      */
      
      //printf("Here is the message: %s\n",buffer);

      //Question 6:
      //storing the message as a File (vector of u8)
      std::string buffer_str = buffer; //convert buffer into a string
      File recieved; //File vector
      for (u8 bytes : buffer_str){
         recieved.push_back(bytes); //adds each byte onto the File vector
      }

      //decrypt message
      File decrypted;
      for (u8 bytes : recieved){
         u8 decrypt = bytes ^ 42; //XOR by the key (42)
         decrypted.push_back(decrypt);
      }

      

      
      /* Write a response to the client */
      n = write(newsockfd,"I got your message",18);
   
      if (n < 0) {
         perror("ERROR writing to socket");
         exit(1);
      }
   }
      
   return 0;
}

