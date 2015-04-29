#include <cassert>
#include <cstdlib>

#include "KDPointDisk.h"

int32_t KDPointDisk::m_B = -1;
int32_t KDPointDisk::m_xBitWidth = -1;
int32_t KDPointDisk::m_yBitWidth = -1;

KDPointDisk::KDPointDisk(uint64_t nPoint){
	m_vx = new int_vector<0>(nPoint, 0, m_xBitWidth);
	m_vy = new int_vector<0>(nPoint, 0, m_yBitWidth);
}

// void KDPointDisk::init(uint64_t nPoint) {

// 	m_vx = new int_vector<0>(nPoint, 0, m_xBitWidth);
// 	m_vy = new int_vector<0>(nPoint, 0, m_yBitWidth);
// }

KDPointDisk::~KDPointDisk(){
	delete m_vx;
	delete m_vy;
}

// We set it in the begining of the program.
void KDPointDisk::Set(int32_t B, int32_t xBitWidth, int32_t yBitWidth){
	m_B = B;
	m_xBitWidth = xBitWidth;
	m_yBitWidth = yBitWidth;
}

// The maximum of points that can store in a disk page.
int32_t KDPointDisk::GetMaxPoint() {
	int32_t totalBytes = 0;
	totalBytes += 8 + 1 + 1;  //#points, xBitWidth, yBitWidth;
	int32_t max = 0;
	max = ((m_B - totalBytes)*8 - 64*2) / (m_xBitWidth + m_yBitWidth);
	return max;
}

void KDPointDisk::setPoint(uint64_t idx, uint64_t xValue, uint64_t yValue) {
	(*m_vx)[idx] = xValue;  // I have write m_vx[idx], how c++ can do this?
	(*m_vy)[idx] = yValue;
}

int32_t KDPointDisk::writeToDisk(DiskFile* diskOut){

	int32_t diskNum = diskOut->m_diskNum;		// the disk nubmer that it need to write into
	FILE*	out 	= diskOut->m_out;		// the file descripter.

	assert(m_vx->size() == m_vy->size());
	uint64_t nPoint = m_vx->size();				//8 byte
	uint8_t  xWidth = m_vx->get_int_width();	//1 byte
	uint8_t	 yWidth = m_vy->get_int_width();	//1 byte

	int64_t totalBytes = 0;
	totalBytes += 8 + 1 + 1;
	totalBytes += m_vx->capacity() >> 3;  // turn bits into bytes.
	totalBytes += m_vy->capacity() >> 3;  // turn bits into bytes.
	assert(totalBytes <= m_B);

	int32_t written = 0;
	written += fwrite(&nPoint, 1, sizeof(uint64_t), out);
	written += fwrite(&xWidth, 1, sizeof(uint8_t), out);
	written += fwrite(&yWidth, 1, sizeof(uint8_t), out);

	const uint64_t* data = NULL;
	uint64_t dataLen = 0;

	data = m_vx->data();
	dataLen = m_vx->capacity() >> 3;
	written += fwrite(data, 1, dataLen, out);	// write m_vx

	data = m_vy->data();
	dataLen = m_vy->capacity() >> 3;
	written += fwrite(data, 1, dataLen, out); 	// write m_vy

	assert(written == totalBytes);

	writePadding(m_B - written, out);
	diskOut->m_diskNum++;
	return diskNum;			/*return the diskNum that it writes into*/
}


int32_t KDPointDisk::writePadding(int64_t bytes, FILE* out){

	if(bytes){
		uint8_t * dummy = (uint8_t*)malloc(sizeof(uint8_t) * bytes);
		memset(dummy, 0, bytes);
		int32_t written = fwrite(dummy, 1, bytes, out);
		free(dummy);
		return written;
	}
	return -1;
}



