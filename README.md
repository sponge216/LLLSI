# LLLSI - Low Latency Live Stream Infrastructure
LLLSI is an open source alternative infrastructure for developing livestream-dependent apps. 

I also wanted to make it as lightweight as possible, so I only used the cpp standard library and WinAPI.

These days, apps that depend on livestreams, such as screenshare, audio calls, etc, use an intermediate server that acts as a proxy for connected clients.

I always found that as a bit concerning, since the server's software is (99.9% of the time) either unknown or proprietary, you can never know what the server is actually doing with your session's data.

It could be sold for data brokers, or scanned for sensitive info (passwords, bank details , etc).

LLLSI offers an alternative to these apps. Using the many different interfaces, design patterns, and other complexity-abstracting layers that I've created in this infra, you as a developer can create your own livestsream app!
# What libraries does LLLSI include?
LLLSI includes libraries for:

Concurrency (threads, mutexes, etc)

Data Structures 

Graphics/Video

Mouse/Keyboard events

Networks (specifically a network interface for RTMP and its derived protocols, such as RTSMP)

Streams

And more :)

# Program's Phases
But, how does LLLSI overcome the need for an intermediate server? It uses NAT hole punching! The overall flow of the underlying program is divided into 2 phases:

## Phase 1 - Server-Clients-Host connection
Host connects to server ---> Host creates room ---> Clients join room ---> Host messages server to start NAT hole punching ---> Server distributes clients' addresses to host, and host's address to clients ---> Clients and host form P2P connection using hole punching.

Note: the server is ONLY used for the NAT hole punching and managing clients! It does not manage their livestream session.
## Phase 2 - Host-Clients P2P livestream
This phase is where the actual livestream begins. The host sends over an RTSMP socket a livestream of the developer's choice to the clients, who receive and consume said livestream.

# Files Structure
RDIBY - holds every .h/.cpp file that belongs to the infrastructure.

RDIBYClient - test for client-server-host connection.

VideoHost - example application. Host app that implements the infra's headers/cpp files and used them to build a screenshare + remote control app.

VideoClient -  example application. Client app that implements the infra's headers/cpp files and used them to build a screenshare + remote control app.


# Interfaces and Design Patterns

## stream.h
The stream.h header has stream-related interfaces and abstract classes, which allow developers to easily develop generic stream objects of their choice.

### Stream 
abstract class for stream objects, every stream needs to implement it.

### VectorStream 
abstract class for stream objects that use a vector-type StreamContainer.

### MapStream 
abstract class for stream objects that use a map-type StreamContainer.

### StreamContainer 
interface for stream containers, which are the data structures that manage the stream's data.

### StreamVectorContainer 
interface for vector-type stream containers.

### StreamMapContainer 
interface for map-type stream containers.

### StreamData 
interface for objects that can act as a stream object's datatype.

The IActionListener.h header holds a mediator design pattern, which helps mediate between different classes/sections of a project that aren't necessarily supposed to interact directly with each other.

If you'd like to read a more in-depth explanation of my project, such as the protocols, how I implement screenshare, nat hole punching, etc,  here is a link to the [project doc](https://docs.google.com/document/d/1qst4x2pdtFSPdvGqOZ__rwQHS6JzSB9mjDP2nICv8rk/edit?usp=sharing).
