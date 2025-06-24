
# Webserv

webserv is a simple HTTP web server it support multiple virtual hosts configured via a .conf file.
file.conf is based on ...
# use

```bash
make
./webserv {file.conf}  
```

# test

To test our process, start the server using the test configuration file or execute the following curl command:

```bash
make
./webserv {file.conf}  
```

// TODO 
```bash
./webserv users/multipleServers.conf

curl -H "Host: lol" http://localhost:8080
curl -H "Host: test" http://localhost:8080
``` 

```mermaid

classDiagram

%%====open questions=====%%
%%hoe bufferen we read en write van requests en responses?
%%waar SessionManagement en cookies?
%%myConfig meegeven aan alles of hoe doen we dat? : MyConfig becomes a singleton, but it gets passed on by reference

%%====classes=====%%
class ServerConfig {
		+std::string host;
		+std::string port;
		+std::string root;
		+std::vector<std::string> server_names;
		+std::map<int, std::string> error_pages;
		+size_t client_max_body_size;
		+std::vector<Route> routes;

		+Route()
}

class ConfigParser {
   -vector ServerConfigs
    -...
    
    +ConfigParser
    +~ConfigParser    
    +parseConfigFile() std::vector <ServerConfig> 
   
}

class MyConfig {
	<<singleton>> 
    +std::vector <ServerConfig> servers
    +~MyConfig()
    +static get()$
    +getServerConfig(const std::string &serverKey, const std::string &hostURL)
    +findServerConfig(const std::string &serverKey, const std::string &hostURL)
    
    -ctor() and ctor(const char *filename)
    -static MyConfig *_myConfig

}

%%class HTTPServerBuilder {
%%<<builder>>
%%    -HTTPServer *HTTPServer
%%    -MyConfig &MyConfig 
%%
%%    -HTTPServerBuilder()
%%    -~HTTPServerBuilder()
%%    -socketSetup(struct MyConfig) int socketFd
%%        %% create socket
%%    -epollSetup(struct MyConfig)
%%       %%  add socket fd to epoll instance
%%        %% epoll_ctl is called in this fct, the only thing needed for httpConnection is the epoll_fd
%%    +build()
%%}

class HTTPServer {
    -int socketFD
        %% your server gets one FD and the client gets another, so you only have to worry about one
        %% wether you are writing or reading will be managed by the apoll api
    -int epollFD
    -std::vector < ConnectionHandler > Connections
        %% in order to make it non blocking
    -...

        %% create socket
       %%  add socket fd to epoll instance
        %% epoll_ctl is called in this fct, the only thing needed for httpConnection is the epoll_fd
    +HTTPServer()
    +~HTTPServer()
    +init()
    +start()  
    +stop() 
    %% basically signal handler, free necessary stuff
}

%%The normal procedure for parsing an HTTP message is to read the start-line into a structure, 
%%read each header field line into a hash table by field name until the empty line, and then use the parsed data to determine if a message body is expected.
%%If a message body has been indicated, then it is read as a stream until an amount of octets equal to the message body length is read or the connection is closed.
%%src: https://www.rfc-editor.org/rfc/rfc9112

class ConnectionHandler {
    -HTTPRequest
    -MyConfig &MyConfig 
    -HTTPAction *HTTPAction
    -std::string requestBuffer
        -receiveRequest(int socketFDs) HTTPRequest
        %%We need a kind of buffer if we can't read an entire request at once
        %%call read again inside this method?
        %% has to handle chunking: set requestEndReached to true if final chunk is 0 bytes
        %%      => it will show in the HTTP Request header if it's a chunk
        %%      => just keep adding inside of receiveRequest
        %% also error check within this
    -createResponse(int socketFD, HTTPResponse)

    -...
            
    +ctor() and dtor()
    +handleHTTP()
    +shouldClose()
    +sentdCgiResponse()
    +shouldClose()
    +isCgiRunning()
    +Cgi *getCgi()

}

class HTTPRequest {
    -std::string method
    -std::string requestTarget
    -std::string hostUrl
    -std::string body
    -const std::string HTTPVersion = "1.1"
    -vector std::string headers

    +HTTPRequest()
    +getters() ...
    +setters() ...
    +parseRequest(std::string rawMessage) void
    %% request line   = method WHITESPACE request-target WHITESPACE HTTP-version: GET /pub/WWW/TheProject.html HTTP/1.1
    %% host line   = host url: Host: www.example.org
    +rest()
    +hasCloseHeader

}

class HTTPResponse {
    -int statusCode
    -const std::string HTTPVersion = "1.1"
    -std::string body
    -std::string statusText
    -map std::string headers
    -std::string responseString


    +HTTPResponse()
    -setStatusMessage()
    +setBody()
    +buildResponse() const std::string
    %%  status-line = HTTP-version SP status-code SP [ reason-phrase ]
    %%  ...?
    %%  body
	  +reset()
	  +setters() ...
}

class HTTPAction  {
    -ctor()
    -HTTPRequest
    -HTTPResponse
    
    -std::vector handlers
    -MethodRegistry
    -Cgi *_cgi
    -...
    -runCGI()
       %% server should be non blocking, so if a cgi call takes a while and the response is not ready, and your server is waiting, this can be a problem, because you can't wait
       %%Flynn: Een pipe die uw child process met uw parent process verbind (voor de response te accepteren van de child proces), zijn read end bij epoll te registreren en dan read hetzelfde zoals inkomende requests 

    +ctor(...)
     +run()
     +bool cgiIsRunning()
     +Cgi *getCgi
}

class MethodRegistry {
		-std::map <std::string, AMethod*> methods
		+ctor
		+dtor
		+createMethodInstance()
}

class HTTPhandleGET  {
    +ctor() and dtor()    
    +static AMethod *create()
    +implementMethod() override
    -...

}

class HTTPhandlePOST {
    +ctor() and dtor()    
    +static AMethod *create()
    +implementMethod() override
    -...
}

class HTTPhandleDEL {
    +ctor() and dtor()    
    +static AMethod *create()
    +implementMethod() override

}

class AMethod {
		<<interface>>
		+implementMethod()
}

%%class SessionManager {
%%    +SessionManager()
%%}

class Cgi {
    +ctor  & dtor
    +startCgi()
    +processCgi()
    +getStatusCode()
    +getBody()
    +getCgiFds()
    -...
}

class Path {
    +...
}


%%====connections=====%%

ConfigParser *-- ServerConfig : owns a vector of
MyConfig *-- ServerConfig : owns a vector of 
MyConfig ..> ConfigParser : uses (stack) 
MyConfig <.. ConnectionHandler : uses
MethodRegistry --> AMethod : creates
ConnectionHandler *-- HTTPRequest : owns
ConnectionHandler ..> HTTPAction : per-request usage (stack) 
HTTPServer *-- ConnectionHandler : uses
%%HTTPAction o-- SessionManager : uses
HTTPAction *-- MethodRegistry : owns
HTTPAction ..> HTTPRequest : uses
HTTPAction *.. HTTPResponse : owns
ConnectionHandler ..> HTTPResponse : uses (stack)
HTTPAction ..> ServerConfig : uses
AMethod <|-- HTTPhandlePOST : implements
AMethod <|-- HTTPhandleGET : implements
AMethod <|-- HTTPhandleDEL : implements
ConnectionHandler ..> Cgi: may get instance from HTTPAction
Cgi --o HTTPAction : may make new instance of cgi
Cgi ..> HTTPRequest : uses
Cgi ..> ServerConfig : uses
Cgi <.. HTTPServer : uses
ServerConfig --> Path : uses (stack)
```
