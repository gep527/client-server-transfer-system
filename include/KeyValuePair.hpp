#include <cstring>
typedef std::string string;
class KeyValuePair{
    private:
        //data members
        string key;
        int value; 
    public:
        //constructor
        KeyValuePair(string k, int v){
            key = k;
            value = v;
        }
        //deconstructor
        ~KeyValuePair(){
            key = "";
            value = 0;
        }

        //accessor for Key
        string getKey(){
            return key;
        }

        //acessor for Value 
        int getValue(){
            return value;
        }

        //setter for Key
        void setKey(string k){
            key = k;
        }

        //setter for value
        void setValue(int v){
            value = v;
        }
};