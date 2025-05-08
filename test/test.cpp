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
  cout << "lhs:" << endl;
  for (int i = 0; i < rhs.size(); i++){
    cout << (char)rhs[i];
  }
  cout << endl;

  cout << "rhs: " << endl;
  for (int i = 0; i < lhs.size(); i++){
    cout << (char)lhs[i];
  }
  cout << endl;
  return 0;
}

//will print the two names, only will terminate if the test fails
int printResultName(string name1, string name2){
  cout << "string 1: " << name1 << endl;
  cout << "string 2: " << name2 << endl;
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

  // FILE - PASS -------------------------------------------------------------------------------------------------------------------------------------
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

  // FILE - Empty File name -------------------------------------------------------------------------------------------------------------------------------------
 
  cout << "Testing Ser and De for a File message (both should fail, because the file has an empty file name)" << endl;
  struct FileStruct file_no_name = {.name =  "", .bytes = {'H', 'e', 'l', 'l', 'o'}};
  File fileSer_fail = pack109::serialize(file_no_name);
  cout << "Test 3: File ser" << endl;
  if (testVec(fileSer_fail, hello_file)){ //using array of bytes above 
    cout << "Passed!" << endl;
  } else{
    cout << "Failed!" << endl;
  }

  struct FileStruct file_helloDe_fail = pack109::deserialize_file(fileSer_fail);
  cout << "Test 4: File de" << endl;

  //finding results of both tests 
  file_de_name = testName(file_helloDe_fail.name, file_hello.name); //testing the names to see if they are equal
  file_de_bytes = testVec(file_helloDe_fail.bytes, file_hello.bytes); //testing the contents to see if they are equal

  //printing resilts of both test
  if (file_de_name && file_de_bytes){ //if they are both true
    cout << "Passed!" << endl;
  } else if (file_de_name){ //if name is true, bytes is not true
    cout << "Failed!" << endl;
    printResultVec(fileSer, hello_file);
  } else if (file_de_bytes){ //if bytes are true, name is not
    cout << "Failed!" << endl;
    printResultName(file_helloDe_fail.name, file_hello.name);
  }

  cout << endl;

  // FILE - Malformed File -------------------------------------------------------------------------------------------------------------------------------------
 
  cout << "Testing Ser and De for a File message (both should fail, because the message is malformed)" << endl;
  cout << "The way is is malformed is the serialization in bytes says that it should be a size of 3, but it should be a size of 5" << endl;
  File file_fail = {0xae, 0x01, //map tag, 1 length
                      0xaa, 0x04,  0x46, 0x69, 0x6C, 0x65,  //Key: string File
                      0xae, 0x02, //Value is 2 KVPs 
                      0xaa, 0x04, 0x6E, 0x61, 0x6D, 0x65, //Value: KVP1: Key : name
                      0xaa, 0x08, 0x66, 0x69, 0x6C, 0x65, 0x2E, 0x74, 0x78, 0x74, //Value: KVP1: Value : file.txt
                      0xaa, 0x05, 0x62, 0x79, 0x74, 0x65, 0x73, //Value: KVP 2: Key : bytes
                      0xac, 0x03, 0xa2, 0x48, 0xa2, 0x65, 0xa2, 0x6C, 0xa2, 0x6C, 0xa2, 0x6F}; //Value: KVP 2: Value : Hello
  cout << "Test 4: File ser" << endl;
  if (testVec(fileSer, file_fail)){ //using previouse ser of correct struct with malformed serializied bytes array
    cout << "Passed!" << endl;
  } else{
    cout << "Failed!" << endl;
    for (int i = 0; i < fileSer.size(); i++){
      if (fileSer[i] != file_fail[i]){
        cout << "It should be: " << (int)fileSer[i] << " but malformed: " << (int)file_fail[i] << endl;
      }
    }
  }

  // REQUEST - Pass -------------------------------------------------------------------------------------------------------------------------------------
  cout << endl;
  cout << "Testing Ser and De for a Request message (both tests should pass)" << endl;
  struct Request file_request = {.name = "file.txt"};
    File file_txt = {0xae, 0x01, //map tag, 1 length
                    0xaa, 0x07,  0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, //Key: string Request
                    0xae, 0x01, //Value is 1 KVPs 
                    0xaa, 0x04, 0x6E, 0x61, 0x6D, 0x65, //Value: KVP1: Key : name
                    0xaa, 0x08, 0x66, 0x69, 0x6C, 0x65, 0x2E, 0x74, 0x78, 0x74};

    //serialize
    File requestSer = pack109::serialize(file_request);
    cout << "Test 5: Request ser" << endl;
    if (testVec(requestSer, file_txt)){
      cout << "Passed!" << endl;
    } else{
      cout << "Failed!" << endl;
      printResultVec(requestSer, file_txt);
    }

    //deserialize
    struct Request request_de = pack109::deserialize_request(requestSer);
    cout << "Test 6: Request de" << endl;
    file_de_name = testName(request_de.name, file_request.name); //testing the names to see if they are equal
    if (file_de_name){
      cout << "Passed!" << endl;
    } else {
      printResultName(request_de.name, file_request.name);
    }



  return 0;
}
