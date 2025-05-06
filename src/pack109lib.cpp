#include <vector> 
#include <stdio.h>
#include <iostream>
#include "../include/pack109.hpp"
#include <cstring>
using std::begin;
using std::end;

vec pack109::slice(const vec& bytes, int vbegin, int vend) {
  if (vbegin < 0 || vend >= (int)bytes.size() || vbegin > vend) {
      throw std::out_of_range("slice: invalid indices. asked for slice(" + std::to_string(vbegin) + ", " + std::to_string(vend) + 
          "), but vector size is " + std::to_string(bytes.size()));
  }

  auto start = bytes.begin() + vbegin;
  auto end   = bytes.begin() + vend + 1; // inclusive
  return vec(start, end);
}

//TEST BOOL -------------------------------------------------------
vec pack109::serialize(bool item) {
  vec bytes;
  if (item == true) {
    bytes.push_back(PACK109_TRUE);
  } else {
    bytes.push_back(PACK109_FALSE);
  }
  return bytes;
}

bool pack109::deserialize_bool(vec bytes) {
  if (bytes.size() < 1) {
     throw;
  }

  if (bytes[0] == PACK109_TRUE) {
    return true;
  } else if (bytes[0] == PACK109_FALSE) {
    return false;
  } else {
    throw;
  }

}

//TEST INTS -------------------------------------------------------

//UNSIGNED INTS ---------------------------------------------------

//u8 --------------------------------------------------------------
vec pack109::serialize(u8 item) {
  vec bytes;
  bytes.push_back(PACK109_U8);
  bytes.push_back(item);
  return bytes;
}

u8 pack109::deserialize_u8(vec bytes) {
  if (bytes.size() < 2) {
    throw;
  }
  if (bytes[0] == PACK109_U8) {
    return bytes[1];
  } else {
    throw;
  }
}

//u32 -------------------------------------------------------------
vec pack109::serialize(u32 item){ //32-bit big-endian unsigned integer
  vec bytes;
  bytes.push_back(PACK109_U32); //adding tag
  //USED SOURCE 1 
  bytes.push_back((item >> 24) & 0xFF); //left most byte
  bytes.push_back((item >> 16) & 0xFF); //left middle byte
  bytes.push_back((item >> 8) & 0xFF); //right middle byte
  bytes.push_back((item >> 0) & 0xFF); //the right most byte 

  /* 
    The way this logic works is by extracting byte by byte for the 4 byte (32 bit) int
    For example if you have the number 0x12345678: it can be broken up by [0x12, 0x34, 0x56, 0x78]
    To do this, you shift it by the number of bits you want to extract in multiples of 8 because there are 8 bits in a byte.
    So continuing the example of 0x12345678: 
    - (shifting it 24 bites will give 0000 0000 0000 0000 0000 0000 0001 0010) and & with 0xFF (which is 1111 1111) gives itself, so 0000 0000 0000 0000 0000 0000 0001 0010 which is 0x12 
    - (shifting it 16 bites will give 0000 0000 0000 0000 0001 0010 0011 0100) and & with 0xFF (which is 1111 1111) will extract the last byte, so 0000 0000 0000 0000 0000 0000 0011 0100 which is 0x34 
    - (shifting it 8 bites will give 0000 0000 0001 0010 0011 0100 0101 0110) and & with 0xFF (which is 1111 1111) will extract the last byte, so 0000 0000 0000 0000 0000 0000 0101 0110 which is 0x56
    - (shifting it 0 bytes will keep it as 0001 0010 0011 0100 0101 0110 0111 1000) and & with 0xFF (which is 1111 1111) will extract the last byte, so 0000 0000 0000 0000 0000 0000 0111 1000 which is 0x78
    This will effectively separate the item into its bytes 
   */
  return bytes;
}

u32 pack109::deserialize_u32(vec bytes) {
  u32 result;
  if (bytes.size() < 5) { //tag + 4 bytes = 5 
    throw;
  }
  if (bytes[0] == PACK109_U32) { //checks if tags is equal to u32 tag
    result = ((bytes[1] << 24) |(bytes[2] << 16)|(bytes[3]<< 8)|(bytes[4]<< 0)); //concatenates the result of the the bytes (other than tag)
    /*
      What this does is takes each section of the bytes, shifts it to its appropriate location, and essentially adds it to the other bytes
      For example, if bytes was {0xa3, 0x12, 0x34, 0x56, 0x78}, it would skip over the tag, 
      the it would shift the bites of 0x12 24 places, so that it is the first byte, then shift the bites of 0x34 16 places so that it is the second byte, and so on
      Then it would OR all of these shifts, but since each byte is designated to its own spot, when you OR them together, it will store all of the bits onto result where the other ones are 0
      Result is returned which is the bytes concatenated together
    */

  } else {
    throw;
  }
  return result;
}

