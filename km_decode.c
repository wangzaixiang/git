//
// Created by 王在祥 on 2021/12/20.
//

#include "builtin.h"
#include "km_decode.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "strbuf.h"

static int do_write(int fd, char *data, unsigned long size);
static int do_read(int fd, void **data, unsigned long *size);
int decode_km(char *path, void *data, unsigned long size, void **decoded_data,
	      unsigned long *decoded_size)
{
	int child_stdin[2], child_stdout[2];
	if(pipe(child_stdin) < 0){
		return -1;
	}
	if(pipe(child_stdout) < 0) {
		close(child_stdin[0]);
		close(child_stdin[1]);
		return -2;
	}

	pid_t child = fork();
	if(child == 0){
		// stdin
		close(child_stdin[1]);
		dup2(child_stdin[0], 0);
		// stdout
		dup2(child_stdout[1], 1);
		close(child_stdout[0]);
		char * args[] = { "gocat", NULL};
		if(execvp("gocat", args) < 0){
			perror("can't exec command gocat");
			exit(100);
		}
	}
	else if(child > 0){
		close(child_stdin[0]);
		close(child_stdout[1]);
		if( do_write(child_stdin[1], (char*)data, size) != 0){
			return -3;
		};
		close(child_stdin[1]);

		if( do_read(child_stdout[0], decoded_data, decoded_size) != 0){
			return  -4;
		}

		int status = 0;
		if(waitpid(child, &status, 0) <0){
			close(child_stdout[0]);
			return -5;
		}
		if(WIFEXITED(status)){
			return -1 * WEXITSTATUS(status);
		}
		else {
			return -6;
		}
	}
	return 0;
}
static int do_read(int fd, void **data, unsigned long *size)
{
	struct strbuf buffer;
	int length = 0;

	strbuf_init(&buffer, 1024);

	char line[1024];
	for(;;){
		int n = read(fd, line, sizeof(line));
		if(n < 0) return -1;
		else if(n == 0) break;
		else {
			strbuf_grow(&buffer, n);
			strbuf_add(&buffer, line, n);
		}
	}
	*data = (void*)strbuf_detach(&buffer, size);
	strbuf_release(&buffer);
	return 0;
}

static int do_write(int fd, char *data, unsigned long size)
{
	long writed = 0;
	while(writed < size){
		int n = write(fd, data + writed, size - writed);
		if(n == -1) return -1;
		writed += n;
	}
	return 0;
}
