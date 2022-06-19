# WebRTC-echo
This project is a playground for playing with libdatachannel library (which provides C++ WebRTC APIs)

Disclaimer: This project uses heavy reference from examples folder of above library. This is indented for pure-educational purposes, to understand WebRTC internals.

### Purpose
This project builds two binaries, server and client.

By default, server and client won't talk to each other. The desription from one(server or client) needs to be provided to the other in order to establish a session. See SDP on MDN

Once they are connected, the messages you sent via client are looped back from server.


### Building
This project requires C++17 supported-compiler and libdatachannel shared library. Change the path for shared library for recompiling this project.

