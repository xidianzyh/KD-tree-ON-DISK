#include "rect.h"
#include <algorithm>

using std::min;
using std::max;

// 构造函数
Rect::Rect(int lx, int hx, int ly, int hy, bool blx, bool bhx, bool bly, bool bhy) {
	m_lx = lx;
	m_hx = hx;
	m_ly = ly;
	m_hy = hy;
	b_lx = blx;
	b_hx = bhx;
	b_ly = bly;
	b_hy = bhy;
}

// 判断一个点是否在这个矩形中。
bool Rect::isInRange(int x, int y) const {
	if( (m_lx < x && x < m_hx) || (b_lx == true && m_lx == x)  || (b_hx == true && m_hx == x)){
	
		if((m_ly < y && y < m_hy) || (b_ly == true && m_ly == y) || (b_hy == true && m_hy == y))
			return true;
	}
	return false;				
}

// 判断两个矩形是否相交
bool Rect::isIntersected(const Rect& r) const {
	//两个矩形相交之后依旧是个矩形，求取相交之后矩形的坐标。
	int minx = max(this->m_lx, r.m_lx);
	int maxx = min(this->m_hx, r.m_hx);
	int miny = max(this->m_ly, r.m_ly);
	int maxy = min(this->m_hy, r.m_hy);
	
	if(minx > maxx || miny > maxy)  
		return false;
	if(minx == maxx) {
		//两个矩形相交于一条y轴的边
		if(m_hx < r.m_hx){
			if(b_hx == false || r.b_lx == false)
				return false;
		}
		if(r.m_hx < m_hx) {
			if(r.b_hx == false || this->b_lx == false )
				return false;
		}
	}

	if(miny == maxy) {
		//两个矩形相交于一条x轴的边上。
		if(m_hy < r.m_hy) {
			if(this->b_hy == false || r.b_ly == false)
				return false;
		}

		if(r.m_hy < m_hy) {
			if(r.b_hy == false || this->b_ly == false)
				return false;
		}
		
	}

	return true;
}


//if fully contained rect r
bool Rect::isContained(const Rect& r) const {
	//判断是否完全包含矩形 r.
	if(r.m_lx <  this->m_lx)
		return false;
	if(r.m_lx == this->m_lx && r.b_lx == true && this->b_lx == false)
		return false;

	if(r.m_hx > this->m_hx)
		return false;
	if(r.m_hx == this->m_hx && r.b_hx == true && this->b_hx == false)
		return false;
	
	if(r.m_ly < this->m_ly)
		return false;
	if(r.m_ly == this->m_ly && r.b_ly == true && this->b_ly == false)
		return false;
	
	if(r.m_hy > this->m_hy)
		return false;
	if(r.m_hy == this->m_hy && r.b_hy == true && this->b_hy == false)
		return false;

	return true;

}




















