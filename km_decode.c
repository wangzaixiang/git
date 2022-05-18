//
// Created by 王在祥 on 2021/12/20.
//

#include "builtin.h"
#include "object-store.h"
#include "km_decode.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "strbuf.h"

static int do_write(int fd, char *data, unsigned long size);
static int do_read(int fd, void **data, unsigned long *size);


static int decode_encode_km(int decode, char *path, void *data, unsigned long size, void **decoded_data,
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
		char * args0[] = { "gocat", NULL};
		char * args1[] = { "gocat","-e", NULL};
		char **args = (decode == 1) ? args0 : args1;
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

int decode_km(char *path, void *data, unsigned long size, void **decoded_data,
	      unsigned long *decoded_size) {
	return decode_encode_km(1, path, data, size, decoded_data, decoded_size);
}
int encode_km(char *path, void *data, unsigned long size, void **encoded_data, unsigned  long *encoded_size) {
	return decode_encode_km(0, path, data, size, encoded_data, encoded_size);
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

int write_object_file_km(char *path, const void *buf, unsigned long len,
				    const char *type, struct object_id *oid) {
	int fEncode = 0;

	if(ends_with(path, ".java")) fEncode = 1;

	char *buf2;
	unsigned long len2;
	if(fEncode && encode_km(path, buf, len, &buf2, &len2) == 0){
		int ret  = write_object_file(buf2, len2, type, oid);
		free(buf2);
		return ret;
	}
	else {
		return write_object_file(buf, len, type, oid);
	}
}