# File Server System

A client-server file sharing system with custom serialization protocol and encryption.

## Features

- **Client-Server Architecture**: TCP-based communication
- **Custom Protocol**: Pack109 serialization format
- **Encryption**: XOR encryption for message security
- **File Storage**: HashMap-based file storage with persistence
- **File Operations**: Send, request, and manage files

## Project Structure
```
client-server-transfer-system/
├── include/
│   ├── HashMap.hpp
│   ├── KeyValuePair.hpp
│   ├── linkedlist.hpp
│   └── pack109.hpp
├── src/
│   ├── client.cpp
│   ├── server.cpp
│   ├── test.cpp
│   ├── pack109lib.cpp
│   ├── linkedlistlib.cpp
│   └── lib.cpp
├── files/
│   ├── HMdata.txt
│   ├── document.txt
│   ├── file.txt
│   └── testing.txt
├── bin/ 
├── Makefile
├── .gitignore
└── README.md
```
