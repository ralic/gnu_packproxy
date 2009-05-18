#ifndef USER_CONN_H
#define USER_CONN_H

#include <sys/queue.h>
#include <sys/types.h>
#include <event.h>

#include "list.h"

LIST_CLASS_TYPE(user_conn_http_request)
LIST_CLASS_TYPE(user_conn_http_conn)

struct user_conn
{
  /* The file descriptor which is connected to the user.  */
  int fd;


  /* The output side of the processing.  */
  struct bufferevent *output;


  /* The input side of the processing.  */
  struct event input;

  /* The command buffer.  */
#define USER_CONN_BUFFER_SIZE 4096
  char buffer[USER_CONN_BUFFER_SIZE];
  /* Offset of the start of buffer.  */
  int start;
  /* Length of buffer.  */
  int len;


  /* List of http connections owned by this user connection.  */
  struct user_conn_http_conn_list http_conns;
  /* List of outstanding http requests owned by this user connection.
     This list is maintained in request order as a response must be
     delivered in the order in which it was received.  */
  struct user_conn_http_request_list requests;

  int refs;

  /* Number of requests handled by this connection.  */
  int request_count;

  /* Number of bytes received from client.  */
  int client_in_bytes;
  /* Number of bytes sent to client.  */
  int client_out_bytes;

  /* Number of bytes received from web servers.  */
  int server_in_bytes;
  /* Number of bytes sent to web servers.  */
  int server_out_bytes;

  char ip[0];
};

/* Forward.  */
struct http_request;


/* Establish a new user connection.  */
extern struct user_conn *user_conn_new (int fd, const char *from);

/* Release the resources associated with USER_CONN.  This closes any
   http connections and aborts any transfers.  */
extern void user_conn_free (struct user_conn *user_conn);

#define user_conn_ref(_uc) user_conn_ref_(_uc, __FUNCTION__)
extern void user_conn_ref_ (struct user_conn *user_conn, const char *caller);

#define user_conn_deref(_uc) user_conn_deref_(_uc, __FUNCTION__)
extern void user_conn_deref_ (struct user_conn *user_conn, const char *caller);

/* The data associated with the request ID is in buffer OUTPUT.
   OUTPUT is owned by the callee.  This is called by the downloader
   when a request has been retrieved (and processed).  */
extern void http_request_processed_cb (struct http_request *request);

#endif