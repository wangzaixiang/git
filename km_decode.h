//
// Created by 王在祥 on 2021/12/20.
//

#ifndef GIT_KM_DECODE_H
#define GIT_KM_DECODE_H

int decode_km(char *path, void *data, unsigned long size, void **decoded_data,
	      unsigned long *decoded_size);

#endif // GIT_KM_DECODE_H
