#include    <limits.h>
#include    <sys/errno.h>
#include    <sys/select.h>

#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */

#include "HTTP.h"
#include "StringFunctions.h"

#define	SA	struct sockaddr
#define	LISTENQ		1024	/* 2nd argument to listen() */
// Another source
#define BUFFER_SIZE 2048

#define MAX_CLIENTS 128
#define MAX_THREADS 128

#define START_INDEX_FILTERS 2 

//=============================VARIABLE DECLARE====================
typedef	void	Sigfunc(int);
/// A structure to hold a file descriptor and a message.
typedef struct {
	/// Socket/File Descriptor.
	int sock;
	
	/// The address in the communications space of the socket.
	struct sockaddr_in address;
	
	/// Message sent by fd.
	char *msg;

	/// Number of argument
	int argc;
} sock_msg;

int errno;
unsigned int *filtersCount;
const char **filters;

/// The total number of successful requests.
static unsigned int numberOfSuccessfulRequests = 0;
/// The total number of filtered requests.
static unsigned int numberOfFilteredRequests = 0;
/// The total number of errored requests.
static unsigned int numberOfErroredRequests = 0;

//===============================================
void incrementNumberOfSuccessfulRequests();
void incrementNumberOfFilteredRequests();
void incrementNumberOfErroredRequests();

//===========================UTILS FUNCTION===========================
void error(char* msg)
{
    perror(msg);
    exit(0);
}

//==========================UNPV13E LIB FUNCTION======================
/* include Socket */
int
Socket(int family, int type, int protocol)
{
	int		n;

	if ( (n = socket(family, type, protocol)) < 0)
		error("socket error");
	return(n);
}
/* end Socket */

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		error("bind error");
}

/* include Listen */
void
Listen(int fd, int backlog)
{
	char	*ptr;

		/*4can override 2nd argument with environment variable */
	if ( (ptr = getenv("LISTENQ")) != NULL)
		backlog = atoi(ptr);

	if (listen(fd, backlog) < 0)
		error("listen error");
}
/* end Listen */

int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int		n;

again:
	if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef	EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
			goto again;
		else
			error("accept error");
	}
	return(n);
}

void
Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (connect(fd, sa, salen) < 0)
		error("connect error");
}

ssize_t
Recv(int fd, void *ptr, size_t nbytes, int flags)
{
	ssize_t		n;

	if ( (n = recv(fd, ptr, nbytes, flags)) < 0)
		error("recv error");
	return(n);
}

void
Send(int fd, const void *ptr, size_t nbytes, int flags)
{
	if (send(fd, ptr, nbytes, flags) != (ssize_t)nbytes)
		error("send error");
}

void
Close(int fd)
{
	if (close(fd) == -1)
		error("close error");
}

pid_t
Fork(void)
{
	pid_t	pid;

	if ( (pid = fork()) == -1)
		error("fork error");
	return(pid);
}

Sigfunc *
Signal(int signo, Sigfunc *func)	/* for our signal() function */
{
	Sigfunc	*sigfunc;

	if ( (sigfunc = signal(signo, func)) == SIG_ERR)
		error("signal error");
	return(sigfunc);
}

void 
//======================================SIGNAL FUNCTION=====================
sigintHandler(int sig_num)
{
	/* Reset handler to catch SIGINT next time. 
       Refer http://en.cppreference.com/w/c/program/signal */
	Signal(SIGINT, sigintHandler);
	fflush(stdout);
}

void
sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		printf("child %d terminated\n", pid);
	}
	return;
}

void
handleSIGUSR1()
{
	// Received signal.
	printf("Received SIGUSR1...reporting status:\n");
	
	// Report the number of requests.
	if ( numberOfSuccessfulRequests == 1 ) {
		printf("-- Processed %u request successfully.\n", numberOfSuccessfulRequests);
	} else {
		printf("-- Processed %u requests successfully.\n", numberOfSuccessfulRequests);
	}
	
	// Report the filters being used.
	printf("-- Filtering:");
	for ( unsigned int i=0; i<*filtersCount; ++i ) {
		printf(" %s;", filters[i]);
	}
	printf("\n");
	
	// Report the number of filtered requests.
	if ( numberOfFilteredRequests == 1 ) {
		printf("-- Filtered %u request.\n", numberOfFilteredRequests);
	} else {
		printf("-- Filtered %u requests.\n", numberOfFilteredRequests);
	}
	
	// Report the number of requests that resulted in errors.
	if ( numberOfErroredRequests == 1 ) {
		printf("-- Encountered %u request in error\n", numberOfErroredRequests);
	} else {
		printf("-- Encountered %u requests in error\n", numberOfErroredRequests);
	}
}