//u64 -------------------------------------------------------------
vec pack109::serialize(u64 item){ //64-bit big-endian unsigned integer
  vec bytes;
  bytes.push_back(PACK109_U64); //adding tag
  bytes.push_back((item >> 56) & 0xFF); // 1st byte
  bytes.push_back((item >> 48) & 0xFF); // 2nd byte
  bytes.push_back((item >> 40) & 0xFF); // 3rd byte
  bytes.push_back((item >> 32) & 0xFF); // 4th byte
  bytes.push_back((item >> 24) & 0xFF); // 5th byte
  bytes.push_back((item >> 16) & 0xFF); // 6th byte
  bytes.push_back((item >> 8) & 0xFF); // 7th byte
  bytes.push_back((item >> 0) & 0xFF); // 8th byte

  /* 
    Uses same logic as u32, just for 4 more bytes  
   */
  return bytes;
}

u64 pack109::deserialize_u64(vec bytes) {
  u64 result;
  if (bytes.size() < 9) { //tag + 8 bytes = 9 
    throw;
  }
  if (bytes[0] == PACK109_U64) { //checks if tags is equal to u64 tag

    //creating separate variables for each portion of the integer
    //have to cast them all because they are being significantly casted, so it is safer when casting them first 
    u64 first = ((u64)bytes[1] << 56);
    u64 second = ((u64)bytes[2] << 48);
    u64 third = ((u64)bytes[3]<< 40);
    u64 fourth = ((u64)bytes[4]<< 32);
    u64 fifth = ((u64)bytes[5]<< 24);
    u64 sixth = ((u64)bytes[6] << 16);
    u64 seventh = ((u64)bytes[7]<< 8);
    u64 eigth = ((u64)bytes[8] << 0);

    result = (first|second|third|fourth|fifth|sixth|seventh|eigth); //concatenates the result of the the bytes (other than tag)
    /*
      Uses similar logic as u32, just for 4 more bytes
    */

  } else {
    throw;
  }
  return result;
}

//SIGNED INTS -----------------------------------------------------

//i8 --------------------------------------------------------------
/* 
  Same format as u8, just for signed i8
*/
vec pack109::serialize(i8 item) {
  vec bytes;
  bytes.push_back(PACK109_I8);
  bytes.push_back(item);
  return bytes;
}

/* 
  Same format as u8, just for signed i8
*/
i8 pack109::deserialize_i8(vec bytes) {
  if (bytes.size() < 2) {
    throw;
  }
  if (bytes[0] == PACK109_I8) {
    return bytes[1];
  } else {
    throw;
  }
}
//i32 -------------------------------------------------------------
vec pack109::serialize(i32 item){ //32-bit big-endian signed integer
  vec bytes;
  bytes.push_back(PACK109_I32); //adding tag
  bytes.push_back((item >> 24) & 0xFF); //left most byte
  bytes.push_back((item >> 16) & 0xFF); //left middle byte
  bytes.push_back((item >> 8) & 0xFF); //right middle byte
  bytes.push_back((item >> 0) & 0xFF); //the right most byte 

  /* 
    Same logic as u32 unsigned ints, just for signed ints 
   */
  return bytes;
}

i32 pack109::deserialize_i32(vec bytes) {
  i32 result;
  if (bytes.size() < 5) { //tag + 4 bytes = 5 
    throw;
  }
  if (bytes[0] == PACK109_I32) { //checks if tags is equal to u32 tag
    result = ((bytes[1] << 24) |(bytes[2] << 16)|(bytes[3]<< 8)|(bytes[4]<< 0)); //concatenates the result of the the bytes (other than tag)
    /*
      Same logic as u32 deserialization process
    */

  } else {
    throw;
  }
  return result;
}

//i64 -------------------------------------------------------------
vec pack109::serialize(i64 item){ //64-bit big-endian signed integer
  vec bytes;
  bytes.push_back(PACK109_I64); //adding tag
  bytes.push_back((item >> 56) & 0xFF); // 1st byte
  bytes.push_back((item >> 48) & 0xFF); // 2nd byte
  bytes.push_back((item >> 40) & 0xFF); // 3rd byte
  bytes.push_back((item >> 32) & 0xFF); // 4th byte
  bytes.push_back((item >> 24) & 0xFF); // 5th byte
  bytes.push_back((item >> 16) & 0xFF); // 6th byte
  bytes.push_back((item >> 8) & 0xFF); // 7th byte
  bytes.push_back((item >> 0) & 0xFF); // 8th byte

  /* 
    Uses same logic as u64, just for signed ints 
   */
  return bytes;
}

