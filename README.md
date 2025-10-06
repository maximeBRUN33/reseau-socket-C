# Reseau Socket C — Chat Server/Client

This project was a class project during 3rd year at INSA Lyon.

### Overview

Simple multi-client chat using TCP sockets with ephemeral in-memory history on the server. Features include public broadcast, private messaging, group creation/membership, and a small usage guide automatically sent to new clients on connect.

Edit - Oct 2025 :  The project is now supported to compile and run on MacOS/Linux

### Repository layout

- Client/: interactive terminal client
- Serveur/: server and chat history logic
- Makefile: cross-platform build (macOS/Linux/Windows MinGW)

### Build

Requirements: gcc/clang and make.

- macOS/Linux:
  - make
  - Outputs: Serveur/serveur and Client/client

- Windows (MinGW/MSYS):
  - make (links ws2_32 automatically)

You can force platform detection if needed:

- make OSNAME=Darwin
- make OSNAME=Linux

### Run

1) Start the server

- ```./Serveur/serveur```
- Stop: press ```Enter``` in the server terminal (graceful) or ```Ctrl+C```.

2) Start one or more clients

- ```./Client/client <server_ip> <your_name>```
- Example: ```./Client/client 127.0.0.1 Alice```

On connect, the server sends you:

- recent history (ephemeral, server memory only)
- a short usage guide with all commands

**Client commands**

- Public message: type your message and press Enter
- Private message: ```Private to <UserName> : <message>```
- List clients: ```See Clients```
- List groups: ```See Groups```
- Create group: ```Group <GroupName> : <User1>, <User2>```
- Send to group: ```To <GroupName> : <message>```
- Add to group: ```Add to <GroupName> : <UserName>```
- Remove from group: ```Remove from <GroupName> : <UserName>```
- Quit client: ```quit```

**Notes**

- History is ephemeral: restarting the server clears it.
- Only public messages and connect/disconnect actions are saved to history.
- Group membership is not restricted: any client can add/remove members.

**Implementation details**

- Server: TCP socket accept loop with select(), manages clients, groups, and history.
- History: in-memory ring buffer in Serveur/history.c.
- Cross-platform: Windows uses winsock (linked via -lws2_32); macOS/Linux use POSIX sockets. Headers are selected at compile time in Client/client2.h and Serveur/server2.h.

**Troubleshooting**

- Port in use: change PORT in Serveur/server2.h and Client/client2.h or stop the other process.
- Build issues on Windows: ensure you run in MSYS/MinGW and have ws2_32 available.
