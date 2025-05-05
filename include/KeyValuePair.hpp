//intializing string for the key
#include <cstring>
typedef std::string string;
//initalizing the vector<v8> file for the value
#include <vector>
typedef unsigned char u8;
typedef std::vector<u8> File;

class KeyValuePair{
    private:
        //data members
        string key;
        File value; 
    public:
        //constructor
        KeyValuePair(string k, File v){
            key = k;
            value = v;
        }
        //deconstructor
        ~KeyValuePair(){
            key = "";
            value = {};
        }

        //accessor for Key
        string getKey(){
            return key;
        }

        //acessor for Value 
        File getValue(){
            return value;
        }

        //setter for Key
        void setKey(string k){
            key = k;
        }

        //setter for value
        void setValue(File v){
            value = v;
        }
};