i64 pack109::deserialize_i64(vec bytes) {
  i64 result;
  if (bytes.size() < 9) { //tag + 8 bytes = 9 
    throw;
  }
  if (bytes[0] == PACK109_I64) { //checks if tags is equal to u64 tag

    //creating separate variables for each portion of the integer
    //have to cast them all because they are being significantly casted, so it is safer when casting them first 
    u64 first = ((i64)bytes[1] << 56);
    u64 second = ((i64)bytes[2] << 48);
    u64 third = ((i64)bytes[3]<< 40);
    u64 fourth = ((i64)bytes[4]<< 32);
    u64 fifth = ((i64)bytes[5]<< 24);
    u64 sixth = ((i64)bytes[6] << 16);
    u64 seventh = ((i64)bytes[7]<< 8);
    u64 eigth = ((i64)bytes[8] << 0);

    result = (first|second|third|fourth|fifth|sixth|seventh|eigth); //concatenates the result of the the bytes (other than tag)
    /*
      Uses similar logic as i64, just for i64 signed 
    */

  } else {
    throw;
  }
  return result;
}
//FLOATS -----------------------------------------------------

//f32 ---------------------------------------------------------
vec pack109::serialize(f32 item){ //64-bit float
  vec bytes;
  bytes.push_back(PACK109_F32); //adding tag

  u32 float_to_int; //where the float will be stored as an int in order to do bit shifting operatiosn
  //USED SOURCE 5 
  std::memcpy(&float_to_int, &item, sizeof(item)); //copies the specified number of bytes from one memory location to the other memory location regardless of the type of data stored

  bytes.push_back((float_to_int >> 24) & 0xFF); //left most byte
  bytes.push_back((float_to_int >> 16) & 0xFF); //left middle byte
  bytes.push_back((float_to_int >> 8) & 0xFF); //right middle byte
  bytes.push_back((float_to_int >> 0) & 0xFF); //the right most byte 

  /* 
    Same logic as u32 unsigned ints, just for floats
   */
  return bytes;
}

f32 pack109::deserialize_f32(vec bytes){
  f32 result;
  if ((bytes.size() < 5) || bytes[0] != PACK109_F32) { //tag + 4 bytes = 5 and tag is not f32
    throw;
  }

  u32 int_to_float = ((bytes[1] << 24) |(bytes[2] << 16)|(bytes[3]<< 8)|(bytes[4]<< 0)); //concatenates the result of the the bytes (other than tag)
  std::memcpy(&result, &int_to_float, sizeof(int_to_float)); //copies the specified number of bytes from one memory location to the other memory location regardless of the type of data stored

  return result;
}

//f64 --------------------------------------------------------
vec pack109::serialize(f64 item){ //64-bit float
  vec bytes;
  bytes.push_back(PACK109_F64); //adding tag

  u64 float_to_int; //where the float will be stored as an int in order to do bit shifting operatiosn
  std::memcpy(&float_to_int, &item, sizeof(item)); //copies the specified number of bytes from one memory location to the other memory location regardless of the type of data stored

  bytes.push_back((float_to_int >> 56) & 0xFF); // 1st byte
  bytes.push_back((float_to_int >> 48) & 0xFF); // 2nd byte
  bytes.push_back((float_to_int >> 40) & 0xFF); // 3rd byte
  bytes.push_back((float_to_int >> 32) & 0xFF); // 4th byte
  bytes.push_back((float_to_int >> 24) & 0xFF); // 5th byte
  bytes.push_back((float_to_int >> 16) & 0xFF); // 6th byte
  bytes.push_back((float_to_int >> 8) & 0xFF); // 7th byte
  bytes.push_back((float_to_int >> 0) & 0xFF); // 8th byte 

  /* 
    Same logic as u32 unsigned ints, just for floats
   */
  return bytes;
}

f64 pack109::deserialize_f64(vec bytes) {
  f64 result;
  if (bytes.size() < 9) { //tag + 8 bytes = 9 
    throw;
  }
  if (bytes[0] == PACK109_F64) { //checks if tags is equal to u64 tag

    //creating separate variables for each portion of the integer
    //have to cast them all because they are being significantly casted, so it is safer when casting them first 
    u64 first = ((i64)bytes[1] << 56);
    u64 second = ((i64)bytes[2] << 48);
    u64 third = ((i64)bytes[3]<< 40);
    u64 fourth = ((i64)bytes[4]<< 32);
    u64 fifth = ((i64)bytes[5]<< 24);
    u64 sixth = ((i64)bytes[6] << 16);
    u64 seventh = ((i64)bytes[7]<< 8);
    u64 eigth = ((i64)bytes[8] << 0);

    u64 int_to_float = (first|second|third|fourth|fifth|sixth|seventh|eigth); //concatenates the result of the the bytes (other than tag)
    std::memcpy(&result, &int_to_float, sizeof(int_to_float)); //copies the specified number of bytes from one memory location to the other memory location regardless of the type of data stored

    /*
      Uses similar logic as f32 and i64
    */

  } else {
    throw;
  }
  return result;
}

