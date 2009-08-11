// ----------------------------------------------------------------------------
// Project           : K7 - Standard Library for V8
// -----------------------------------------------------------------------------
// Author            : Sebastien Pierre                   <sebastien@type-z.org>
//                     Victor Grishchenko
// ----------------------------------------------------------------------------
// Creation date     : 27-Sep-2008
// Last modification : 05-Jun-2009
// ----------------------------------------------------------------------------
/*
  File: system.posix
    posix functions

    Author: Sebastien Pierre                   <sebastien@type-z.org>
    Author: Victor Grishchenko

    Creation date:      27-Sep-2008

    Last modification:  10-Aug-2009
*/



#include "k7.h"

// FIXME: I dont' know if this is Linux only... probably !
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define MODULE_NAME   "system.posix"
#define MODULE_STATIC  system_posix

int obj2addr (Handle<Object> obj, struct sockaddr_in* addr) {
	HandleScope scope;
	String::AsciiValue a (obj->Get(JS_str("addr")));
	Handle<Value> p = obj->Get(JS_str("port"));
	memset(addr,0,sizeof(struct sockaddr_in));
	if (1!=inet_pton(AF_INET,*a,&(addr->sin_addr.s_addr)))
		return -1;
	if (!p->IsInt32())
		return -2;
	addr->sin_port = htons((uint16_t)p->Uint32Value());
	addr->sin_family = AF_INET;
	return 0;
}

Handle<Object> addr2obj (struct sockaddr_in* addr) {
	HandleScope scope;
	Handle<Object> obj(Object::New());
	obj->Set(JS_str("port"),JS_int(ntohs(addr->sin_port)));
	char str[32];
	inet_ntop(AF_INET,&(addr->sin_addr),str,sizeof(sockaddr_in));
	obj->Set(JS_str("addr"),String::New(str));
	return scope.Close(obj);
}

OBJECT(posix_FILE,1,FILE* file) {
	INTERNAL(0,file);
	return self;
} END

/*
  Function: posix.time()
  Returns the local UNIX time as an integer, in milliseconds
*/
FUNCTION(posix_time) {
	ARG_COUNT(0)
	return JS_int(time(NULL));
} END
/*
  Function: posix.fopen(path, mode)
  Opens the file at the given 'path' using the given 'mode'. Returns a file object, that you should not forget to close.
  
  Parameters:
    path - A string, ie '/tmp/myfile.txt'
    mode - A String, ie 'w'

  Example:
  (start code)
    var fd = fopen("/tmp/myfile.txt", "w")
  (end)
*/
FUNCTION(posix_fopen) {
	ARG_COUNT(2)
	ARG_utf8(path,0);
	ARG_utf8(mode,1);
	FILE* fd = fopen(*path,*mode);
	if (fd == NULL)
		return JS_null;
	return posix_FILE(fd);
} END

/*
  Function posix.fclose(fileObj)
  Closes the given file object.
*/
FUNCTION(posix_fclose) {
	ARG_COUNT(1);
	ARG_obj(fileObj,0);
	EXTERNAL(FILE*,file,fileObj,0);
	return JS_int(fclose(file));
} END

/*
  Function posix.popen(command, mode)
  Opens a pipe
*/
FUNCTION(posix_popen) {
	ARG_COUNT(2)
	ARG_utf8(path,0);
	ARG_utf8(type,1);
	FILE* fd = popen(*path,*type);
	if (fd == NULL)
		return JS_null;
	return posix_FILE(fd);
} END

/*
  Function posix.pclose(fileObj)
  Closes a pipe
*/
FUNCTION(posix_pclose) {
	ARG_COUNT(1);
	ARG_obj(fileObj,0);
	EXTERNAL(FILE*,file,fileObj,0);
	return JS_int(pclose(file));
} END

/*
  Function posix.system(command)
  Executes a system command.
  Returns:
  Integer 
*/
FUNCTION(posix_system) {
	ARG_COUNT(1);
	ARG_utf8(command,0);
	return JS_int(system(*command));
} END

