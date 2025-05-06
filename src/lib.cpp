#include <string>
#include <cmath>
#include <iostream> 
#include <cstdint>
#include <cstring>
#include "../include/HashMap.hpp"
#include <vector>

    /*
      The `rehash` function resizes the hash table by creating a new array with a different number of buckets,
      and rehashes all existing elements, redistributing them across the new buckets to maintain a balanced load.

      Steps:
      1. Create a new array of buckets with the specified new size.
      2. Initialize each bucket in the new array as an empty linked list.
      3. Traverse each bucket in the original array:
          a. For each bucket, get the head of the linked list.
          b. Get the item from the current node
          c. Calculate the new index for this item based on the hash function
          d .Insert the item into the appropriate bucket in the new array
          e. Move to the next node in the linked list
      4. Free the memory allocated for the old array and its buckets.
      5. Update the hash table's properties:
          a. Point the `array` pointer to the new array.
          b. Update the `bucket_count` to the new size.
    */


    // Resize the array by adjusting the number of buckets, rehashes all current elements
    void HashMap::rehash(size_t new_size) {
      // Step 1: Create a new array of buckets with the specified size
      List** new_array = new List*[new_size];
      
      // Step 2: Initialize all buckets in the new array as empty linked lists
      for (size_t i = 0; i < new_size; i++) {
          new_array[i] = new List;       // Create a new List object for each bucket
          initList(new_array[i]);        // Initialize the new List (set up the head to nullptr)
      }
      
      // Step 3: Traverse each bucket in the original array
      for (int i = 0; i < bucket_count; i++) {
          // Get the head of the current bucket's linked list
          Node* current = array[i]->head;
          
          // Step 3a: For each bucket, get the head of the linked list.
          while (current != NULL) {
              // Step 3b: Get the item from the current node
              KeyValuePair* kvp = (KeyValuePair*)current->item; //FOR KVP: casting the current node as a key value pair
              
              // Step 3c: Calculate the new index for this item based on the hash function
              size_t new_index = hash(prehash(kvp->getKey())) % new_size; //FOR KVP: finding new index of key
              
              // Step 3d: Insert the item into the appropriate bucket in the new array
              insertAtHead(new_array[new_index], kvp); //FOR KVP: inserting the new key value pair
              
              // Step 3e. Move to the next node in the linked list
              current = current->next;
          }
      }
      
      // Step 4: Free the memory of the old buckets (assuming nodes are freed elsewhere)
      for (int i = 0; i < bucket_count; i++) {
          delete array[i];  // Delete the old linked list
      }
      
      // Step 5: Update the hash table properties
      delete[] array;    // Free the old array of buckets
      array = new_array; // Point to the new array
      bucket_count = new_size; // Update the bucket count to the new size
    }

    /*
    Example of rehashing:
    Before rehashing, we have a hash table with 5 buckets and the following items:

    ---------------------------------
    Bucket | Linked List
    ---------------------------------
      0    |  5 → 18 → nullptr
      1    |  nullptr
      2    |  12 → nullptr
      3    |  17 → nullptr
      4    |  99 → nullptr
    ---------------------------------

    After rehashing to 10 buckets:

    Assume the new hash indices:
    - 5 → 0
    - 18 → 8
    - 12 → 2
    - 17 → 3
    - 99 → 9

    ---------------------------------
    Bucket | Linked List
    ---------------------------------
      0    |  5 → nullptr
      1    |  nullptr
      2    |  12 → nullptr
      3    |  17 → nullptr
      4    |  nullptr
      5    |  nullptr
      6    |  nullptr
      7    |  nullptr
      8    |  18 → nullptr
      9    |  99 → nullptr
    ---------------------------------
    */



    HashMap::HashMap(size_t initial_size) { 
      array = new List*[initial_size];  // Allocate memory for the array of linked lists
      bucket_count = initial_size;     // Set the number of buckets
      element_count = 0;               // No elements yet
      load_threshold = 70;             // Default load threshold
      load_factor = 0;                 // Load factor is 0 initially
      
      // Initialize each bucket as an empty linked list
      for (int i = 0; i < bucket_count; i++) {
        List* listElement = new List;   // Create a new linked list for this bucket
        array[i] = listElement;         // Assign the list to the current bucket
        initList(listElement);          // Initialize the linked list
      }
    }

    HashMap::~HashMap(){
      delete [] array; // clears memory of the HashSet array 
    }

    
    size_t HashMap::count() const{
      return element_count;
    }
    
    void HashMap:: set_load_threshold(unsigned int threshold){
      load_threshold = threshold;
    }
    
    unsigned long HashMap::hash(unsigned long prehash) const {
      return prehash%bucket_count;
    }

    /*
    The `prehash` function generates a hash value for an integer by processing each byte of the integer.
    The function uses a bit-shifting operation to generate a hash that can be used for indexing in a hash table.
  
    The constant 5381 is the starting value of the hash, and it is chosen because prime numbers like it are commonly used in hashing algorithms to reduce collisions.
    */

    unsigned long HashMap:: prehash(string item) const{
        unsigned long h = 5381; // Starting hash value, chosen for its good distribution properties in hashing algorithms
        //unsigned char* ptr = reinterpret_cast<unsigned char*>(&item); // Cast the integer to a sequence of bytes

        // Loop through each byte (digit) of the integer and update the hash value
        for (char c : item){
            h = ((h << 5) + c);
        }
        // for (size_t i = 0; i < item.size(); ++i) {
        //     unsigned char c = ptr[i]; // Get the byte at position i
        //     h = ((h << 5) + h) + c;   // Update the hash value: h * 33 + c
        // }

        return h; // Return the final computed hash value
    }

    //=========================================================================

    /*The `insert` function adds a new integer to the hash set **only if it's not already present**.
    
    Steps:
    1. Hash the value to figure out which bucket it belongs in.
    2. Check if it's already in the linked list at that bucket.
    3. If it's not, insert it at the front of the list.
    4. Increase the element count.
    5. If the average number of elements per bucket (load factor) exceeds the threshold,
       double the size of the hash table and rehash all elements.
    */
  
    bool HashMap:: insert(string key, File value) {
      int index = hash(prehash(key)); //Compute the bucket index
      KeyValuePair* kvp = contains(key); //finds the pointer to the Key Value Pair if it is in the HashSet
    
      if (kvp != NULL) { //if it is in the HashMap, this will return true
        kvp->setValue(value); //changes the existing value (File) to a new value (new File)
        return false; //if already in the HM
      } else{ //if it is not in the HashMap already
        insertAtHead(array[index], new KeyValuePair(key, value)); // Insert at head of list
        element_count++; //Track how many elements we have
    
        if (load() > load_threshold) { // Step 5: Resize if necessary
          rehash(bucket_count * 2);
        }
      }
      return true; //return if not already in HM
    }
    /*
    Suppose we insert value 42 and it hashes to bucket 3:
    
    Before inserting:
    
    ---------------------------------
    Bucket | Linked List
    ---------------------------------
      0    |  5 → 18 → nullptr
      1    |  nullptr
      2    |  12 → nullptr
      3    |  nullptr
      4    |  99 → nullptr
    ---------------------------------
    
    After inserting 42 into bucket 3:
    
    ---------------------------------
    Bucket | Linked List
    ---------------------------------
      0    |  5 → 18 → nullptr
      1    |  nullptr
      2    |  12 → nullptr
      3    |  42 → nullptr
      4    |  99 → nullptr
    ---------------------------------
    
    Now insert 17, which also hashes to bucket 3:
    
    ---------------------------------
    Bucket | Linked List
    ---------------------------------
      0    |  5 → 18 → nullptr
      1    |  nullptr
      2    |  12 → nullptr
      3    |  17 → 42 → nullptr
      4    |  99 → nullptr
    ---------------------------------*/

    //=========================================================================

    /*The `remove` function deletes an integer from the hash set **if it exists**.

      Steps:
      1. Hash the value to find the bucket it belongs in.
      2. Check if it exists in the linked list at that bucket.
      3. If it does, remove it from the list.
      4. Decrease the element count.
    */


    bool HashMap:: remove(string key) {

      int index = hash(prehash(key)); //Find the bucket
      KeyValuePair* kvp = contains(key); //finds the pointer to the Key Value Pair if it is in the HashSet

      int i = 0; //used to find the index of item being removed in the bucket
      if (kvp != NULL) { //if the kvp is found

        Node* current = array[i]->head;//used to iterate through the bucket to find the item being removed
        while(current != NULL){ //iterate the whole LL
          KeyValuePair* pair = (KeyValuePair*)(current->item); //casting as a KVP
          if (key == pair->getKey()){ 
            break; //meaning key is found at that value of i
          }
          i++;//incraments count as it moves along the LL
          current = current->next; //iterating
        }

        removeAtIndex(array[index], i); // Remove it at index
        element_count--;                // Update count
        return true;
      } else { 
        return false; // Item wasn't in the set
      }
    }

    /*
    Suppose we remove 42, which is currently in bucket 3:

    Before removal:

    ---------------------------------
    Bucket | Linked List
    ---------------------------------
      0    |  5 → 18 → nullptr
      1    |  nullptr
      2    |  12 → nullptr
      3    |  17 → 42 → nullptr
      4    |  99 → nullptr
    ---------------------------------

    After removing 42:

    ---------------------------------
    Bucket | Linked List
    ---------------------------------
      0    |  5 → 18 → nullptr
      1    |  nullptr
      2    |  12 → nullptr
      3    |  17 → nullptr
      4    |  99 → nullptr
    ---------------------------------
    */

    bool HashMap::get(string key, File&value_out) const{
      int index = hash(prehash(key)); //Hash to find the bucket
    
      Node* current = array[index]->head; //Will get the linkedlist and set the head equal to the current node
      while (current != NULL){ //will iterate through the LL
        KeyValuePair* kvp = (KeyValuePair*) current->item; //casts as KVP
        if (kvp->getKey() == key){
          value_out = kvp->getValue(); //having the value be value associated with the key
          return true; //returns true if found
        }
        current = current->next; //iterates through LL
      }
      return false; //Return the result

    }
    //=========================================================================

    /*The `contains` function checks if an integer is in the hash set.
      Used as a helper method for the insert and remove functions
      will return a Key Value Pair pointer 
    */

    KeyValuePair* HashMap::contains(string key) const{ 
      int index = hash(prehash(key)); //Hash to find the bucket
      Node* current = array[index]->head; //Will get the linkedlist and set the head equal to the current node 
      while (current != NULL){ //will iterate through the LL
        KeyValuePair* kvp = (KeyValuePair*) current->item; //casts as KVP
        if (kvp->getKey() == key){
          return kvp; //if found in the LL, will return the KVP
        }
        current = current->next; //iterate through the LL
      }
      return NULL; //Return null if not found
    }



    /*
    Let’s check if 17 is in the set.

    ---------------------------------
    Bucket | Linked List
    ---------------------------------
      0    |  5 → 18 → nullptr
      1    |  nullptr
      2    |  12 → nullptr
      3    |  17 → nullptr
      4    |  99 → nullptr
    ---------------------------------

    - 17 hashes to bucket 3.
    - We find it in the list: returns true.

    Now check if 88 is in the set.

    - 88 hashes to bucket 2.
    - List at bucket 2 is: 12 → nullptr
    - 88 is not there: returns false.
    */

    //=========================================================================


    /*The `load` function calculates the current **load factor** of the hash set.

      - The load factor is the average number of elements per bucket, expressed as a percentage.
      - If the load factor goes above the threshold, the hash table will resize.

      Steps:
      1. Count the total number of items across all buckets.
      2. Divide by the number of buckets.
      3. Multiply by 100 and round to get a percentage.
    */

    unsigned int HashMap:: load(){
      double total_elements = 0;

      for (int i = 0; i < bucket_count; i++) {
        int llist_elements = size(array[i]); // Count items in each list
        total_elements += llist_elements;
      }

      int curr_load = round((total_elements / bucket_count) * 100); // Compute percentage
      load_factor = curr_load; // Update internal variable
      return curr_load;        // Return result
    }

    /*
    Suppose we have 5 buckets and the following contents:

    ---------------------------------
    Bucket | Linked List
    ---------------------------------
      0    |  5 → 18 → nullptr      (2 items)
      1    |  nullptr               (0 items)
      2    |  12 → nullptr          (1 item)
      3    |  17 → nullptr          (1 item)
      4    |  99 → nullptr          (1 item)
    ---------------------------------

    Total elements = 2 + 0 + 1 + 1 + 1 = 5
    Number of buckets = 5

    Load factor = (5 / 5) * 100 = 100%

    If the load threshold was 70%, this would trigger a rehash.
    */

    //=========================================================================

    void HashMap::clear(){
      for(int i=0;i<bucket_count;i++){ 
        initList(array[i]);
      }
      load_factor=0; 
      element_count=0; 
    }

    //NEW METHOD FILE GET
    File HashMap::get(string key){
      int index = hash(prehash(key)); //hash to find the bucket

      bool notFound = true;
      File value_out; //stores the File if found for associated key

      Node* current = array[index]->head; //will get the LL and set the head equal to the first value
      while (current != NULL){ //will iterate throught the LL

        KeyValuePair* kvp = (KeyValuePair*)current->item; //casts as KVP
        if (kvp->getKey() == key){
          value_out = kvp->getValue(); //getting the value associated with the key
          notFound = false; //sets it equal to false, because it was found
        }

        current = current->next; //iterates 
      }

      if (notFound){ //if not found on the list (never set to false)
        throw; //throws an exception
      } else{ //found
        return value_out; //returns associated value
      }
    }

    void HashMap::print() const{
      for(int i=0;i<bucket_count;i++){//will iterate through each bucket
        std::cout <<"Bucket: " << (i + 1) << ":";
        Node* current = array[i]->head; //will find the LL at the bucket and set the current node equal to the head

        if (current == NULL){//if the bucket is empty, it will print that it is an empty bucket 
          std::cout << " Empty List";
        }

        while (current != NULL){ //if not it will print everything in the bucket
          KeyValuePair* pair = (KeyValuePair*)(current->item);
          std::cout << " [ " << pair->getKey()<< ", "; //prints the key (string)

          //prints key
          File fileTemp = pair->getValue(); //gets value and stores it on a temp val
          std::cout << "{ ";
          for (int i  = 0; i < fileTemp.size(); i++){ //prints each value in the File value
            std::cout << fileTemp[i]; 
          }
          std::cout  << "} ]";

          current = current->next; //iterate through the LL
        }
        std::cout << " " << std::endl;
        printf("\n");
      }
    }