//STRINGS -----------------------------------------------------
vec pack109::serialize(string item){
  vec bytes;
  if (item.size() <= 255){ //represents an 8 bit length string 
    bytes.push_back(PACK109_S8); //adding the tag
    bytes.push_back((u8)item.size()); //size as 1 byte length
  } else if (item.size() > 255 && item.size() <= 65535) { //represents an 16 bit length string 
    bytes.push_back(PACK109_S16); //adding the tag

    //adding the length over 2 bytes - using similar bit shifting process as serializing u32
    bytes.push_back((item.size() >> 8) & 0xFF); //first part of 300 bit representation
    bytes.push_back(item.size() & 0xFF); //second part of 300 bit representation
    /*
      The way this works is splittng the size into two bytes 
      Using the example of 300 as the length of the string, this is how this works. Since 300 > 255 this works because the bits fall into the 2 byte range
      If the size is 300 its binary is 0000 0001 0010 1100, so we must add this in incraments of 2 
      - shifting 300 by 8 bits gives 00000000 00000001 and & with 0xFF gives (00000001) which is the first bit that is going to be added to represent length
      - then & the 300 by 0xFF gives (0000 0001 0010 1100) & (1111 1111) which equals (0010 1100) which is the last byte in the representation of 300
      it will add both of the bytes, which represents the value of 300 in two separate bytes 
    */


  } else{ //meaning it is not a 1 or 2 byte length, which is invalid
    throw;
  }

  //adding each char
  for (char c : item){
    bytes.push_back((u8) c); //will cast the char into a byte
  }

  return bytes;
}

string pack109::deserialize_string(vec bytes){
  string result;
  if (bytes.size() < 2){ //includes tag and length 
    throw; 
  }
  int len = 0; //the length of the string 
  int beg = 0; //short for beginning, tracks num of tags and length bytes
  u8 tag = bytes[0]; //will determine if it is 8 or 16 bit
  if (tag == PACK109_S8){ //represents 1 byte (255 as the max +  2(size and length)
    len = bytes[1]; //getting length from the second byte
    beg = 2; //tag, length = 2

  } else if (tag == PACK109_S16){
    len = ((bytes[1] << 8) |(bytes[2])); //uses similar logic of attaing length as tehe deserialization process for ints
    beg = 3; //tag, length (2 bytes) = 3
  } else{ //invalid tag
    throw;
  }

  //checking length
  if (bytes.size() < (2 + len)){ //if it is less than the length and tag (2) plus whatever the length is, it will throw an exception
      throw;
    }

  //deserialization process
  for (int i = 0; i < len; i++){
    result.push_back(bytes[i+beg]); //will append each bite from the vec bytes onto string result (adding 2 to skip over the tag and length)
  }

  return result;
}

//ARRAYS -----------------------------------------------------
//u8 ---------------------------------------------------------
vec pack109::serialize(std::vector<u8> item){
  vec bytes; 

  if (item.size() <= 255){ //A8
    bytes.push_back(PACK109_A8); //adding the tag
    bytes.push_back((u8)item.size()); //adds the size of the array
  } else if (item.size() > 255 && item.size() <= 65535){ //represnts A16
    bytes.push_back(PACK109_A16); //adding the tag
    bytes.push_back((item.size() >> 8) & 0xFF); //first part of bit representation
    bytes.push_back(item.size() & 0xFF); 
  }
  

  //will iterate through each bit in the vector
  for (int i = 0; i < item.size(); i++){
    bytes.push_back(PACK109_U8); //adding tag
    bytes.push_back(item[i]); //adds bite
  }

  return bytes;
}

std::vector<u8> pack109::deserialize_vec_u8(vec bytes){
  std::vector<u8> result;

  u8 tag = bytes[0]; //gets tag (first item
  int len; //gets number of items in array
  int place; //tracks location of where index is 
  if (tag == PACK109_A8){
    len = bytes[1]; //length takes up one byte
    place = 2;
  } else if (tag == PACK109_A16){
    len = (bytes[1] << 8 | bytes[2]); 
    place = 3;
  } else { //invalid tag
    std::cout << tag << std::endl;
    throw;
  }


  if (bytes.size() < (2 + len)){//includes tag and length of array
    throw;
  }

  if (bytes[place++] != PACK109_U8){
    throw;
  }

  for (int i = 0; i < len; i++){ //iterates thorugh num items in array
    result.push_back(bytes[place]); //adds it
    place += 2; //moves 2 places going past current bite and skipping next tag
  } 

  return result;
}