/*
  Function posix.fwrite(data, size, nmemb, fileObj)
  Writes to a file
*/
FUNCTION(posix_fwrite) {
	ARG_utf8(data,0);
	ARG_int(size,1);
	ARG_int(nmemb,2);
	ARG_obj(fileObj,3);
	EXTERNAL(FILE*,file,fileObj,0);
	return JS_int(fwrite(*data,size,nmemb,file));
} END

/*
  Function: posix.fread(size, nmemb, fileObj)
  Reads 'quantity' elements of the given 'size' (in bytes) from the given 'file'
  object, returning a string which may be emty if there is nothing to be read.
 */
FUNCTION(posix_fread) {
	ARG_COUNT(3);
	ARG_int(size,0);
	ARG_int(nmemb,1);
	ARG_obj(fileObj,2);
	EXTERNAL(FILE*,file,fileObj,0);
	if (size < 0) {
		return v8::ThrowException(JS_str("Exception: invalid bufsize"));
	}
	char *buf   = new char[size*nmemb + 1024];
	size_t read = fread(buf, size, nmemb, file);
	v8::Handle<v8::String> strbuf = JS_str2(buf, read);
	delete [] buf;
	return strbuf;
} END

/*
  Function posix.feof(fileObj)
  Returns true if end of file
  Returns:
    Boolean
*/
FUNCTION(posix_feof) {
	ARG_obj(fileObj, 1);
	EXTERNAL(FILE*,file,fileObj,0);
	return Boolean::New(feof(file));
} END

/*
  Function posix.readfile()
  STUB
*/
FUNCTION(posix_readfile) {
	STUB
} END

/*
  Function posix.writefile(name, data)
  Writes data to a file.
*/
FUNCTION(posix_writefile) {
	ARG_COUNT(2);
	ARG_utf8(name, 0);
	ARG_utf8(data, 1);
	
	if (*name == NULL) {
		return v8::ThrowException(v8::String::New("Invalid filename"));
	}

	FILE* file = fopen(*name, "wb");
	if (file == NULL) {
		return v8::ThrowException(v8::String::New("Could not open file for writing."));
	}
	int result = fwrite(*data,strlen(*data),1,file);
	fclose(file);
	
	return JS_int(result);
	
} END

// See this good tutorial for the pthreads API 
//  https://computing.llnl.gov/tutorials/pthreads/

OBJECT(posix_PTHREAD,1,pthread_t* thread) {
	INTERNAL(0,thread)
	return self;
} END

// NOTE: This is WIP code that will be moved to a "task" module using libtask API
void* posix_pthread_create_callback(void* context) {
	//ARG_COUNT(1);
	//ARG_obj(callback_context,0);
	printf("NEW THREAD STARTED...\n");

	Locker locker;
	HandleScope HandleScope;

	Object* context_obj = reinterpret_cast<Object*>(context);

	Handle<Function> callback_v = Handle<Function>::Cast(context_obj->Get(JS_str("callback")));
	callback_v->Call(v8::Context::GetCurrent()->Global(), 0, NULL);

	// TODO: We should release the callback context
}


FUNCTION(posix_pthread_create) {
	ARG_COUNT(2);
	//Handle<Function> callback = Handle<Function>::Cast(args[(0)]);
	ARG_fn(callback,0);
	ARG_obj(arguments, 1);
	//Persistent<Arguments> thread_arguments = args;
	// FIXME: This won't work, of course

	pthread_t* thread = (pthread_t*) malloc(sizeof(pthread_t));
	Persistent<Object> result = Persistent<Object>::New(posix_PTHREAD(thread));
	result->Set(JS_str("callback"),callback);
	/* SEGFAULT HERE
	result->Set(JS_str("arguments"),arguments);
	*/
	//pthread_create(thread, NULL, posix_pthread_create_callback, (void*)*result);
	posix_pthread_create_callback((void*)*result);
	return result;
} END

