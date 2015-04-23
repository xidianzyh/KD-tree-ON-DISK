#ifndef INCLUDED_RECT_H
#define INCLUDED_RECT_H

//定义一个矩形，这个矩形有四条边

class Rect{

public:
	int m_lx;
	int m_hx;
	int m_ly;
	int m_hy;

	bool b_lx;
	bool b_hx;
	bool b_ly;
	bool b_hy;

public:
	//构造函数
	Rect(int lx, int hx, int ly, int hy, bool blx, bool bhx, bool bly, bool bhy);
	
	//判断一个点是否在这个矩形中
	bool isInRange(int x, int y) const;

	//判断两个矩形是否相交
	bool isIntersected(const Rect& r) const;

	//判断是否包含矩形R
	bool isContained(const Rect& r) const;

	int getLowX(){return m_lx;}
	int getHighX(){return m_hx;}
	int getLowY(){return m_ly;}
	int getHighY(){return m_hy;}

	void setLowX(int lx) { m_lx = lx;}
	void setLowX(int lx, bool blx) { m_lx = lx; b_lx = blx;}
	void setHighX(int hx) { m_hx = hx;}
	void setHighX(int hx, bool bhx) { m_hx = hx; b_hx = bhx;}
	void setLowY(int ly) { m_ly = ly;}
	void setLowY(int ly, bool bly) { m_ly = ly; b_ly = bly;}
	void setHighY(int hy) { m_hy = hy;}
	void setHighY(int hy, bool bhy) { m_hy = hy; b_hy = bhy;}

};












#endif