//u64 --------------------------------------------------------
vec pack109::serialize(std::vector<u64> item){
  vec bytes; 

  if (item.size() <= 255){ //A8
    bytes.push_back(PACK109_A8); //adding the tag
    bytes.push_back((u8)item.size()); //adds the size of the array
  } else if (item.size() > 255 && item.size() <= 65535){ //represnts A16
    bytes.push_back(PACK109_A16); //adding the tag
    bytes.push_back((item.size() >> 8) & 0xFF); //first part of bit representation
    bytes.push_back(item.size() & 0xFF); 
  }

  //will iterate through each bit in the vector
  for (int i = 0; i < item.size(); i++){
    bytes.push_back(PACK109_U64); //adding tag
    bytes.push_back((item[i] >> 56) & 0xFF); // 1st byte
    bytes.push_back((item[i] >> 48) & 0xFF); // 2nd byte
    bytes.push_back((item[i] >> 40) & 0xFF); // 3rd byte
    bytes.push_back((item[i] >> 32) & 0xFF); // 4th byte
    bytes.push_back((item[i] >> 24) & 0xFF); // 5th byte
    bytes.push_back((item[i] >> 16) & 0xFF); // 6th byte
    bytes.push_back((item[i] >> 8) & 0xFF); // 7th byte
    bytes.push_back((item[i]>> 0) & 0xFF); // 8th byte
  }

  return bytes;
}

std::vector<u64> pack109::deserialize_vec_u64(vec bytes){
  std::vector<u64> result;


  if (bytes.size() < (2 + 9)){ //tags and one element
    throw;
  }

  u8 tag = bytes[0]; //gets tag (first item
  int len; //gets number of items in array
  int place; //tracks location of where index is 
  if (tag == PACK109_A8){
    len = bytes[1]; //length takes up one byte
    place = 2;
  } else if (tag == PACK109_A16){
    len = (bytes[1] << 8 | bytes[2]); 
    place = 3;
  } else { //invalid tag
    throw;
  }

  if (bytes.size() < (2 + (len*9))){//includes tag and length of array * (tag + 8 bits)
    throw;
  }


  for (int i = 0; i < len; i++){ //iterates thorugh num items in array
    if (bytes[place] != 0xa4){ //ensures tag matches u_64 format
      throw;
    }
    place++; //incraments to go to the u64 val
    u64 int_u64 = ((u64)bytes[place] << 56|((u64)bytes[place + 1] << 48)|((u64)bytes[place + 2]<< 40)|((u64)bytes[place + 3]<< 32)|((u64)bytes[place + 4]<< 24)|((u64)bytes[place + 5] << 16)|((u64)bytes[place + 6]<< 8)|((u64)bytes[place + 7] << 0)); //concatenates the result of the the bytes (other than tag)
    result.push_back(int_u64);
    place += (8); //moves 8 places going past current bite 
  } 

  return result;
}

//f64 --------------------------------------------------------
vec pack109::serialize(std::vector<f64> item){
  vec bytes; 

  if (item.size() <= 255){ //A8
    bytes.push_back(PACK109_A8); //adding the tag
    bytes.push_back((u8)item.size()); //adds the size of the array
  } else if (item.size() > 255 && item.size() <= 65535){ //represnts A16
    bytes.push_back(PACK109_A16); //adding the tag
    bytes.push_back((item.size() >> 8) & 0xFF); //first part of bit representation
    bytes.push_back(item.size() & 0xFF); 
  }

  //will iterate through each bit in the vector
  for (int i = 0; i < item.size(); i++){
    bytes.push_back(PACK109_F64); //adding tag

    u64 float_to_int; //where the float will be stored as an int in order to do bit shifting operatiosn
    std::memcpy(&float_to_int, &item[i], sizeof(item[i])); //copies the specified number of bytes from one memory location to the other memory location regardless of the type of data stored

    bytes.push_back((float_to_int >> 56) & 0xFF); // 1st byte
    bytes.push_back((float_to_int >> 48) & 0xFF); // 2nd byte
    bytes.push_back((float_to_int >> 40) & 0xFF); // 3rd byte
    bytes.push_back((float_to_int >> 32) & 0xFF); // 4th byte
    bytes.push_back((float_to_int >> 24) & 0xFF); // 5th byte
    bytes.push_back((float_to_int >> 16) & 0xFF); // 6th byte
    bytes.push_back((float_to_int >> 8) & 0xFF); // 7th byte
    bytes.push_back((float_to_int >> 0) & 0xFF); // 8th byte 
  }

  return bytes;
}