/*
  Function: posix.open(name, mode)
  
  Parameters:
  filename - String containing the name of the file to be opened.
  mode - Flags describing the requested i/o mode for the file.
  
  which consists on a combination of one or more of the following flags defined as member constants:
      flag value	opening mode
      app	(append) Set the stream's position indicator to the end of the stream before each output operation.
      ate	(at end) Set the stream's position indicator to the end of the stream on opening.
      binary	(binary) Consider stream as binary rather than text.
      in	(input) Allow input operations on the stream.
      out	(output) Allow output operations on the stream.
      trunc	(truncate) Any current content is discarded, assuming a length of zero on opening.
  
*/
FUNCTION(posix_open) {
	ARG_COUNT(2);
	ARG_str(name,0);
	ARG_int(mode,1);
	int fd = open(*name,mode,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd<0) {
		return ThrowException(String::New(strerror(errno)));
	} else {
		return Integer::New(fd);
	}
} END
/* 
  Function: posix.close(file_descriptor)
*/
FUNCTION(posix_close) {
	ARG_COUNT(1);
	ARG_int(fd,0);
	close(fd);
	return JS_undefined;
} END

/* 
  Function: posix.unlink(path)
*/
FUNCTION(posix_unlink,PSTR(path)) {
	return JS_int(unlink(*path));
} END

/*
  Function: posix.socket(domain, type)
  Creates a socket: endpoint for communication.
 
  Parameters:
    domain - An integer, representing the protocol family of the created socket.
      ie: posix.AF_INET (IPv4), posix.AF_INET6 (IPv6), posix.AF_UNIX (local socket using a file)
    type - An integer: posix.SOCK_STREAM (Stream), posix.SOCK_DGRAM (Datagram), posix.SOCK_SEQPACKET (Sequenced Packet), posix.SOCK_RAW (Raw)
      
    (The protocol is inferred from domain & type.)

  Returns:
    file descriptor integer

  Examples:
  (start code)
    sock = posix.socket(posix.AF_INET, posix.SOCK_STREAM);
    sock = posix.socket(posix.AF_INET6, posix.SOCK_STREAM);
  (end)
 */
FUNCTION(posix_socket,PINT(domain),PINT(type)) {
	int sock = socket(domain,type,0);
	return JS_int(sock);
} END

/*
 Function: posix.bind(sockfd, ip)
 Assigns an address to a socket.

 Parameters:
   sockfd - An integer: Socket File Descriptor, representing the socket to perform the bind on.
   ip - An object: Contains the IP address (addr) and port number (port)
   
   (addrlen is inferred from the address provided)

 Returns:
   0 = success OR -1 = error

 Example:
 (start code)
   sockfd = posix.socket(posix.AF_INET, posix.SOCK_STREAM);
   posix.bind(sockfd, {addr:"127.0.0.1",port:7001});
 (end)
*/
FUNCTION(posix_bind) {
	ARG_COUNT(2);
	ARG_int(sock,0);
	ARG_obj(ip,1);
	struct sockaddr_in addr;
	if (0!=obj2addr(ip,&addr))
		JS_ERROR("invalid IP address");
	if (0!=bind(sock,(struct sockaddr*)&addr,sizeof addr)) 
		JS_ERROR(strerror(errno));
	return Undefined();
} END

/*
  Function: posix.listen(sockfd, backlog)
  Prepares socket for incoming connections.
  
  Parameters:
    sockfd - An integer: Socket File Descriptor, representing the socket to listen on.
    backlog - An integer: Representing the number of pending connections that can be queued up at any one time.
    
  Returns:
    undefined
    
  Example:
  (start code)
    sockfd = posix.socket(posix.AF_INET, posix.SOCK_STREAM);
    posix.bind(sockfd, {addr:"127.0.0.1",port:7001});
    posix.listen(sockfd, {addr:"127.0.0.1",port:7001});
  (end)
 */
FUNCTION(posix_listen) {
  ARG_COUNT(1);
  // if ( ARGC > 0 && ARGC < 3 ) { return ThrowException(String::New("Insufficient arguments")); }
	ARG_int(sock,0);
  // ARG_int(backlog,1);
  int backlog = 8;
	if (listen(sock, backlog)==-1) 
		JS_ERROR(strerror(errno));
	return Undefined();
} END