void handleSIGUSR2()
{
	printf("Received SIGUSR2...");
	exit(1);
}

//=====================================================================
/// Parse an HTTP request.
/// @param request The HTTP request.
/// @param error The possible error in processing.
/// @return The parsed HTTP request.
HTTPRequest processRequest(char *requestString, int *error);

/// Determine whether a request should be filtered out or not.
/// @param request The HTTP request.
/// @return Whether or not the request should be allowed.
bool shouldAllowRequest(HTTPRequest request);

/// Determine whether a server should be filtered out or not.
/// @param server The server in the HTTP request.
/// @return Whether or not the server should be allowed.
bool shouldAllowServer(const char *server);

/// Send an HTTP Status Line to a socket.
/// @param errorNo The HTTP Response code.
/// @param client The socket to send the status to.
/// @return Whether or not the transmission was successful.
bool sendHTTPStatusToSocket(int status, int client);

/// Handle an HTTP request.
/// @param arg The request string.
/// @return @c NULL
/// @discussion Designed for use in new thread.
void *handleRequest(void *arg);


int main(int argc, char* argv[])
{
    pid_t pid;
    struct sockaddr_in addr_in,cli_addr,serv_addr;
    struct hostent* host;

    int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	void				sig_chld(int);
    
    if(argc<2)
        error("./1712328 <port>");

    // The port number to use for the socket.
	unsigned short port = 8888; // The default port is 8127 (if no arguments are given).
	if ( argc > 1 ) {
		int conversion = atoi(argv[1]);
		// Cannot convert int to unsigned short.
		if ( conversion < 0 || conversion > USHRT_MAX ) {
			error("Invalid port");
		}
		// Assign the port value.
		port = (unsigned short) conversion;
	}

    // /*
	//  Your proxy server must filter based on domain name (or IP address) prefixes/suffixes as described above. All command-line arguments following the port number are domain or IP address prefixes/suffixes that must be filtered. There can be zero or more of these; and there is no limit to the number of arguments.
	//  */
	// memory leak
	filtersCount = malloc(sizeof(unsigned int));
	*filtersCount = 0;
	
	// Contains at least one filter.
	if ( argc > START_INDEX_FILTERS ) {
		// Filters are START_INDEX_FILTERS less than the number of arguments to the program.
		*filtersCount = argc - START_INDEX_FILTERS;
		// Create a pointer to the start of the filters in argv.
		filters = &argv[START_INDEX_FILTERS];
	}
    
    // set all 0 for memory buffer
    bzero((char*)&servaddr,sizeof(servaddr));
    bzero((char*)&cliaddr, sizeof(cliaddr));
    
    // Create the server
    servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));
    
    // Create the listener 
    listenfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    // Bind
    Bind(listenfd, (SA *) &servaddr,sizeof(servaddr));
 
    // Number of maximum accept client
    Listen(listenfd, LISTENQ);
    printf("Listener socket created and bound to port %d\n", port);
    
    Signal(SIGCHLD, sig_chld);
    Signal(SIGINT, sigintHandler);
    Signal(SIGUSR1, &handleSIGUSR1);
	Signal(SIGUSR2, &handleSIGUSR2);

    for ( ; ; ) {
        clilen = sizeof(cliaddr);
		if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;		/* back to for() */
			else
				error("accept error");
		}
        if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* close listening socket */
			char buffer[BUFFER_SIZE]; // Packet size

            // Receive the message.
            bzero((char*)buffer,BUFFER_SIZE);
			ssize_t n = Recv(connfd, buffer, BUFFER_SIZE, 0);
			// Stream received message.
			buffer[n] = '\0';
		
			//printf("Received message from fd %d\n: %s\n", listenfd, buffer);	
        /* 
		 6. Your server does must be a concurrent server (i.e. do not use an iterative server).
		 */

			// Create a thread to handle message.
			sock_msg *arg = malloc(sizeof(sock_msg));
			arg->sock = connfd;
			arg->address = servaddr;
			arg->msg = stringDuplicate(buffer);
			arg->argc = argc;
			handleRequest(arg);
            exit(0);
		}
		Close(connfd);
    }
    
    return EXIT_SUCCESS;
}

