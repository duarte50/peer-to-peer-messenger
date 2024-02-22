# Peer-to-peer messenger

Simple messenger application using the peer-to-peer architecture in C.

## Installation

Use make to compile the code.

```
make messenger
```

## Usage

Run the executable file.

```
./messenger
```

Set up the local server by binding an unused TCP port number.
```
- Configure local server
Port:
```

Connect to another peer by typing its host name and port.

```
Hostname:
Host port:
```

Type the message to be sent.

```
Message to [localhost:3030]: Hello World!
Message sent
```

The other peer receives the message and the peer host name and port.

```
[127.0.0.1:3031] sent: Hello World!
```

## License

This project is licensed under the GNU General Public License v3.0.