/* 
  Function: posix.accept(sockfd)
  Creates a new socket for each connection and removes the connection from the listen queue.
  
  Parameters:
    sockfd - An integer: Socket File Descriptor, representing the socket to listen on.
  
  Datagram sockets do not require processing by accept() since the receiver may immediately respond to the request using the listening socket.
  
  Returns:
    The new socket descriptor for the accepted connection, or -1 if an error occurs. All further communication with the remote host now occurs via this new socket.
*/
FUNCTION(posix_accept) {
	ARG_COUNT(1);
	ARG_int(sock,0);
	struct sockaddr_in addr;
	socklen_t len;
	int newsock = accept(sock,(struct sockaddr*)&addr,&len);
	if (-1==newsock)
		JS_ERROR(strerror(errno));
	Handle<Object> addrobj = addr2obj(&addr);
	addrobj->Set(JS_str("sock"),JS_int(newsock));
	return addrobj;
} END

/* 
  Function: posix.connect(sockfd, ip)
  Connects a socket, identified by its file descriptor, to a remote host specified by that host's address in the argument list.
  
  Parameters:
    sockfd - An integer: Socket File Descriptor, representing the socket to listen on.
    ip - An object: Contains the IP address (addr) and port number (port)
  
  Returns:
    undefined
*/
FUNCTION(posix_connect) {
	ARG_COUNT(2);
	ARG_int(sock,0);
	ARG_obj(ip,1);
	struct sockaddr_in addr;
	if (0!=obj2addr(ip,&addr))
		JS_ERROR("invalid IP address");
	if (0!=connect(sock,(struct sockaddr*)&addr,sizeof addr)) 
		JS_ERROR(strerror(errno));
	return Undefined();
} END

/* 
  Function: posix.isDir(path)

  Parameters:
    path - string ie: '/tmp'
  
  Returns:
    Boolean
*/
FUNCTION(posix_isDir,PSTR(path)) {
	struct stat stat_info;
	// TODO: Check errors
	if (stat(*path, &stat_info) != -1) {
		return JS_bool(S_ISDIR(stat_info.st_mode));
	} else {
		return JS_bool(0);
	}
} END

/* 
  Function: posix.isfile(path)

  Parameters:
    path - string ie: '/tmp/file.txt'
  
  Returns:
    Boolean
*/
FUNCTION(posix_isFile,PSTR(path)) {
	struct stat stat_info;
	// TODO: Check errors
	if (stat(*path, &stat_info) != -1) {
		return JS_bool(S_ISREG(stat_info.st_mode));
	} else {
		return JS_bool(0);
	}
} END