std::vector<f64> pack109::deserialize_vec_f64(vec bytes){
  std::vector<f64> result;

  if (bytes.size() < (2 + 9)){ //tags and one element
    throw;
  }

  u8 tag = bytes[0]; //gets tag (first item
  int len; //gets number of items in array
  int place; //tracks location of where index is 
  if (tag == PACK109_A8){
    len = bytes[1]; //length takes up one byte
    place = 2;
  } else if (tag == PACK109_A16){
    len = (bytes[1] << 8 | bytes[2]); 
    place = 3;
  } else { //invalid tag
    throw;
  }

  if (bytes.size() < (2 + (len*9))){//includes tag and length of array * (tag + 8 bits)
    throw;
  }

  for (int i = 0; i < len; i++){ //iterates thorugh num items in array
    if (bytes[place] != 0xa9){ //ensures tag matches u_64 format
      throw;
    }
    place++; //incraments to go to the f64 val

    f64 temp; //making a temp to copy onto 

    u64 first = ((i64)bytes[place] << 56);
    u64 second = ((i64)bytes[place + 1] << 48);
    u64 third = ((i64)bytes[place + 2]<< 40);
    u64 fourth = ((i64)bytes[place + 3]<< 32);
    u64 fifth = ((i64)bytes[place + 4]<< 24);
    u64 sixth = ((i64)bytes[place + 5] << 16);
    u64 seventh = ((i64)bytes[place + 6]<< 8);
    u64 eigth = ((i64)bytes[place + 7] << 0);

    u64 int_to_float = (first|second|third|fourth|fifth|sixth|seventh|eigth); //concatenates the result of the the bytes (other than tag)
    std::memcpy(&temp, &int_to_float, sizeof(int_to_float)); //copies the specified number of bytes from one memory location to the other memory location regardless of the type of data stored

    result.push_back(temp);
    place += (8); //moves 8 places going past current bite 
  } 

  return result;
}

//STRING -----------------------------------------------------
vec pack109::serialize(std::vector<string> item){
  vec bytes;

  if (item.size() <= 255){ //A8
    bytes.push_back(PACK109_A8); //adding the tag
    bytes.push_back((u8)item.size()); //adds the size of the array
  } else if (item.size() > 255 && item.size() <= 65535){ //represnts A16
    bytes.push_back(PACK109_A16); //adding the tag
    bytes.push_back((item.size() >> 8) & 0xFF); //first part of bit representation
    bytes.push_back(item.size() & 0xFF); 
  }

  //will iterate through each string in the vector
  for (int i = 0; i < item.size(); i++){
    bytes.push_back(PACK109_S8); //adds string tag
    bytes.push_back(item[i].size()); //adds the number of chars in a particular string
    for (char c : item[i]){ //iterates through each char in the particular string
      bytes.push_back((u8) c); //adds the specific char from the string 
    }
  }
  return bytes;
}

std::vector<string> pack109:: deserialize_vec_string(vec bytes){
  std::vector<string> result;
  if (bytes.size() < 2){//includes tag and length of array
    throw;
  }

  u8 tag = bytes[0]; //gets tag (first item
  int len; //gets number of items in array
  int place; //tracks location of where index is 
  if (tag == PACK109_A8){
    len = bytes[1]; //length takes up one byte
    place = 2;
  } else if (tag == PACK109_A16){
    len = (bytes[1] << 8 | bytes[2]); 
    place = 3;
  } else { //invalid tag
    throw;
  }

  if (bytes.size() < (2 + len)){ //if it is less than the length and tag (2) plus whatever the length is, it will throw an exception
    throw;
  }

  //deserialization process
  for (int i = 0; i < len; i++){ //goes through each string in the vec. Ex. 3 strings, loops three times
    std::string temp_string; //add to string as incrament through bytes
    int string_len = bytes[place + 1]; //skips over tag of string and gets length of the string


    for (int j = 0; j < string_len; j++){
      temp_string.push_back(bytes[place + 2 + j]); //adding bytes to temp string 
    }

    result.push_back(temp_string); //adding tempstring to result
    place +=2 + string_len;
  }

  return result;
}

// vec pack109::serialize(struct Person item){
//   vec bytes;
//   bytes.push_back(PACK109_M8); //adding map tag
//   bytes.push_back(0x01); //adds the size of the map (1)

//   //Key Person
//   bytes.push_back(PACK109_S8); //adding map tag
//   bytes.push_back(0x06); //length of person
//   bytes.push_back('P');
//   bytes.push_back('e');
//   bytes.push_back('r');
//   bytes.push_back('s');
//   bytes.push_back('o');
//   bytes.push_back('n');

//   //Value
//   bytes.push_back(PACK109_M8); //another map 
//   bytes.push_back(0x03); //adds the number of data members (3)

//   //KV 1 = age
//   bytes.push_back(PACK109_S8); //adding map tag
//   bytes.push_back(0x03); //length of person
//   bytes.push_back('a');
//   bytes.push_back('g');
//   bytes.push_back('e');

