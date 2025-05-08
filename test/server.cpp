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
#include "../include/pack109.hpp"
#include <vector>

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

      //Question 7:
      int size_key = decrypted[3]; //where the size of the key is stored (make it an int to compare)
      string response; //will determine if a status response or file response is needed
      struct Status status_report; //stores the status message based on file status
      struct FileStruct file_found; //stores the file if requested and found
      if (size_key == 4){ //means that the key should be a File, will check in deserialization
         struct FileStruct file_deser = pack109::deserialize_file(decrypted); //creates a file struct

         //Question 8:
         bool replace; //will track if file is replaced or not
         replace = fileHM->insert(file_deser.name, file_deser.bytes); //inserting the File into the HM 

         //seeing the type of status of the file
         try {
            /*
               - by calling get, it will find the File according to the key
               - if it exist, it will continue through the try block
               - if it does not exist, it will throw causing the catch block to execute
            */
            fileHM -> get(file_deser.name); 
            //will get to this point if it exists
            if (replace){ //if replace = true, the file was not already in the HM
               status_report = {.message = "Sucess! New file is stored."};
            } else{ // replace = false, the file was already in HM
               status_report = {.message = "Sucess! File was replaced."};
            }
         } catch (std::exception e){//file did not exist
            status_report = {.message = "Failure! Can not store."};
         }

         response = "status"; //status response is needed

      } else if (size_key == 7){ //means that the key should be a Request, will check in deserialization
         struct Request request_deser = pack109::deserialize_request(decrypted); //creates a request struct
         
         //Question 9:
         try {
            /*
               - by calling get, it will find the File according to the key
               - if it exist, it will continue through the try block
               - if it does not exist, it will throw causing the catch block to execute
            */
            File file_found_value = fileHM -> get(request_deser.name); 
            //will get to this point if it exists
            file_found = {.name = request_deser.name, .bytes = file_found_value};

            response = "file"; //file response is needed
         } catch (std::exception e){ //file did not exist
            status_report = {.message = "File not found."};
            response = "status"; //status response is needed
         }

      } else{
         throw; //undentified
      }


      /* Question 10: Write a response to the client */
      File send_encrypt; //encrypted message to send to client
      if (response == "status"){ //if status is the return message
         File statusSer = pack109::serialize(status_report); //serializes status
         //Encrypt
         for (u8 bytes : statusSer){
            u8 encrypt = bytes ^ 42; //XOR with key (42)
            send_encrypt.push_back(encrypt);
         }
      } else{ //if file is return message
         File fileSer = pack109::serialize(file_found); //serializes file
         //Encrypt
         for (u8 bytes : fileSer){
            u8 encrypt = bytes ^ 42; //XOR with key (42)
            send_encrypt.push_back(encrypt);
         }
      }

      n = write(newsockfd,send_encrypt.data(), send_encrypt.size()); //will send the file encrypted back to the client
   
      if (n < 0) {
         perror("ERROR writing to socket");
         exit(1);
      }
   }
      
   return 0;
}

