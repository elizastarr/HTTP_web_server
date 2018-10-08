
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
#include <stdlib.h>
#include "server.h"
#include <sys/wait.h>

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

	//printf("\n Accepted client \n");
	int exit_status = OK;

	int client_socket_fd = -1;

	socklen_t client_length; 

	struct sockaddr_in client_address;

	char request[1024];

	client_length = sizeof( client_address );

    client_socket_fd = accept( server_socket_fd, (struct sockaddr *) &client_address, &client_length );
		
	// -------------------------------------
	// TODO:
	// -------------------------------------
	// Modify code to fork a child process
	// -------------------------------------

	//printf("\nclient_socket_fd %d\n", client_socket_fd);
	
	if ( client_socket_fd >= 0 ) {

		int pid = fork();
		if(pid == 0){
			//all stuff the child needs to handle:

			// character buffer, he has 2048. The bigger the more info the browser can send to server
			bzero( request, 1024 );
			
			read( client_socket_fd, request, 1024 );
			
			if ( DEBUG ) {
				printf("Here is the http request:\n%s\n\n", request );
			}

			// declare the entitiy_body which will be filled by GET or POST if-else branches
			char entity_body[1024];
			
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
			// parsing string
			//https://stackoverflow.com/questions/3217629/how-do-i-find-the-index-of-a-character-within-a-string-in-c
			//http://man7.org/linux/man-pages/man3/strchr.3.html
			//https://www.quora.com/How-do-I-convert-a-char-to-string-in-C
			//https://www.tutorialspoint.com/cprogramming/c_file_io.htm

			// REQUEST FORMAT	
			// |method|sp|URL|sp|version|cr|lf|
			// |header_field_name| |value|cr|lf|
			// ...
			// |cr|lf| 
			// |entity_body|
			// ------------------------------------------------------
			*/

			// GET request
			// |GET| |URL| |HTTP/1.1|\r|\n|
			// |header_field_name| |value|\r|\n|
			// ...
			// |\r|\n| 
			// ||

			if(strstr(request, "GET") != NULL ) {
				// VERSION
				char *version = strstr(request, "HTTP/1.1");
				int version_inx = version - request;

				// URL
				int begin_url_inx = 4;
				//printf("begin_url %c", request[begin_url_inx]);
				int end_url_inx = (int)(version_inx - 2);
				//printf("end_url %c\n", request[end_url_inx]);
				int url_length = end_url_inx - begin_url_inx;

				// if there is a URL
				if(url_length > 0) {

					// URL CONTENT
					char* url_content = malloc(512);
					strncpy(url_content, request + begin_url_inx + 1, url_length);
					//printf("url content %s ",  url_content);

					// KEY VALUE
					int begin_key_value = (int)(strchr(url_content, '?') - url_content);
					
					// FILE
					char* file = "SimplePost.html";

					strcpy(entity_body, "");

					// if requesting SimplePost.html
					if (strstr(url_content, file)) {
						FILE* file_obj;
						file_obj = fopen(file, "r");
						char line[50];

						if (file_obj != NULL) {
							while ( fgets(line, sizeof(line), file_obj) != NULL ) {
								strcat(entity_body, line);
							}
						}
						fclose(file_obj);
						//printf("\nFILE CLOSED: %d", status);
					}

					// else there may be key value pairs
					else {
						strcpy(entity_body, "<html><body><h1>GET Operation</h1>");

						// there are key-value pairs
						if(strchr(url_content, '?') != NULL) {
							char table [512] = "<table cellpadding=5 cellspacing=5 border=1><tr><td><b>";

							// how many key-value pairs?
							int num_pairs = 0;
							for (int i = 0; i < url_length; i++)
							{
								if (url_content[begin_key_value + i] == '&') num_pairs++;
							}

							int count = 0;
							for(int i = 1; i < url_length; i++) {
								//printf("%c", url_content[begin_key_value + i]);
								if(url_content[begin_key_value + i] == '=') {
									strcat(table, "</b></td><td>");
								}
								else if (url_content[begin_key_value + i] == '&') {
									if(count < num_pairs) {
										strcat(table, "</td></tr><tr><td><b>");
										count ++;
									}
									else {
										strcat(table, "</td></tr>");
									}	
								}
								else {
									char str_char[2];
									str_char[0] = url_content[begin_key_value + i];
									str_char[1] = '\0';
									strcat(table, str_char);
								}
							}

							strcat(table, "</td></tr><table>");
							strcat(entity_body, table);
						}

						strcat(entity_body, "</body></html>");
					}
				}

				// there is no url
				else {
					strcpy(entity_body, "<h1>400</h1><b>Bad Request: missing a URL.");
				}
			}
			
			// ------------------------------------------------------
			// POST request
			// |POST| |http://127.0.0.1/SimplePost.html| |HTTP/1.1|\r|\n|
			// |header_field_name| |value|\r|\n|
			// ...
			// |\r|\n| 
			// |entity_body|
			else if(strstr(request, "POST") != NULL) {
				strcpy(entity_body, "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Simple Post</title></head><body><h1>POST Operation</h1>");
				
				// LOCATE BODY
				char* begin_req_body = strstr(request, "\r\n\r\n");
				int begin_req_body_inx = begin_req_body - request;
				//printf("\n body content: %s", begin_req_body);

				// CREATE TABLE
				char table [512] = "<table cellpadding=5 cellspacing=5 border=1><tr><td><b>";

				// COUNT PAIRS
				int num_pairs = 0;
				for (int i = 0; i < strlen(begin_req_body); i++)
				{
					if (request[begin_req_body_inx + i] == '\0') break;
					else if (request[begin_req_body_inx + i] == '&') num_pairs++;
				}
				// FILL TABLE
				int pair_count = 0;
				for(int i = 1; i < strlen(request); i++) {
					//printf(" %c", request[begin_req_body_inx + i]);

					if(request[begin_req_body_inx + i] == '\0') {
						break;
					}
					else if(request[begin_req_body_inx + i] == '=') {
						strcat(table, "</b></td><td>");
					}
					else if (request[begin_req_body_inx + i] == '&') {
						if(pair_count < num_pairs) {
							strcat(table, "</td></tr><tr><td><b>");
							pair_count ++;
						}
						else {
							strcat(table, "</td></tr>");
						}	
					}
					else if (request[begin_req_body_inx + i] == '?'){
						continue;
					}
					else {
						char str_char[2];
						str_char[0] = request[begin_req_body_inx + i];
						str_char[1] = '\0';
						strcat(table, str_char);
					}
				}

				strcat(table, "</td></tr><table>");
				strcat(entity_body, table);
				//printf("\nEND OF POST PARSING\n");

				strcat(entity_body, "</body></html>");
			}

			// ------------------------------------------------------
			// UNKNOWN REQUEST
			else {
				// need to change entity body, create response, and return from method
				strcpy(entity_body, "<h1>400</h1><b>Unknown HTTP request.");

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
			
			if ( DEBUG ) printf( "\nResponse:\n%s\n", response );
			
			write( client_socket_fd, response, strlen( response ) );
			
			close( client_socket_fd );
			//printf("Close socket %d \n", close_status);
			//------------------------------------------------------
			
			//Must exit the child process
			exit(0);
		}
		else if (pid > 0){
			// parent
			close( client_socket_fd );
			//printf("Close socket %d \n", close_status);
		}
		else {
			// forking error
		}

	} else {
		//socket error
		exit_status = FAIL;
	}
	
	if ( DEBUG ) printf("Exit status = %d\n", exit_status );
	
	return exit_status;
	
} // end accept_client function