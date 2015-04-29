#ifndef INCLUDED_KDDISK_H
#define INCLUDED_KDDISK_H

#include <cstdint>
#include <cstddef>
#include <cstdio>

#include "sdsl/int_vector.hpp"
#include "rect.h"
#include "DiskFile.h"



class KDDisk {

public:
	static int32_t 	m_B;		//确保初始化数据的时候，所有数据的大小少于一个磁盘页。
	static int32_t	m_bits;		// m_bits == 32 or 64

	Rect 			m_rootRect; //根节点对应的Bounding Box, size = 4*8+8=40 bytes
	int32_t 		m_depth;	//跟节点对应的深度，size = 4 bytes.
	int32_t 		m_diskNum;	//当前块的磁盘号,   size = 4 bytes.

	int32_t 		m_np;
	int32_t			m_nc;
	sdsl::int_vector<2> 	*treeShape;  	// two bit for a node(00, 01, 10, 11)
	sdsl::int_vector<0>		*splitValue;	// the max bit of (m_xBitWidth, m_yBitWidth)
	sdsl::int_vector<32> 	*childPointer;	// the disk number of child.

public:
	//构造函数
	KDDisk(Rect r, int32_t depth);

	//析构函数
	~KDDisk();

	//初始化函数，对int_Vector进行初始化。
	void init(int32_t np, int32_t nc);
 	
	//对KDDisk中的三个 int_vector 进行写操作
	void writeTreeShape(uint64_t idx, uint64_t value);
	
	void writeSplitValue(uint64_t idx, uint64_t value);

	void writeChildPointer(uint64_t idx, uint64_t value);

	//将KDDisk中的内容写入磁盘中，返回写入的磁盘编号。
	int32_t writeToDisk(DiskFile* diskOut);
	
	//返回 写入的数据。
	int64_t writePadding(uint64_t bytes, FILE* out);

	static void Set(int32_t B, int32_t bits);
	static bool OverFlow(int32_t np, int32_t nc);

};



#endif
