# Simple IRC Server

This project involves creating a simple IRC (Internet Relay Chat) server in C++. 
The goal is to develop a non-blocking server (no forking), using only one poll, that is fully compatible with an official IRC client, providing a platform for real-time text messaging in a network.

## Features

- **Compatibility:** tested with IRSSI and netcat. (File transfer is handle by IRSSI client)
- **Channels:** Support for multiple channels.
- **Operators:** Multiple operators and user features: KICK, INVITE, TOPIC, MODE (i, t, k, o , l).
- **Private Messaging:** Allows private messaging between users.
- **User Authentication:** Basic user authentication process.
- **Robustness:** Handles network errors and user disconnections gracefully.

## Prerequisites

- C++ compiler (clang++)
- Make (for building the project)

## Run

- /ircserv `<port>` `<password>`

`<port>`: listening port
`<password>`: server password
