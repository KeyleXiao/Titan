#ifndef __PATH_H__
#define __PATH_H__

#include "Common.h"

#include <string>

#pragma warning(disable: 4251)

#define  CHAR_SLASH			'\\'
#define  CHAR_DOT			'.'
#define  CHAR_SLASH2		'/'

namespace rkt{


#define stringIsNull(strPtr)		( (strPtr) == NULL )
#define stringIsEmpty(strPtr)		( (strPtr)[0] == '\0' )
#define stringIsValid(strPtr)		( (strPtr) != NULL && (strPtr)[0] != '\0' )

template <class _Elem>
class basic_path : public std::basic_string<_Elem>
{
public:
	typedef typename std::basic_string<_Elem>	_String;
	typedef typename basic_path<_Elem>			_MyType;

	///////////////////////////////////////////////////////////////////////////////
	// 构造函数
	basic_path()
		: _isFile(true)
	{
	}
	basic_path(const _Elem* p, bool isFilePath = true)
		: _isFile(isFilePath), _String(p)
	{
		//normalize();
	}
	basic_path(const _Elem* p, size_type roff, size_type count, bool isFilePath = true) 
		: _isFile(isFilePath), _String(p,roff,count)
	{
		normalize();
	}
	basic_path(const _String& str, bool isFilePath = true)
		: _isFile(isFilePath), _String(str)
	{
		normalize();
	}
	basic_path(const _MyType& path) 
		: _isFile(path._isFile), _String(path)
	{

	}
	basic_path(const _MyType& path, bool isFilePath) 
		: _isFile(isFilePath), _String(path)
	{

	}
	_MyType& operator+(_Elem _Ch)
	{
		_String::assign(1, _Ch);
		return (*this);
	}

	/*
	~basic_path()
	{
		outputStringA("%s\n", (const char*)(*this));
	}*/
/*
	operator _String()
	{
		return (*this);
	}*/

	/** 标准化路径:
	 * 工序: 
	 *		1.修剪两边的空格
	 *		2.把['/']统一替换成['\']
	 */
	void normalize()
	{
		_Elem slash[2] = {CHAR_SLASH};
		_Elem slash2[2] = {CHAR_SLASH2};
		replace_all(_String(slash2), _String(slash));
	}

	void replace_all(const _String& substr,const _String& str)
	{
		size_type search_len=substr.length();
		if (search_len==0)
			return;

		long/*size_type*/ pos = (long)(length()-search_len); // 原来size_type是无符号的
		for (;pos>=0;)
		{
			if (compare(pos,search_len,substr)==0)
			{
				replace(pos,search_len,str);
				pos -= (long)search_len;
			}else
			{
				--pos;
			}
		}	
	}

	_String getFileName() const
	{
		if (!isFile())
			return _String();

		size_type pos = getLastSlashPos();

		return substr(pos + 1, length() - pos - 1);
	}

	_String getFileTitle() const
		{
		if (!isFile())
			return _String();

		std::string path=this->c_str();
		char filename[MAX_PATH]={0};
		_splitpath(path.c_str(),NULL,NULL,filename,NULL);
		if (!filename)
			return _String();

		return filename;

		//size_type slashPos = getLastSlashPos();
		//size_type dotPos = getLastDotPos();
		//return substr(slashPos + 1, dotPos - slashPos - 1);
	}

	_String getFileExt() const
	{
		if (!isFile() || empty())
			return _String();

		std::string path=this->c_str();
		char ext[16]={0};
		_splitpath(path.c_str(),NULL,NULL,NULL,ext);
		if (!ext)
			return _String();

		return ext+1;

		//size_type pos = getLastDotPos();
		//if (pos == npos) // no ext
		//	return _String();

		//return substr(pos + 1, length() - pos - 1);
	}

	_String getParentDir()
	{
		if (!isFile())
			removeTailSlash();

		size_type pos = getLastSlashPos();
		if (pos == npos)
			return _String();
		return substr(0, pos);
	}

	_MyType& addTailSlash()
	{
		if (*(end() - 1) != CHAR_SLASH)
			*this += CHAR_SLASH;
		return (*this);
	}

	_MyType& removeTailSlash()
	{
		if (*(end() - 1) == CHAR_SLASH)
			erase(end() - 1);
		return (*this);
	}

	void isFile(bool isfile)
	{
		_isFile = isfile;
	}
	bool isFile() const
	{
		return _isFile;
	}
protected:
	inline size_type getLastDotPos() const
	{
		return find_last_of(CHAR_DOT);
	}
	inline size_type getLastSlashPos() const
	{
		return find_last_of(CHAR_SLASH);
	}
	inline size_type getLastSlashPos(int count) const
	{
		size_type pos;
		pos = find_last_of(CHAR_SLASH);
		while (pos != npos && --count > 0)
			pos = find_last_of(CHAR_SLASH, pos - 1);
		return pos;
	}
private:
	bool	_isFile;
};

typedef basic_path<char>			PathA,CPathA,Path,CPath;
typedef basic_path<wchar_t>			PathW,CPathW;

/*
#ifdef RKT_UNICODE
typedef basic_path<wchar_t>			Path,CPath;
#else
typedef basic_path<char >			Path,CPath;
#endif
*/



//////////////////////////////////////////////////////////////////////////
	/// 设置工作目录
	RKT_API	void		setWorkDir(const char* path);

/// 获取工作目录
	RKT_API const char* getWorkDir();

