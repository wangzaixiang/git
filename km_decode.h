//
// Created by 王在祥 on 2021/12/20.
//

#ifndef GIT_KM_DECODE_H
#define GIT_KM_DECODE_H

int decode_km(char *path, void *data, unsigned long size, void **decoded_data,
	      unsigned long *decoded_size);

int write_object_file_km(char *path, const void *buf, unsigned long len,
			 const char *type, struct object_id *oid);
#endif // GIT_KM_DECODE_H
