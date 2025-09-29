#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include <cstdlib> 
#include <iostream>
#include <cstring>
#include <chrono>
#include "../include/HashMap.hpp"
#include "../include/pack109.hpp"
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>

std::mutex cout_mutex;

// ----------------------------------------------------
// Utility: Calculate the size of a serialized FileStruct
// ----------------------------------------------------
int calculate_file_size(vec bytes, int position){
   int size = 0; 

   if (bytes[position + size++] != PACK109_M8){
      std::cout << bytes[position + size];
      throw std::runtime_error("Failure!");
   }

   try {
      size += 16;                        
      size += bytes[position + size];    
      size += 9;                         
      size += 2 * (bytes[position + size]); 
   } catch (std::exception e){
      std::cout << "Size mismatch error" << std::endl;
      exit(1);
   }

   return size;
}

// ----------------------------------------------------
// Threaded client handler
// ----------------------------------------------------
void handle_client(int newsockfd, HashMap* fileHM, bool persist, std::string file_dir) {
   char buffer[65536];
   int n;
   bool comm = true;

   while (comm){
      // --- Step 6: Read incoming message from client ---
      bzero(buffer, 65535);
      n = read(newsockfd, buffer, 65535);
      if (n < 0) {
         perror("ERROR reading from socket");
         break;
      } else if (n == 0){
         std::lock_guard<std::mutex> lock(cout_mutex);
         std::cout << "Client disconnected" << std::endl;
         comm = false;
         break;
      }

      {
         std::lock_guard<std::mutex> lock(cout_mutex);
         std::cout << "Message Received" << std::endl;
      }

      // --- Step 7: Convert buffer into File (vector<u8>) ---
      std::string buffer_str = buffer;
      File recieved;
      for (u8 bytes : buffer_str){
         recieved.push_back(bytes);
      }

      // --- Step 8: Decrypt with XOR key (42) ---
      File decrypted;
      for (u8 bytes : recieved){
         decrypted.push_back(bytes ^ 42);
      }

      // --- Step 9: Deserialize message (Request or FileStruct) ---
      int size_key = decrypted[3]; 
      string response;
      struct Status status_report;
      struct FileStruct file_found;

      if (size_key == 4){ 
         struct FileStruct file_deser = pack109::deserialize_file(decrypted);

         bool replace = fileHM->insert(file_deser.name, file_deser.bytes);

         try {
            fileHM->get(file_deser.name); 
            if (replace){
               status_report = {.message = "Sucess! New file is stored."};
            } else {
               status_report = {.message = "Sucess! File was replaced."};
            }
         } catch (std::exception e){
            status_report = {.message = "Failure! Can not store."};
         }

         response = "status";

      } else if (size_key == 7){ 
         struct Request request_deser = pack109::deserialize_request(decrypted);

         try {
            File file_found_value = fileHM->get(request_deser.name);
            file_found = {.name = request_deser.name, .bytes = file_found_value};
            response = "file";
         } catch (std::exception e){
            status_report = {.message = "File not found."};
            response = "status";
         }

      } else {
         throw;
      }

      // --- Step 10: Serialize + Encrypt response ---
      File send_encrypt;
      if (response == "status"){
         File statusSer = pack109::serialize(status_report);
         for (u8 bytes : statusSer){
            send_encrypt.push_back(bytes ^ 42);
         }
      } else {
         File fileSer = pack109::serialize(file_found);
         for (u8 bytes : fileSer){
            send_encrypt.push_back(bytes ^ 42);
         }
      }

      n = write(newsockfd, send_encrypt.data(), send_encrypt.size());
      if (n < 0) {
         perror("ERROR writing to socket");
         break;
      }
   }

   // Persist HashMap if enabled
   if (persist){
      fileHM->write();
   }

   close(newsockfd);
}

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno;
   socklen_t clilen;
   struct sockaddr_in serv_addr, cli_addr;

   // --- Step 1: Create a TCP socket ---
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   // --- Step 2: Initialize server address ---
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5001; 

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);

   // --- Step 3: Bind socket ---
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }

   // Create an empty hash map for storing files
   HashMap* fileHM = new HashMap(20);

   // --- Step 4: Listen ---
   listen(sockfd, 5);
   clilen = sizeof(cli_addr);

   // ----------------------------------------------------
   // Optional persistence setup
   // ----------------------------------------------------
   std::string file_name;
   bool persist = false;
   if (argc == 4){ 
      if ((strcmp(argv[1], "--persist") == 0) && strcmp(argv[2],"-p") == 0){
         file_name = argv[3];
         persist = true; 
      }
   }

   std::string file_dir = "./files/"; 
   for (char c: file_name){
      file_dir.push_back(c);
   }

   // (Loading persistence skipped for brevity — same as your original)

   // ----------------------------------------------------
   // Multi-threaded accept loop
   // ----------------------------------------------------
   while (true) {
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
      if (newsockfd < 0) {
         perror("ERROR on accept");
         continue;
      }

      {
         std::lock_guard<std::mutex> lock(cout_mutex);
         std::cout << "Accepted new client" << std::endl;
      }

      std::thread client_thread(handle_client, newsockfd, fileHM, persist, file_dir);
      client_thread.detach(); // run independently
   }

   close(sockfd);
   return 0;
}