/* 
  Function: posix.stat(path)

  Parameters:
    path - string ie: '/tmp/file.txt'
  
  Returns:
    Object
    
    dev_t     - ID of device containing file
    ino_t     - inode number
    mode_t    - protection
    nlink_t   - number of hard links
    uid_t     - user ID of owner
    gid_t     - group ID of owner
    dev_t     - device ID (if special file)
    off_t     - total size, in bytes
    blksize_t - blocksize for filesystem I/O
    blkcnt_t  - number of blocks allocated
    time_t    - time of last access
    time_t    - time of last modification
    time_t    - time of last status change
*/
FUNCTION(posix_stat,PSTR(path)) {

	// struct stat {
	//     dev_t     st_dev;     /* ID of device containing file */
	//     ino_t     st_ino;     /* inode number */
	//     mode_t    st_mode;    /* protection */
	//     nlink_t   st_nlink;   /* number of hard links */
	//     uid_t     st_uid;     /* user ID of owner */
	//     gid_t     st_gid;     /* group ID of owner */
	//     dev_t     st_rdev;    /* device ID (if special file) */
	//     off_t     st_size;    /* total size, in bytes */
	//     blksize_t st_blksize; /* blocksize for filesystem I/O */
	//     blkcnt_t  st_blocks;  /* number of blocks allocated */
	//     time_t    st_atime;   /* time of last access */
	//     time_t    st_mtime;   /* time of last modification */
	//     time_t    st_ctime;   /* time of last status change */
	// };

	struct stat stat_info;
	// TODO: Check errors
	if (stat(*path, &stat_info) != -1) {
		Handle<Object> res = JS_obj();
		OBJECT_SET(res, "dev",   JS_int(stat_info.st_dev));
		OBJECT_SET(res, "ino",   JS_int(stat_info.st_ino));
		OBJECT_SET(res, "mode",  JS_int(stat_info.st_mode));
		OBJECT_SET(res, "nlink", JS_int(stat_info.st_nlink));
		OBJECT_SET(res, "uid",   JS_int(stat_info.st_uid));
		OBJECT_SET(res, "gid",   JS_int(stat_info.st_gid));
		OBJECT_SET(res, "rdev",  JS_int(stat_info.st_rdev));
		OBJECT_SET(res, "size",  JS_int(stat_info.st_size));
		// FIXME: Probably not ints
		OBJECT_SET(res, "blksize",JS_int(stat_info.st_blksize));
		OBJECT_SET(res, "blocks", JS_int(stat_info.st_blocks));
		OBJECT_SET(res, "atime",  JS_int(stat_info.st_atime));
		OBJECT_SET(res, "mtime",  JS_int(stat_info.st_mtime));
		OBJECT_SET(res, "ctime",  JS_int(stat_info.st_ctime));
		OBJECT_SET(res, "IS_DIR", JS_bool(S_ISDIR(stat_info.st_mode)));
		OBJECT_SET(res, "IS_CHR", JS_bool(S_ISCHR(stat_info.st_mode)));
		OBJECT_SET(res, "IS_BLK", JS_bool(S_ISBLK(stat_info.st_mode)));
		OBJECT_SET(res, "IS_REG", JS_bool(S_ISREG(stat_info.st_mode)));
		OBJECT_SET(res, "IS_LNK", JS_bool(S_ISLNK(stat_info.st_mode)));
		OBJECT_SET(res, "IS_FIFO",JS_bool(S_ISFIFO(stat_info.st_mode)));
		OBJECT_SET(res, "IS_SOCK",JS_bool(S_ISSOCK(stat_info.st_mode)));
		return res;
	} else {
		return JS_undefined;
	}
} END


MODULE
	// FIXME: When I set the module 'time' slot to a string, accessing the slot
	// from JavaScript works, but when I BIND it to the posix_time function, the
	// JavaScript returns undefined. Even worse, the next BIND has no effect.
	//module->Set(v8::String::New("time"), JS_str("system.posix.time is a string"));
	BIND("time",      posix_time);
	BIND("fopen",     posix_fopen);
	BIND("fwrite",    posix_fwrite);
	BIND("fread",     posix_fread);
	BIND("fclose",    posix_fclose);
	BIND("writefile", posix_writefile);
	BIND("readfile",  posix_readfile);
	BIND("feof",      posix_feof);
	BIND("popen",     posix_popen);
	BIND("pclose",    posix_pclose);
	BIND("stat",      posix_stat);
	BIND("system",    posix_system);
	BIND("open",      posix_open);
	BIND("close",     posix_close);
	BIND("bind",      posix_bind);
	BIND("listen",    posix_listen);
	BIND("accept",    posix_accept);
	BIND("connect",   posix_connect);
	BIND("socket",    posix_socket);  /**< Bind the posix_socket function to the JavaScript Socket Object */
	BIND("unlink",    posix_unlink);
	// Custom extensions
	BIND("isFile",    posix_isFile);
	BIND("isDir",     posix_isDir);
	// TODO: exists
	SET_int("O_RDWR",    O_RDWR);
	SET_int("O_RDONLY",  O_RDONLY);
	SET_int("O_WRONLY",  O_WRONLY);
	SET_int("O_NONBLOCK",O_NONBLOCK);
	SET_int("O_CREAT",   O_CREAT);
	SET_int("O_TRUNC",   O_TRUNC);
	SET_int("AF_INET", AF_INET);
	SET_int("SOCK_STREAM", SOCK_STREAM);
END_MODULE

// EOF - vim: ts=4 sw=4 noet
