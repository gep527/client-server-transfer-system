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
#include <fstream>

//calculates the file size of a specific file struct so that it can be extracted from the file 
int calculate_file_size(vec bytes, int position){
   int size = 0; //keeps track of file struct size
   if (bytes[position + size++] != PACK109_M8){ //checks tag
      std::cout << bytes[position + size];
      throw std::runtime_error("Failure!");
   }

   /*
      While this does include a try, catch block, it does not fully check for specifc errors
      However, the specific errors (correct tags and names) will be checked in deserialization
   */
   try {
      size += 16; //skips over File name and key
      size += bytes[position + size];
      size += 9; //skips over byte key
      size += 2 * (bytes[position + size]); //gets and adds the size of the file (multiplies 2 because there is a tag infront of every byte)
   } catch (std::exception e){ //cathing if this fails at any point
      std::cout << "Size mismatch error" << std::endl;
      exit(1);
   }

   return size; //size of file struct
}

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno;
   socklen_t clilen; //make it compatable for c++
   char buffer[65536]; //for larger file sizes
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

   /* 
      Checking if user is using the persist flags
   */

   std::string file_name; //if the persist flag, this will be the name of file 
   bool persist = false;//tracks if a user wants to persist
   if (argc == 4){ //meaning persist flag could be passed
      if ((strcmp(argv[1], "--persist") == 0) && strcmp(argv[2],"-p") == 0){ //then persist is passed  
         file_name = argv[3];
         
         persist = true; 
      }
   }
   std::string file_dir = "../files/"; //file directory for where my files are located
   for (char c: file_name){
      file_dir.push_back(c);
   }
   /* 
      Reading from file if flags are used
   */
   if (persist){ //if uses flags
      
      //opens file and initalizing vars
      string file_text; //gets file contents line by line
      vec file_info; //will store each byte from the file
      std::ifstream file(file_dir); //opening file containing HM serialization
      if (!file.is_open()){
         std::cerr << "Error Opening file"; //if file could not open
         exit(1);
      }

      //adding bytes onto file_info
      while(getline(file, file_text)){ //goes through the file
         for (char c : file_text){
            file_info.push_back(c); //adds each byte onto the vector
         }
      }

      /* 
         Must extract file structs from the file in order to add them, so deserializing the file
      */
      int place = 0; //tracks location in the file_info vec
      if (file_info[place++] != PACK109_M8){ //ensures first bit is the HM tag
         std::cout << "Wrong Tag" << std::endl;
         exit(1);
      }
      
      //initalizes
      int elements = file_info[place++]; //gets number of file structs in HM
      int start = place; //starting point of first File
      if (elements < 0){ //if there are no files
         std::cout << "No Files from disk to add to HM" << std::endl;
         exit(1);
      }

      for (int i = 0; i < elements; i++){ //will loop through the number of files

         //will calculate the size of the file struct in order to slice it and deserialize
         int file_size = calculate_file_size(file_info, start); 
         if (start + file_size > file_info.size()){ //ensuring safe bounds
            std::cout << "Invalid bounds" << std::endl;
         }

         vec file_temp = pack109::slice(file_info, start, (start + file_size)); //slices file struct 
         struct FileStruct file_to_add = pack109::deserialize_file(file_temp); //deserializes
         fileHM->insert(file_to_add.name, file_to_add.bytes); //inserts file into HM

         start += file_size + 1; //adds file struct length and moves to next bit
      } 
   }

   bool comm = true; //tracks infinite loop of communication between client and server, will be set = fales when communication is ended 
   while (comm){ //enters an infinite loop of communication

      /* If connection is established then start communicating */

      bzero(buffer,65535); //larger file sizes
      n = read( newsockfd,buffer, 65535 ); //Question 5: message is read in buffer (updates for larger file sizes)

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

   /*
      Write the HM onto a file so it can store it 
   */
   if (persist){
      fileHM->write(); //will write HM info onto data.txt //Question 11
   }

   return 0;
}