//   bytes.push_back(0xa2);
//   bytes.push_back(item.age); //calls the person struct to acess age
  
//   //KV 2 = height
//   bytes.push_back(PACK109_S8); //adding map tag
//   bytes.push_back(0x06); //length of person
//   bytes.push_back('h');
//   bytes.push_back('e');
//   bytes.push_back('i');
//   bytes.push_back('g');
//   bytes.push_back('h');
//   bytes.push_back('t');

//   vec height_value = serialize(item.height); //calls the person struct to access height
//   for (u8 byte : height_value){
//     bytes.push_back(byte);
//   }

//   //KV 3 = name
//   bytes.push_back(PACK109_S8); //adding map tag
//   bytes.push_back(0x04); //length of person
//   bytes.push_back('n');
//   bytes.push_back('a');
//   bytes.push_back('m');
//   bytes.push_back('e');
//   bytes.push_back(0xaa);
//   string name_of_person_ser = item.name; //calls the person struct to access name
//   bytes.push_back(name_of_person_ser.size());
//   for (char c : name_of_person_ser){
//     bytes.push_back((u8) c); //will cast the char into a byte
//   }

//   return bytes;
// }

// struct Person pack109:: deserialize_person(vec bytes){
//   struct Person result;


//   u8 tag = bytes[0]; //gets tag (first item
//   int len; //gets number of items in array
//   int place; //tracks location of where index is 
//   if (tag == PACK109_M8){
//     len = bytes[1]; //length takes up one byte
//     place = 2;
//   } else if (tag == PACK109_M16){
//     len = (bytes[1] << 8 | bytes[2]); 
//     place = 3;
//   } else { //invalid tag
//     throw;
//   }


//   if (bytes[place++] != PACK109_S8){ //assuring the byte is the string tag in the correct location
//     throw;
//   }

//   //KEY - Person
//   int len_key = bytes[place++]; //length of key string
//   std::string key; //creating a temp string to add
//   for (int i = 0; i < len_key; i++){
//     key.push_back(bytes[place]);
//     place++;
//   }

//   //VALUE - 3 Map KV pairs
//   if (bytes[place++] != PACK109_M8){ //checking for correct map tag
//     throw;
//   }
  
//   int len_of_val = bytes[place++]; //get number of keyval pairs (should be 3)
//   if (len_of_val != 3){ //age, height, name
//     throw;
//   }

//   //AGE KV PAIR
//   //KEY
//   if (bytes[place++] != PACK109_S8){
//     throw;
//   }

//   int len_age = bytes[place++];
//   std::string age; //creating a temp for age
//   for (int i = 0; i < len_age; i++){
//     age.push_back(bytes[place]);
//     place++;
//   }

//   //VAL
//   if (bytes[place++] != PACK109_U8){
//     throw;
//   }
//   result.age = bytes[place++]; //adding the age onto the result here (one byte so just the current index in bytes)

//   //HEIGHT KV PAIR
//   //KEY
//   if (bytes[place++] != PACK109_S8){
//     throw;
//   }

//   int len_height = bytes[place++];
//   std::string height; //creating a temp string for height
//   for (int i = 0; i < len_height; i++){
//     height.push_back(bytes[place]);
//     place++;
//   }


//   //VAL
//   if (bytes[place++] != PACK109_F32){
//     throw;
//   }

//   f32 height_float; //where the height will be stored
//   u32 first = (bytes[place++] << 24);
//   u32 second = (bytes[place++] << 16);
//   u32 third = (bytes[place++]<< 8);
//   u32 fourth = (bytes[place++]<< 0);
//   u32 int_to_float = (first|second|third|fourth); //concatenates the result of the the bytes (other than tag)

//   std::memcpy(&height_float, &int_to_float, sizeof(int_to_float)); //copies the specified number of bytes from one memory location to the other memory location regardless of the type of data stored
//   result.height = height_float; //setting result value equal to the height derived

//   //NAME KEY VALUE PAIR
//   //KEY
//   if (bytes[place++] != PACK109_S8){ //verifys string tag
//     throw;
//   }

//   int len_name = bytes[place++];
//   std::string name; //creating a temp string for name
//   for (int i = 0; i < len_name; i++){
//     height.push_back(bytes[place]);
//     place++;
//   }

//   //VALUE
//   if (bytes[place++] != PACK109_S8){
//     throw;
//   }

//   int len_name_of_person = bytes[place++]; //getting length of the person's name
//   std::string name_of_person; //creating temp for name of the person
//   for (int i = 0; i < len_name_of_person; i++){
//     name_of_person.push_back(bytes[place]);
//     place++;
//   }

//   result.name = name_of_person; //setting result value equal to the name derived