void *handleRequest(void *argument)
{
	// Unpack argument into variables.
	sock_msg *arg = (sock_msg *)argument;
	int fd = arg->sock;
	struct sockaddr_in client = arg->address;
	char *requestString = arg->msg;
	int numberOfArgument = arg->argc;
	// Process the request string into a HTTPRequest.
	int error = 0;
	HTTPRequest request = processRequest(requestString, &error);
	if ( !request ) {
		// Send back HTTP Error.
		sendHTTPStatusToSocket(error, fd);
		
		// Increment number of (purposefully) errored requests.
		//incrementNumberOfErroredRequests();
		numberOfErroredRequests++;
		goto end;
	}
	
	/*
	 When your server detects a request that should be filtered, your server should return an HTTP error 403 (forbidden), which means you need to send back an HTTP status line that indicates an error.
	 */
	
	// Read the IP Address into a string.
	char *ip_addr = inet_ntoa((struct in_addr)client.sin_addr);
	
	// Figure out if the request should be filtered out.
	if ( !shouldAllowRequest(request) ) {
		// Print Request Line [FILTERED].
		printf("%s: %s [FILTERED]\n", ip_addr, request[HTTPRequestHeaderField_Request_Line]);
		
		// Send back HTTP Error 403 Forbidden.
		sendHTTPStatusToSocket(403, fd);
		
		// Increment filtered requests counter.
		//incrementNumberOfFilteredRequests();
		numberOfFilteredRequests++;
		goto end;
	}
	else{
		printf("%s: %s\n", ip_addr, request[HTTPRequestHeaderField_Request_Line]);
		if (numberOfArgument > START_INDEX_FILTERS)
			sendHTTPStatusToSocket(301, fd);
	}
	
	/*
	 Your server must forward the appropriate HTTP request headers to the requested server, then send the responses back to the client.
	 */
	
	int serverSocket = Socket(PF_INET, SOCK_STREAM, 0);
	
	// Server
	struct sockaddr_in server;
	server.sin_family = PF_INET;
	
	struct hostent *hp = gethostbyname(request[HTTPRequestHeaderField_Host]);
	if ( hp == NULL ) {
		perror("Unknown host");
		goto end;
	}
	
	// Could also use memcpy
	bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	unsigned short port = 80;
	server.sin_port = htons(port);
	
	// Connect.
	Connect(serverSocket, (struct sockaddr *)&server, sizeof(server));
	
	// Strip out Accept-Encoding to prevent chunking (not yet supported).
	char *accept_encoding = request[HTTPRequestHeaderField_Accept_Encoding];
	if ( accept_encoding != NULL ) {
		free(accept_encoding);
		request[HTTPRequestHeaderField_Accept_Encoding] = NULL;
	}
	
	// Get the request string.
	char *serverRequestString = requestStringFromRequest(request);
	if ( !serverRequestString ) {
		goto end;
	}
	
	// Send.
	Send(serverSocket, serverRequestString, strlen(serverRequestString), 0);
	
	// Buffer to load received messages into.
	char buffer[BUFFER_SIZE];
	
	// Receive.
	while (1) {
		// BLOCK
		ssize_t received_n = Recv(serverSocket, buffer, BUFFER_SIZE - 1, 0);
		if ( received_n == 0 ) {
			// Peer has closed its half side of the (TCP) connection.
			break;
		} 
		// End the buffer with a null-terminator.
		buffer[received_n] = '\0';
			
#ifdef DEBUG
			// Print out the received message.
			//printf("\n\nReceived message from %s:\n%s\n\n", inet_ntoa((struct in_addr)server.sin_addr), buffer);
#endif
			
		Send(fd, buffer, strlen(buffer), 0);
	}
	
	// Increment number of successful requests.
	//incrementNumberOfSuccessfulRequests();
	numberOfSuccessfulRequests++;
		
end:
	
	if ( requestString ) {
		free(requestString);
	}
	
	if ( serverRequestString ) {
		free(serverRequestString);
	}
	
	Close(serverSocket);
	
	if ( request != NULL ) {
		// Request is no longer needed.
		HTTPRequestFree(request);
	}

	return NULL;
}


#pragma mark - Private API (Implementation)

