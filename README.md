# Webserv

## Overview

Webserv is a project aimed at building a fully functional HTTP server in C++ 98. This project allows to understand the inner workings of web servers and the HTTP protocol, enhance knowledge of network programming, file handling, and server management.

## Objectives

- Implement a basic HTTP server that can handle requests and responses.
- Understand and use network programming concepts such as sockets and protocols.
- Learn to manage multiple connections using non-blocking I/O.

## Build Instructions

Use the provided Makefile to compile the project:
```bash
make all
```

## Usage
Run the server with a configuration file:

```bash
./webserv configuration_file
```

If no configuration file is provided, the server use a default configuration.

## Features

HTTP Server: Ability to parse and respond to HTTP requests.
Concurrency: Use of poll (or equivalents like select, kqueue, or epoll) to handle multiple connections simultaneously.
Non-blocking I/O: Implementation of non-blocking sockets to efficiently manage connections.
Configuration Flexibility: Ability to read server configurations from a file, allowing customization of server behavior.

## Configuration File

The server configuration should be customizable through a configuration file with directives similar to NGINX, enabling settings like:

Host and port settings.
Server names.
Error pages.
Maximum client body size.
Routes and redirections.
File serving directories.
CGI scripting support.

## HTTP Features

Methods: Handle basic HTTP methods such as GET, POST, and DELETE.
Redirections: Support for HTTP redirections.
File Upload: Ability to handle file uploads.

## Contributors

- @iamgrg

## License

![MIT License](https://img.shields.io/badge/license-MIT-green)
Distributed under the MIT License.
