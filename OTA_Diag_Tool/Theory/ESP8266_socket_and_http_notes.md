# ESP8266 Server Basics: ESP calls vs Linux socket calls

This note explains the flow behind a simple ESP8266 TCP/HTTP server in a beginner-friendly way and maps the Arduino/WiFi API calls to the equivalent Linux socket syscalls.

---

## 1) Beginner-friendly explanation

A typical ESP8266 server does this:

1. Connect to Wi-Fi
2. Start a listener on a TCP port
3. Wait for a client to connect
4. Read the request that the client sends
5. Decide what to do based on the request
6. Send a response back
7. Close the client connection

The most common Arduino-style calls are:

- `WiFiServer server(80);`
- `server.begin();`
- `WiFiClient client = server.available();`
- `client.connected();`
- `client.available();`
- `client.readStringUntil('\r');`
- `client.println("HTTP/1.1 200 OK");`
- `client.stop();`

These are just high-level wrappers around the same underlying socket model used in Linux.

---

## 2) Raw server example (ESP8266)

```cpp
#include <ESP8266WiFi.h>

const char* ssid = "ESP8266_AP";
const char* password = "12345678";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.println(WiFi.softAPIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  while (client.connected()) {
    if (client.available()) {
      String request = client.readStringUntil('\r');
      Serial.println(request);

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("Hello from ESP8266");
      break;
    }
  }

  client.stop();
}
```

### What this code does

- `server.available()` waits for a client to connect
- `client.available()` checks if the client has sent data
- `client.readStringUntil('\r')` reads the HTTP request
- `client.println(...)` sends the HTML/text response
- `client.stop()` closes the socket

---

## 3) Side-by-side: ESP8266 call vs equivalent Linux socket call

| ESP8266 Arduino call | Equivalent Linux socket call | Meaning |
|---|---|---|
| `WiFiServer server(80);` | `socket(); bind(); listen();` | Create a listening socket and bind it to port 80 |
| `server.begin();` | `listen(sockfd, backlog);` | Start listening for incoming connections |
| `WiFiClient client = server.available();` | `accept(sockfd, ...);` | Accept a new connection from a client |
| `client.connected()` | `recv()`, `poll()`, or `select()` | Check if the socket is still alive |
| `client.available()` | `recv()` / `read()` / `poll()` | Check whether data is waiting to be read |
| `client.readStringUntil('\r');` | `recv(clientfd, buf, ...);` | Read the request bytes from the socket |
| `client.println("HTTP/1.1 200 OK");` | `send(clientfd, ...);` / `write(clientfd, ...);` | Send a response to the client |
| `client.stop();` | `shutdown(); close();` | Close the client connection |

---

## 4) Equivalent Linux socket flow

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(serverfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(serverfd, 5);

    int clientfd = accept(serverfd, NULL, NULL);

    char buffer[256];
    recv(clientfd, buffer, sizeof(buffer), 0);

    const char* response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Connection: close\r\n\r\n"
                           "Hello from Linux socket server\n";

    send(clientfd, response, strlen(response), 0);

    shutdown(clientfd, SHUT_RDWR);
    close(clientfd);
    close(serverfd);

    return 0;
}
```

This is the same logic as the ESP8266 example, just without the Arduino wrapper.

---

## 5) One-line mapping for beginners

- `server.available()` = `accept()`
- `client.available()` = `recv()` / `read()`
- `client.connected()` = check if socket is still valid
- `client.stop()` = `close()`

---

## 6) Simple mental model

Think of the server like this:

- `socket()` = open a communication door
- `bind()` = assign port number to that door
- `listen()` = wait for someone to knock
- `accept()` = open the door when someone knocks
- `recv()` = read what the client says
- `send()` = reply back
- `close()` = shut the connection

The ESP8266 Arduino API hides these lower-level steps, but the backend behavior is the same.

---

## 7) Full lifecycle diagram (plain text)

```text
Power ON
  -> Connect to Wi-Fi
  -> Create server socket
  -> Start listening on port
  -> Wait for client
     -> server.available() = accept()
     -> client connected?
        -> yes
           -> client.available() = data waiting?
              -> yes -> read request
              -> no  -> wait
           -> decide command
           -> send response using send()/write()
           -> client.stop() = close()
        -> no
           -> end connection
  -> Repeat loop
```

---

## 8) Important takeaway

The ESP8266 `WiFiServer` API is a convenient C++ wrapper over the Linux socket model. It is not a completely different concept; it is the same idea with easier names.

When you understand this mapping, you can reason about networking more easily and apply the same ideas to Linux, embedded Linux, and other networked devices.
