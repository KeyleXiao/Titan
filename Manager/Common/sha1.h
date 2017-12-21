
#ifndef _SHA1_H
#define _SHA1_H

#include <memory.h>
#include <stdio.h>

class ctx_sha1 
{
public:
	ctx_sha1();

public:
	void initialize(void);
	void update(const unsigned char *pdata, unsigned long count);
	void finish(unsigned char cid[20]);
	
protected:
	void handle(unsigned long *state, const unsigned char block[64]);

protected:
	unsigned long _state[5];
	unsigned long _count[2];
	unsigned char _inner_data[64];
};

// 将一个字符串转化成16进制
extern void string_to_hex( const unsigned char * str,int len,char * out_buff );

/******************************************************************************
 *  unsigned char cid[20]; 
 *  ctx_sha1 hash; 

 *  hash.update((unsigned char *)("a"), 1);
 *  hash.update((unsigned char *)("b"), 1);
 *  hash.finish(cid);   
 *****************************************************************************/
#endif