#include "KDDisk.h"

using namespace sdsl;

/*define the static variable.*/
int32_t KDDisk::m_B = -1;
int32_t KDDisk::m_bits = -1;


KDDisk::KDDisk(Rect r, int32_t depth){

	m_rootRect = r;
	m_depth = depth;

	treeShape = NULL;
	splitValue = NULL;
	childPointer = NULL;
}

KDDisk::~KDDisk() {
	delete treeShape;
	delete splitValue;
	delete childPointer;
}

void KDDisk::init(int32_t nparent, int32_t nchild){
	
	m_np = nparent;
	m_nc = nchild;
	treeShape = new int_vector<2>(nparent);
	splitValue = new int_vector<0>(nparent, 0, m_bits);
	childPointer = new int_vector<32>(nchild);
}

 void KDDisk::Set(int32_t B, int32_t bits){
	m_B = B;
	//assert(bits == 32 || bits == 64);
	m_bits = bits;
}

 bool KDDisk::OverFlow(int32_t np, int32_t nc){

	int32_t totalBits = 0;  // in bits.
	totalBits += ((np * 2 + 63) / 64) * 64;  	// the shape of tree.
	totalBits += ((np * m_bits + 63) / 64) * 64;  	// the internal value (4bytes/8bytes).
	totalBits += ((nc * 32 + 63) / 64) * 64;  	// the child pointer.
	totalBits += (Rect::BytesInDisk() + 4 + 4)*8;  // Rect + deepth + diskNum.
	totalBits += (4+4)*8; 						// nparent and nchild.
	totalBits += (1+1+1)*8;						// width of int_vector.	

	if(totalBits  > m_B*8)
		return true;
	return false;
}

/***********************************************
* int_vector 很方便，预计要在两天的时间内掌握它。
***********************************************/
void KDDisk::writeTreeShape(uint64_t idx, uint64_t value){
	(*treeShape)[idx] = value; 
}

void KDDisk::writeSplitValue(uint64_t idx, uint64_t value){

	(*splitValue)[idx] = value;
}

void KDDisk::writeChildPointer(uint64_t idx, uint64_t value){
	(*childPointer)[idx] = value;
}

/*****************************
* 将一个disk page 写入到磁盘中。
* @return  返回需要写入的磁盘号。
* 
******************************/
int32_t KDDisk::writeToDisk(DiskFile* diskOut){
	
	FILE*	out = diskOut->m_out;  //写入文件的数据流, out 已经指向需要写入的磁盘号。
	m_diskNum = diskOut->m_diskNum;	//需要写入的磁盘号。
	
	/**calculate the total bytes that need to write **/
	int32_t totalBytes = 0;
	totalBytes += (Rect::BytesInDisk()) + 4 + 4;  // bounding box + deepth + diskNum;
	totalBytes += 4 + 4;	   // number of parent  + number of child.
	totalBytes += 1 + 1 + 1;   // the width of every int_vector.
	totalBytes += treeShape->capacity() >> 3;	// the bytes of treeShape
	totalBytes += splitValue->capacity() >> 3;  // the bytes of the splitValue
	totalBytes += childPointer->capacity() >> 3;  // the bytes of the childPointer
	
	assert(totalBytes < m_B);

	uint8_t width = 0;
	/**** 将所有需要的数据写入磁盘中 ******/
	int32_t written = 0;
	written += m_rootRect.writeToDisk(out);  // write Rect
	written += fwrite(&m_depth, 1, sizeof(int32_t), out);  // write deepth
	written += fwrite(&m_diskNum, 1, sizeof(int32_t), out);
	written += fwrite(&m_np, 1, sizeof(int32_t), out);
	written += fwrite(&m_nc, 1, sizeof(int32_t), out);
	width = treeShape->get_int_width();						// write the width of the int_vector
	written += fwrite(&width, 1, sizeof(uint8_t), out);
	width = splitValue->get_int_width();
	written += fwrite(&width, 1, sizeof(uint8_t), out);
	width = childPointer->get_int_width();
	written += fwrite(&width, 1, sizeof(uint8_t), out);
	
	uint64_t data_len = 0;
	const uint64_t* data  = NULL;
	data_len = treeShape->capacity() >> 3;  // trun bits into bytes.
	data = treeShape->data();
	written += fwrite(data, 1, data_len, out);	//write treeShape

	data_len = splitValue->capacity() >> 3; 
	data 	 = splitValue->data();
	written += fwrite(data, 1, data_len, out); // write splitValue
	
	data_len = childPointer->capacity() >> 3;
	data	 = childPointer->data();
	written += fwrite(data, 1, data_len, out); // write childPointer
	
	assert(written == totalBytes);
	
	writePadding(m_B - written, out);
	
	diskOut->m_diskNum++;  //下一个写入的磁盘号+1.

	return m_diskNum;      //返回写入的磁盘编号。
}	
	

/*************************
* write padding
**************************/
int64_t KDDisk::writePadding(uint64_t bytes, FILE* out){

	if(bytes){
		uint8_t* dummy = (uint8_t*)malloc(sizeof(uint8_t)*bytes);	
		memset(dummy, 0, bytes);
		int64_t written = fwrite(dummy, 1, bytes, out);  //write dummy
		free(dummy);
		return written;
	}
	return 0;
}


