HTTPRequest processRequest(char *requestString, int *error)
{
	HTTPRequest request = HTTPRequestCreate();
	
	// Start the parsing at the beginning of the request.
	char *parse = requestString;
	static const char * const delimiter = "\r\n";
	
	// Parse out the rest of the headers.
	for ( int i = 0; i < HTTPRequestHeaderFieldsCount; ++i ) {
		
		// Find the end of the line.
		char *next = strstr(parse, delimiter);

		// Get just the line.
		char *line = substring(parse, next);

		// Advance the parse pointer to the end of the line, and after the delimiter.
		parse = next + strlen(delimiter);
		
		// Stop when at the end of the header.
		if ( line == NULL ) {
			break;
		}
		
		// The first line is different than the rest.
		if ( i == 0 ) {
			
			/*
			 2. Your server must handle GET, HEAD, and POST request methods.
			 */
			
			char *Method = NULL;
			char *MethodArgs = NULL;
			if ( !splitStringAtString(line, " ", &Method, &MethodArgs) ) {
				// Invalid Request-Line.
				*error = 400;
				return false;
			}
			
			char *Request_URI = NULL;
			char *HTTP_Version = NULL;
			if ( !splitStringAtString(MethodArgs, " ", &Request_URI, &HTTP_Version) ) {
				// Invalid Request-Line.
				*error = 400;
				return false;
			}
			
			unsigned long valueLength = strlen(Method) + 1 + strlen(Request_URI) + 1 + strlen(HTTP_Version) + 1;
			char *value = malloc(sizeof(char) * valueLength);
			strcpy(value, Method);
			strcat(value, " ");
			strcat(value, Request_URI);
			strcat(value, " ");
			strcat(value, HTTP_Version);
			value[valueLength-1] = '\0';
			
			if ( stringEquality(Method, "GET") ) {
				request[HTTPRequestHeaderField_Request_Line] = value;
			} else if ( stringEquality(Method, "HEAD")) {
				request[HTTPRequestHeaderField_Request_Line] = value;
			} else if ( stringEquality(Method, "POST") ) {
				request[HTTPRequestHeaderField_Request_Line] = value;
			} else {
				/*
				 3. Your server must refuse to process any HTTP request method other than GET, HEAD, and POST. In such cases, you should send back an HTTP status code of 405 (Method not allowed) or 501 (Not Implemented) if you receive any other request method.
				 */
				HTTPRequestFree(request);
				*error = 405;
				return NULL;
			}
			
		} else { // Handle another request
			// In the format of: "Field Name: Value"
			
			// Split the line into field name and value.
			char *fieldName = NULL;
			char *fieldValue = NULL;
			if ( !splitStringAtString(line, ": ", &fieldName, &fieldValue) ) {
				// Line isn't in the correct format.
				continue;
			}
			
			HTTPRequestHeaderField field = HTTPRequestHeaderFieldForFieldNamed(fieldName);
			if ( (int) field == -1 ) {
				// Conversion failed.
				continue;
			}
			request[field] = fieldValue;
		}
		
#ifdef DEBUG
		// Print the line.
		//printf("%s\n", line);
#endif
	}
	
	/*
	 Your server must send an error to the client whenever appropriate, including such cases as the request line being invalid, a Host: header is not found, or a POST request does not include a Content-Length: header. In these cases, your server must send a 400 (Bad Request) as a result.
	 */
	
	// Check request to see if the request's valid.
	if ( !validateRequest(request) ) {
		// Free unused request.
		HTTPRequestFree(request);
		*error = 400;
		return NULL;
	}
	
	return request;
}

bool shouldAllowRequest(HTTPRequest request)
{
	// Request must be non-NULL.
	if ( request == NULL ) {
		return false;
	}
	
	return shouldAllowServer(request[HTTPRequestHeaderField_Host]);
}

bool shouldAllowServer(const char *server)
{
	// The end of the server string (used for finding suffix matches).
	const char *serverEnd = server + strlen(server);
	
	// Check against all of the filters.
	for ( int i=0; i<*filtersCount; ++i ) {
		const char *filter = filters[i];
		
		// Length of filter string.
		size_t filterLen = strlen(filter);
		
		// Check prefix.
		if ( caseInsensitiveStringComparisonLimited(server, filter, filterLen) == 0 ) {
			return false;
		}
		
		// Check suffix.
		if ( caseInsensitiveStringComparisonLimited(serverEnd - filterLen, filter, filterLen) == 0 ) {
			return false;
		}
		
	}
	
	return true;
}

bool sendHTTPStatusToSocket(int status, int client)
{
	// Find the maximum length of the status string. (To create buffer of correct size).
	unsigned long maxLength =  8  // "HTTP/1.1"
						    +  1
							+  3  // 3-digit status code.
						    +  1
							+ 36  // Max length of status string
	                        +  5; // Double carriage return/newline + null-terminator
	
	// Create buffer for status string.
	char *status_str = malloc(sizeof(char) * maxLength);
	
	char *HTTP_Version = "HTTP/1.1";
	
	// Populate the string.
	int sprinted = snprintf(status_str, maxLength, "%s %d %s\r\n\r\n", HTTP_Version, status, statusStringForStatusCode(status));
	
	// Check for errors in sprintf.
	if ( sprinted < 0 ) {
		// Error in snprintf.
	} else if ( sprinted < maxLength ) {
		// Did not get to sprint the whole string.
	}
	
	// Send the status.
	Send(client, status_str, strlen(status_str), 0);
	
	// The string is no longer needed.
	free(status_str);

	// Everything went OK!
	return true;
}

void incrementNumberOfSuccessfulRequests()
{
	numberOfSuccessfulRequests++;
}

void incrementNumberOfFilteredRequests()
{
	numberOfFilteredRequests++;
}

void incrementNumberOfErroredRequests()
{
	numberOfErroredRequests++;
}	