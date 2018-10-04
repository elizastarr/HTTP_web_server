
// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2018
// server.h header file
// Homework 2
//
// -----------------------------------

#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.h"

// ------------------------------------
// Function prototype that creates a socket and 
// then binds it to the specified port_number 
// for incoming client connections
// 
//
// Arguments:	port_number = port number the server 
//				socket will be bound to.
//
// Return:      Socket file descriptor (or -1 on failure)
//

int bind_port( unsigned int port_number ) {

	// -------------------------------------
	// NOTHING TODO HERE :)
	// -------------------------------------
	// Please do not modify

	int socket_fd;
	int set_option = 1;

    struct sockaddr_in server_address;
     
    socket_fd = socket( AF_INET, SOCK_STREAM, 0 );

    setsockopt( socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&set_option, sizeof( set_option ) );

    if (socket_fd < 0) return FAIL;

    bzero( (char *) &server_address, sizeof(server_address) );

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons( port_number );

    if ( bind( socket_fd, (struct sockaddr *) &server_address, sizeof(server_address) ) == 0 ) {

    	return socket_fd;

    } else {

    	return FAIL;

    }

} // end bind_port function

// ------------------------------------
// Function prototype that accepts a client
// socket connection
// 
//
// Arguments:	server file descriptor
//
// Return:      Termination status of client
//				( 0 = No Errors, -1 = Error )
// 	browser connects to server, send over 
int accept_client( int server_socket_fd ) {

	int exit_status = OK;

	int client_socket_fd = -1;

	socklen_t client_length; 

	struct sockaddr_in client_address;


	client_length = sizeof( client_address );

    client_socket_fd = accept( server_socket_fd, (struct sockaddr *) &client_address, &client_length );
		
	// -------------------------------------
	// TODO:
	// -------------------------------------
	// Modify code to fork a child process
	// -------------------------------------
	
	if ( client_socket_fd >= 0 ) {

		if(fork()==0){
			//all stuff the child needs to handle:

			// character buffer, he has 2048. The bigger the more info the browser can send to server
			bzero( request, 512 );
			
			read( client_socket_fd, request, 511 );
			
			if ( DEBUG ) {
				printf("Here is the http message:\n%s\n\n", request );
			}

			// declare the entitiy_body which will be filled by GET or POST if-else branches
			char entity_body[2000];
			
			// -------------------------------------
			// TODO:
			// -------------------------------------
			// Generate the correct http response when a GET or POST method is sent
			// from the client to the server.
			// 
			// In general, you will parse the request character array to:
			// 1) Determine if a GET or POST method was used
			// 2) Then retrieve the key/value pairs (see below)
			// -------------------------------------
			
			/*
			------------------------------------------------------
			GET method key/values are located in the URL of the request message
			? - indicates the beginning of the key/value pairs in the URL
			& - is used to separate multiple key/value pairs 
			= - is used to separate the key and value
			
			Example:
			
			http://localhost/?first=brent&last=munsell
			
			two &'s indicated two key/value pairs (first=brent and last=munsell)
			key = first, value = brent
			key = last, value = munsell
			------------------------------------------------------
			*/
			
			/*
			------------------------------------------------------
			POST method key/value pairs are located in the entity body of the request message
			? - indicates the beginning of the key/value pairs
			& - is used to delimit multiple key/value pairs 
			= - is used to delimit key and value
			
			// THIS IS AN EXAMPLE ENTITY BODY. looking into entity body to see what it contains. POST
			//char* entity_body = "<html><body><h2>CSCI 340 (Operating Systems) Homework 2</h2><table border=1 width=\"50%\"><tr><th>Key</th><th>Value</th></tr></table></body></html>";
			
			Example:
			
			first=brent&last=munsell
			
			two &'s indicated two key/value pairs (first=brent and last=munsell)
			key = first, value = brent
			key = last, value = munsell
			
			// checking if a string contains a string. Returns NULL if does not contain.
			//http://man7.org/linux/man-pages/man3/strstr.3.html

			// REQUEST FORMAT	
			// |method|sp|URL|sp|version|cr|lf|
			// |header_field_name| |value|cr|lf|
			// ...
			// |cr|lf| 
			// |entity_body|
			------------------------------------------------------
			*/

			// GET request
			// |GET| |URL| |HTTP/1.1|\r|\n|
				// |header_field_name| |value|\r|\n|
				// ...
				// |\r|\n| 
				// ||
			if(strstr(request, "GET") != NULL) {
				printf("GET");
				

			}

			// POST request
			// |POST| |http://127.0.0.1/SimplePost.html| |HTTP/1.1|\r|\n|
				// |header_field_name| |value|\r|\n|
				// ...
				// |\r|\n| 
				// |entity_body|
			else if(strstr(request, "POST") != NULL) {
				printf("POST");
				

			}
			// ERROR
			else {
				// need to change entity body, create response, and return from method
				printf("Unknown HTTP request.");

				//change entitiy body?

				//------------------------------------------------------
				// Package up and send off response
				char response[512];
				// code 200 means everything is ok
				sprintf( response, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s", (int)strlen( entity_body ), entity_body );
				
				if ( DEBUG ) printf( "%s\n", response );
				
				write( client_socket_fd, response, strlen( response ) );
				
				close( client_socket_fd );
				//------------------------------------------------------

				return exit_status;
			}
			
			// Package up and send off response for GET and POST
			//------------------------------------------------------
			char response[512];

			// RESPONSE FORMAT	
			// |version|sp|status_code|sp|phrase|cr|lf|
			// |header_field_name| |value|cr|lf|
			// ...
			// |cr|lf| 
			// |entity_body|

			// |HTTP/1.1| |200| |OK|\r|\n|
			// |Content-Length:| |(int)strlen( entity_body )|\r|\n|
			// ...
			// |\r|\n| 
			// |entity_body|

			// NOTE: entity_body is a String, which in c is an array of char 
			sprintf( response, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s", (int)strlen( entity_body ), entity_body );
			
			if ( DEBUG ) printf( "%s\n", response );
			
			write( client_socket_fd, response, strlen( response ) );
			
			close( client_socket_fd );
			//------------------------------------------------------
			
			//Must exit the child process
			exit(0);
		}
		
	} else {
		
		exit_status = FAIL;
		
	}
	
	if ( DEBUG ) printf("Exit status = %d\n", exit_status );
	
	return exit_status;
	
} // end accept_client function
