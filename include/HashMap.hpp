#include <cstddef>
#include "../include/KeyValuePair.hpp"
#include "./linkedlist.hpp"
#include <cstring>
#include <vector>

class HashMap {
  private:
    // The backbone of the hash set: an array of linked list pointers for handling collisions
    List** array;
 
    // The number of buckets in the array
    int bucket_count; 

    // Total number of elements in the set
    size_t element_count;

    // Load factor threshold for resizing (default 70)
    unsigned int load_threshold;

    // The current load factor (average number of elements per bucket)
    unsigned int load_factor;

    // Resize the array by adjusting the number of buckets, rehashes all current elements
    void rehash(size_t new_size);

  public:
    // Initialize an empty hash set with a given number of buckets
    explicit HashMap(size_t initial_size);

    // Destructor: Free all allocated memory
    ~HashMap();

    // Generate a prehash for an item
    unsigned long prehash(string item) const;

    // Convert prehash value into a valid bucket index
    unsigned long hash(unsigned long prehash) const;

    // Insert item into the set. Returns true if inserted, false if already present.
    // Rehashes if inserting will increase the load factor past the threshold.
    bool insert(string key, File value);

    // Remove an item from the set. Returns true if removed, false if not found.
    bool remove(string key);

    // Check if the item exists in the set, and returns a keyvalue pair pointer if found
    KeyValuePair* contains(string item) const;

    //returns value associated with key. Returns true if found, false if not
    bool get(string key, File&value_out) const;

    // Return the number of elements in the hash set
    size_t count() const;

    // Return the current load factor as a percentage
    unsigned int load();

    // Set a new load factor threshold for resizing
    void set_load_threshold(unsigned int threshold);

    // Remove all elements from the hash set
    void clear();

    // Print the hash table (format is implementation-dependent)
    void print() const;
};