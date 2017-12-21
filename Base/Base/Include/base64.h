/**
 * File        : base64.h
 * Author      : Patrick Meier, patrick.meier@gmx.net
 * Date        : 10-11-2000
 * Last change : 12-05-2000 / Patrick Meier
 * Description : moule to en- and decode a base64 coded string
 *
 * Copyright (C) 2000 Patrick Meier
 *
 ****************************************************************************
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 ****************************************************************************
 * $Id$ 
 ****************************************************************************
 */ 


/******************************************************************************/
/* DEFINES                                                                    */
/******************************************************************************/
#ifndef __BASE64_H__
#define __BASE64_H__

#include "Common.h"

#define TYPE1
								/* 两个 BASE64 编码程序，通过这个宏定义来选择使用哪个。 */

#ifdef SUPPORT_BASE64

namespace rkt {
/**
 * Callculate the encode buffersize of a given length
 * @param length of the decoded string
 * @return length of the encoded string
 */
RKT_API int base64encodeSize( unsigned int len );


/**
 * Callculate the decode buffersize of a given length
 * @param length of the encoded string
 * @return length of the decoded string
 */
RKT_API int base64decodeSize( unsigned int len );

/*#ifdef TYPE1*/
/**
 * Encode a given string with base64
 * @param string to encode
 * @param length of the string
 * @param encoded string 
 * @param length of the buffer
 * @return number of written characters (-1 on error)
 */
RKT_API int base64encode( unsigned char *src, int dlen, unsigned char *target, int targsize );


/**
 * Decode a given string with base64
 * @param base64 coded string to decode
 * @param decoded string 
 * @param length of the buffer
 * @return number of written characters (-1 on error)
 */
RKT_API int base64decode( unsigned char *src, unsigned char *target, int targsize );


//#else		/* #ifdef TYPE1 */

//#endif		/* #ifdef TYPE1 */

} // namespace rkt

#endif //SUPPORT_BASE64

#endif		/* #ifndef __BASE64_H__ */

/******************************************************************************/
/* EOF                                                                        */
/******************************************************************************/


