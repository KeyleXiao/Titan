
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/
#ifndef __BufferDef_H__
#define __BufferDef_H__

enum BufferUsage 
{
	/** Static buffer which the application rarely modifies once created. Modifying 
	the contents of this buffer will involve a performance hit.
	*/
	BU_STATIC = 1,
	/** Indicates the application would like to modify this buffer with the CPU
	fairly often. 
	Buffers created with this flag will typically end up in AGP memory rather 
	than video memory.
	*/
	BU_DYNAMIC = 2,
	/** Indicates the application will never read the contents of the buffer back, 
	it will only ever write data. Locking a buffer with this flag will ALWAYS 
	return a pointer to new, blank memory rather than the memory associated 
	with the contents of the buffer; this avoids DMA stalls because you can 
	write to a new memory area while the previous one is being used. 
	*/
	BU_WRITE_ONLY = 4,
	/** Indicates that the application will be refilling the contents
	of the buffer regularly (not just updating, but generating the
	contents from scratch), and therefore does not mind if the contents 
	of the buffer are lost somehow and need to be recreated. This
	allows and additional level of optimisation on the buffer.
	This option only really makes sense when combined with 
	BU_DYNAMIC_WRITE_ONLY.
	*/
	BU_DISCARDABLE = 8,
	/// Combination of BU_STATIC and BU_WRITE_ONLY
	BU_STATIC_WRITE_ONLY = 5, 
	/** Combination of BU_DYNAMIC and BU_WRITE_ONLY. If you use 
	this, strongly consider using BU_DYNAMIC_WRITE_ONLY_DISCARDABLE
	instead if you update the entire contents of the buffer very 
	regularly. 
	*/
	BU_DYNAMIC_WRITE_ONLY = 6,
	/// Combination of BU_DYNAMIC, BU_WRITE_ONLY and BU_DISCARDABLE
	BU_DYNAMIC_WRITE_ONLY_DISCARDABLE = 14

};

enum IndexType 
{
	IT_16BIT,
	IT_32BIT
};

enum LockOptions
{
	/** Normal mode, ie allows read/write and contents are preserved. */
	BL_NORMAL,
	/** Discards the <em>entire</em> buffer while locking; this allows optimisation to be 
	performed because synchronisation issues are relaxed. Only allowed on buffers 
	created with the HBU_DYNAMIC flag. 
	*/
	BL_DISCARD,
	/** Lock the buffer for reading only. Not allowed in buffers which are created with HBU_WRITE_ONLY. 
	Mandatory on statuc buffers, ie those created without the HBU_DYNAMIC flag. 
	*/ 
	BL_READ_ONLY,
	/** As BL_NORMAL, except the application guarantees not to overwrite any 
	region of the buffer which has already been used in this frame, can allow
	some optimisation on some APIs. */
	BL_NO_OVERWRITE

};

#endif
	/** @} */