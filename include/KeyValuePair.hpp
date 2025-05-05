class KeyValuePair{
    private:
        //data members
        int key;
        int value; 
    public:
        //constructor
        KeyValuePair(int k, int v){
            key = k;
            value = v;
        }
        //deconstructor
        ~KeyValuePair(){
            key = 0;
            value = 0;
        }

        //accessor for Key
        int getKey(){
            return key;
        }

        //acessor for Value 
        int getValue(){
            return value;
        }

        //setter for Key
        void setKey(int k){
            key = k;
        }

        //setter for value
        void setValue(int v){
            value = v;
        }
};