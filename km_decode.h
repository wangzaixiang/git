//
// Created by 王在祥 on 2021/12/20.
//

#ifndef GIT_KM_DECODE_H
#define GIT_KM_DECODE_H

int is_encoded(const char *data, unsigned long length);

int decode_km(void *data, unsigned long size, void **decoded_data,
	      unsigned long *decoded_size);
int encode_km(void *data, unsigned long size, void **encoded_data,
	      unsigned  long *encoded_size);

#endif // GIT_KM_DECODE_H
