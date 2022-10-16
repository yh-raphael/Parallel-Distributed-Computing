/* micro_httpd - really small HTTP server
**
** Copyright : 1999,2005 by Jef Poskanzer <jef@mail.acme.com>.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
*/


#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PROTOCOL        "HTTP/1.0"
#define MAX_MSG_LEN     1024
//#define __DEBUG__

/* Forwards. */
static int send_response(int fd, int status, char* title, char* body);
static void strdecode( char* to, char* from );
static int hexit( char c );
static void strencode( char* to, size_t tosize, const char* from );

/* Worker Thread Routine - Handle HTTP protocol */
int do_worker_routine(const char* request, int fd) 
{

#ifdef __DEBUG__
    printf("Worker is watching this request: %s \n", request);
#endif

	// char sbuff[10000];
	// char filecontent[512][128];
    // int cnt = 0;

    // memset(sbuff, 0, 10000);

    // char line[10000], method[10000], path[10000], protocol[10000], idx[20000], location[20000];
    char method[1000], path[1000], protocol[1000];
    char buf[1024];

    size_t len;
    int cn;
    int ich;
    struct stat sb;
    FILE* fp;
    struct dirent **dl;


    if ( sscanf( request, "%[^ ] %[^ ] %[^ ]\r\n", method, path, protocol ) != 3 ) {
        send_response(fd, 400, "Bad Request", "This request is not the correctly formatted one.");
        return -1;
    }

    if ( strncasecmp( method, "get" , 3) != 0 ) {
        send_response(fd, 501, "Not Implemented", "This method is unsupported. \nP.S. (Only Get Method Supported)");
        return -1;
    }

    if ( path[0] != '/' ) {
        send_response(fd, 400, "Bad Request", "Cannot find the file.");
        return -1;
    }

    if ( stat(path, &sb) < 0 ) {
        send_response(fd, 404, "Not Found", "File not found.");
        return -1;
    }
    if ( S_ISDIR( sb.st_mode ) ) {
        send_response(fd, 404, "Not Found", "File not found." );
        return -1;
    }
    else
    {
        fp = fopen( path, "rt" );
        if ( fp == (FILE*) 0 ) {
            send_response(fd, 403, "Forbidden", "File is protected.");
            return -1;
        }
        send_response(fd, 200, "Ok", NULL);

        while ( (cn = fread(buf, 1, sizeof(buf), fp)) > 0 ) {
            write(fd, buf, cn);
        }
        // while (fgets(filecontent[cnt], 128, fp) != NULL)
        // {
        //     if(cnt == 0)
        //         strcpy(sbuff, filecontent[cnt]);
        //     else
        //         strcat(sbuff, filecontent[cnt]);
        //     cnt++;
        // }
        // send(fd, sbuff, strlen(sbuff), 0);

        fclose(fp);
	}

    return 0;
}

static int send_response(int fd, int status, char* title, char* body)
{
    char buf[1024];
    int cn;
    if (body)
        cn = sprintf(buf, "%s %d %s\015\012\015\012%s", PROTOCOL, status, title, body);
    else
        cn = sprintf(buf, "%s %d %s\015\012\015\012", PROTOCOL, status, title);

    write(fd, buf, cn);
    return 0;
}

static void strdecode( char* to, char* from )
{
    for ( ; *from != '\0'; ++to, ++from )
	{
        if ( from[0] == '%' && isxdigit( from[1] ) && isxdigit( from[2] ) )
        {
            *to = hexit( from[1] ) * 16 + hexit( from[2] );
            from += 2;
        }
        else
            *to = *from;
    }
    
    *to = '\0';
}

static int hexit( char c )
{
    if ( c >= '0' && c <= '9' )
        return c - '0';
    if ( c >= 'a' && c <= 'f' )
        return c - 'a' + 10;
    if ( c >= 'A' && c <= 'F' )
        return c - 'A' + 10;

    return 0;		/* shouldn't happen, we're guarded by isxdigit() */
}

static void strencode( char* to, size_t tosize, const char* from )
{
    int tolen;

    for ( tolen = 0; *from != '\0' && tolen + 4 < tosize; ++from )
	{
        if ( isalnum(*from) || strchr( "/_.-~", *from ) != (char*) 0 )
        {
            *to = *from;
            ++to;
            ++tolen;
        }
        else
        {
            sprintf( to, "%%%02x", (int) *from & 0xff );
            to += 3;
            tolen += 3;
        }
	}

    *to = '\0';
}