//   return result; 
// }
//SERIALZING THE FILE STRUCT ----------------------------------
vec pack109::serialize(struct FileStruct item){
  vec bytes;
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x01); //1 KVP
  //key is File
  vec file = serialize(string("File"));
  bytes.insert(end(bytes), begin(file), end(file));
  //value is is an m8
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x02); //2 KVP
  //KVP 1 is "name"
  vec namek = serialize(string("name"));
  bytes.insert(end(bytes), begin(namek), end(namek));
  vec namev = serialize(string(item.name));
  bytes.insert(end(bytes), begin(namev), end(namev));
  //KVP 2 is "bytes"
  vec bytesk = serialize(string("bytes"));
  bytes.insert(end(bytes), begin(bytesk), end(bytesk));
  vec bytesv = serialize(item.bytes);
  bytes.insert(end(bytes), begin(bytesv), end(bytesv));

  return bytes;
}

//DESERIALZING THE FILE STRUCT ----------------------------------
struct FileStruct pack109::deserialize_file(vec bytes){
  if (bytes.size() < 8){ //making sure there is enough room for File to be the key
    throw;
  }

  vec file_slice = slice(bytes, 2, 7);
  string file_string = deserialize_string(file_slice);
  if (file_string != "File"){
    throw;
  }

  //file name
  u8 file_name_len = bytes[17]; //assuming file name length is fewer than 256 which is safe to assume
  vec file_namev = slice(bytes, 16, (16 + file_name_len + 1));
  string file_name = deserialize_string(file_namev);

  //file contents 
  int place = 16 + file_name_len + 1;
  place += 8; //skipping sting tag (1), length of bytes string (1), and chars that make up bytes (5), then get to tag 1+1+5=7+1 = 8
  u8 file_len = bytes[place + 1]; //assuming file is less than 256 
  vec file_contentsv = slice(bytes, place, (place + file_len + 1));

  std::vector<u8> file_bytes = deserialize_vec_u8(file_contentsv);
  struct FileStruct deserialized_file = {file_name, file_bytes};

  return deserialized_file;
}

//SERIALZING THE REQUEST STRUCT ----------------------------------
vec pack109::serialize(struct Request item){
  vec bytes;
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x01); //1 KVP
  //std::cout << bytes.size() << std::endl;
  //key is Request
  vec request = serialize(string("Request"));
  bytes.insert(end(bytes), begin(request), end(request));
  //value is is an m8
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x01); //2 KVP
  //KVP 1 is "name"
  vec namek = serialize(string("name"));
  bytes.insert(end(bytes), begin(namek), end(namek));
  vec namev = serialize(item.name);
  bytes.insert(end(bytes), begin(namev), end(namev));

  return bytes;
}

//DESERIALZING THE REQUEST STRUCT ----------------------------------
struct Request pack109::deserialize_request(vec bytes){
  if (bytes.size() < 11){ //making sure there is enough room for Request to be the key
    throw;
  }

  vec request_slice = slice(bytes, 2, 10);
  string request_string = deserialize_string(request_slice);
  if (request_string != "Request"){
    throw;
  }

  //file name
  u8 file_name_len = bytes[20]; //assuming file name length is fewer than 256 which is safe to assume
  vec file_namev = slice(bytes, 19, (19 + file_name_len + 1));
  string file_name = deserialize_string(file_namev);

  struct Request deserialized_request = {file_name};

  return deserialized_request;
}


//SERIALZING THE STATUS STRUCT ----------------------------------
vec pack109::serialize(struct Status item){
  vec bytes;
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x01); //1 KVP

  //key is Status
  vec status = serialize(string("Status"));
  bytes.insert(end(bytes), begin(status), end(status));
  //value is is an m8
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x01); //1 KVP
  //KVP 1 is "message"
  vec messagek = serialize(string("message"));
  bytes.insert(end(bytes), begin(messagek), end(messagek));
  vec messagev = serialize(item.message);
  bytes.insert(end(bytes), begin(messagev), end(messagev));

  return bytes;
}

//DESERIALZING THE STATUS STRUCT ----------------------------------
struct Status pack109::deserialize_status(vec bytes){
  if (bytes.size() < 10){ //making sure there is enough room for Status to be the key
    throw;
  }

  vec status_slice = slice(bytes, 2, 9);
  string status_string = deserialize_string(status_slice);
  if (status_string != "Status"){
    throw;
  }

  //message
  u8 message_len = bytes[22]; //assuming file name length is fewer than 256 which is safe to assume
  vec messagev = slice(bytes, 21, (21 + message_len + 1));
  string message = deserialize_string(messagev);

  struct Status deserialized = {message};

  return deserialized;
}


//PRINTING
void pack109::printVec(vec &bytes) {
  printf("[ ");
  for (int i = 0; i < bytes.size(); i++) {
    printf("%x ", bytes[i]);
  }
  printf("]\n");
}