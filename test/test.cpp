#include <cstdlib> 
#include <iostream>
#include <cstring>
#include <chrono>
#include "../include/HashMap.hpp"
#include <vector>
#include "../include/pack109.hpp"

using namespace std;

int testVec(vec lhs, vec rhs){

  bool file_ser_match = true; //stores the result of comparison of bytes
  if (lhs.size() != rhs.size()){
    file_ser_match = false; //sets equal to false if they are not the same size
  }

  if (file_ser_match){ //will only execite if they are the same size
    for (int i = 0; i < lhs.size(); i++){
      if (lhs[i] != rhs[i]){
        file_ser_match = false; //if values are not equal, will set equal to false
      }
    }
  }
  
  if (file_ser_match){
    std::cout << "Passed!" << std::endl;
  } else{
    std::cout << "Failed!" << std:: endl;
    std::cout << "It is supposed to be:" << std:: endl;
    for (int i = 0; i < rhs.size(); i++){
      std::cout << rhs[i];
    }
    std::cout << std::endl;
    std::cout << "Yours is: " << std::endl;
    for (int i = 0; i < lhs.size(); i++){
      std::cout << lhs[i];
    }
    std::cout << std::endl;
    exit(1);
  }
  return 0;
}

int main(){

  struct FileStruct file_hello = {.name =  "file.txt", .bytes = {'H', 'e', 'l', 'l', 'o'}};
  File hello_file = {0xae, 0x01, //map tag, 1 length
                      0xaa, 0x04,  0x46, 0x69, 0x6C, 0x65,  //Key: string File
                      0xae, 0x02, //Value is 2 KVPs 
                      0xaa, 0x04, 0x6E, 0x61, 0x6D, 0x65, //Value: KVP1: Key : name
                      0xaa, 0x08, 0x66, 0x69, 0x6C, 0x65, 0x2E, 0x74, 0x78, 0x74, //Value: KVP1: Value : file.txt
                      0xaa, 0x05, 0x62, 0x79, 0x74, 0x65, 0x73, //Value: KVP 2: Key : bytes
                      0xac, 0x05, 0xa2, 0x48, 0xa2, 0x65, 0xa2, 0x6C, 0xa2, 0x6C, 0xa2, 0x6F}; //Value: KVP 2: Value : Hello
  File fileSer = pack109::serialize(file_hello);
  std::cout << "Test 1: File ser" << std::endl;
  testVec(fileSer, hello_file);

  struct FileStruct file_helloDe = pack109::deserialize_file(fileSer);

  std::cout << "Test 2: File de" << std::endl;
  bool file_match = true;
  //checking names
  if (file_helloDe.name != file_hello.name){
    std::cout << "Failed" << std::endl;
    file_match = false;
    std::cout << "It is supposed to be" << file_hello.name << std::endl;
    std::cout << "Yours is " << file_helloDe.name << std::endl;
  } else{
    std::cout << "Passed!" << std::endl;
  }
  //checking file contents
  testVec(file_helloDe.bytes, file_hello.bytes);

  return 0;
}
