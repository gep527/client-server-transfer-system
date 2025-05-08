#include <cstdlib> 
#include <iostream>
#include <cstring>
#include <chrono>
#include "../include/HashMap.hpp"
#include <vector>
#include "../include/pack109.hpp"

using namespace std;

//will print the two vectors, only will terminate if the test fails
int printResultVec(vec lhs, vec rhs){
  cout << "It is supposed to be:" << endl;
  for (int i = 0; i < rhs.size(); i++){
    cout << rhs[i];
  }
  cout << endl;

  cout << "Yours is: " << endl;
  for (int i = 0; i < lhs.size(); i++){
    cout << lhs[i];
  }
  cout << endl;
  return 0;
}

//will print the two names, only will terminate if the test fails
int printResultName(string name1, string name2){
  cout << "It is supposed to be" << name1 << endl;
  cout << "Yours is " << name2 << endl;
  return 0;
}

//will see if the two vectors are equal, and return a bool
bool testVec(vec lhs, vec rhs){
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
  
  if (file_ser_match){ //if they match, passed 
    return true;
  } else{ //if they do not match, failed
    return false;
  }
}

//will see if the two names are equal, and return a bool
int testName(string name1, string name2){
  bool file_match = true;
  //checking names
  if (name1 != name2){
    file_match = false;
  } 
  return file_match;
}

int main(){

  cout << "Testing Ser and De for a File message (both tests should pass)" << endl;
  struct FileStruct file_hello = {.name =  "file.txt", .bytes = {'H', 'e', 'l', 'l', 'o'}};
  File hello_file = {0xae, 0x01, //map tag, 1 length
                      0xaa, 0x04,  0x46, 0x69, 0x6C, 0x65,  //Key: string File
                      0xae, 0x02, //Value is 2 KVPs 
                      0xaa, 0x04, 0x6E, 0x61, 0x6D, 0x65, //Value: KVP1: Key : name
                      0xaa, 0x08, 0x66, 0x69, 0x6C, 0x65, 0x2E, 0x74, 0x78, 0x74, //Value: KVP1: Value : file.txt
                      0xaa, 0x05, 0x62, 0x79, 0x74, 0x65, 0x73, //Value: KVP 2: Key : bytes
                      0xac, 0x05, 0xa2, 0x48, 0xa2, 0x65, 0xa2, 0x6C, 0xa2, 0x6C, 0xa2, 0x6F}; //Value: KVP 2: Value : Hello
  File fileSer = pack109::serialize(file_hello);
  cout << "Test 1: File ser" << endl;
  if (testVec(fileSer, hello_file)){
    cout << "Passed!" << endl;
  } else{
    cout << "Failed!" << endl;
    printResultVec(fileSer, hello_file);
  }

  //deserializing
  struct FileStruct file_helloDe = pack109::deserialize_file(fileSer);

  //printing results
  cout << "Test 2: File de" << endl;
  //finding results of both tests 
  bool file_de_name = testName(file_helloDe.name, file_hello.name); //testing the names to see if they are equal
  bool file_de_bytes = testVec(file_helloDe.bytes, file_hello.bytes); //testing the contents to see if they are equal
  //printing resilts of both test
  if (file_de_name && file_de_bytes){ //if they are both true
    cout << "Passed!" << endl;
  } else if (file_de_name){ //if name is true, bytes is not true
    cout << "Failed!" << endl;
    printResultVec(fileSer, hello_file);
  } else if (file_de_bytes){ //if bytes are true, name is not
    cout << "Failed!" << endl;
    printResultName(file_helloDe.name, file_hello.name);
  }

  cout << endl;

  return 0;
}