	/** 切换到文件的全路径名
	@param path 要转换的文件名（可以是当前目录的相对路径）
	@note 不检查文件或文件所在的各级目录是否存在，检查是否存在使用@c checkPath
	*/
	RKT_API void toggleFullPath(Path& path);

	/** 检查路径是否存在（包括目录路径和文件路径）
	@param path 路径名
	@param isAbsolutePath 是否是绝对路径
	@param attrib 返回文件（或目录）的属性
	@return 成功返回true，否则返回false
	*/
	RKT_API bool checkPath(const char* path, bool& isAbsolutePath, uint& attrib);

	/** 检查路径是否是文件
	@param path 要检查的路径（如果是相对路径，会自动以exe进程所在目录为当前目录进行处理）
	@return 成功返回true，否则如果路径不存在或者是目录则返回false
	*/
	RKT_API bool isFile(const char* path);

	/** 检查路径是否是目录
	@param path 要检查的路径（如果是相对路径，会自动以exe进程所在目录为当前目录进行处理）
	@return 成功返回true，否则如果路径不存在或者是文件则返回false
	*/
	RKT_API bool isDirectory(const char* path);

	/** 检查指定的路径是否是绝对路径
	@param path 要检查的路径
	@return 绝对路径返回true，否则返回false
	*/
	RKT_API bool isAbsolutePath(const char* path);

	/** 递归创建目录
	@param absolutePath 要创建的绝对路径
	@return 成功或者目录已存在返回true，否则返回false，失败后不处理可能创建了的目录
	*/
	RKT_API bool createDirectoryRecursive(const char* absolutePath);

	/** 递归删除目录
	@param absolutePath 要删除的绝对路径
	@return 成功或者目录不存在返回true，否则返回false
	*/
	RKT_API bool removeDirectoryRecursive(const char* absolutePath);

	/// 目录浏览mask
	enum BrowseDirMask{bdm_file=0x1, bdm_dir=0x2};

	/** 浏览目录
	@param absoluteDir 绝对目录路径
	@param relativeDir 相对目录，用于记录绝对目录下面所有的子目录的相对位置
	@param DIRPROC 目录回调函数
	@param FILEPROC 文件回调函数
	@param userdata 用户提供的私有数据
	@param mask 用于浏览文件和目录的mask，mask＝0x1表示只浏览目录，mask=0x2表示只浏览文件，可进行or操作
	@return 成功返回true，否则返回false
	@note 如：browseDirectory("e:\\mytestdir", "", CPkgOperation::dirProc, CPkgOperation::fileProc, (uint)this, 3);
	*/
	RKT_API bool browseDirectory(const char* absoluteDir, const char* relativeDir, bool (*DIRPROC)(const char*, const char*, uint), 
								 bool (*FILEPROC)(const char*, const char*, uint), uint userdata, uint mask = bdm_file|bdm_dir,bool recursive = true);


	enum DFP_RESULT {DFP_ABORT = 0, DFP_SUCCESS = 1, DFP_IGNORE = 2};
	/// 目录或者文件的回调函数签名
	/// 返回值：0-中止所有浏览过程,1-继续当前目录（或文件）的处理，2-忽略当前目录（或文件）的处理，可用于过滤操作
	typedef DFP_RESULT (*DirFileProc)(bool isFile, const char* absolutePath, const char* relativePath, const char* name, uint userdata);
	/** 浏览目录
	@param absoluteDir 绝对目录路径
	@param relativeDir 相对目录，用于记录绝对目录下面所有的子目录的相对位置
	@param dirFileProc 目录和文件的回调函数
	@param userdata 用户提供的私有数据
	@param mask 用于浏览文件和目录的mask，mask＝0x1表示只浏览目录，mask=0x2表示只浏览文件，可进行or操作
	@return 成功返回true，否则返回false
	@note 如：browseDirectoryEx("e:\\mytestdir", "", CPkgOperation::dirFileProc, (uint)this);
	*/
	RKT_API bool browseDirectoryEx(const char* absoluteDir, 
								   const char* relativeDir, 
								   DirFileProc dirFileProc, 
								   uint userdata = 0, 
								   uint mask = bdm_file|bdm_dir, 
								   bool recursive = true, 
								   bool browseHidden = false
								   );

}

#endif // __PATH_H__
