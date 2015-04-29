#include "KDNode.h"

#include <iostream>
#include <cassert>

using std::make_pair;

//构造函数
KDNode::KDNode(uint64_t x, uint64_t y, int32_t depth, Rect r) : m_rect(r){
	m_x = x;
	m_y = y;
	m_split = (depth % 2 == 1) ? true :false;
	m_depth = depth;
	m_PointNode = false;		// It is not a point node.
	m_leftChild = NULL;
	m_rightChild = NULL;
}

/***** Create a KDNode storing the points.
*
********/
KDNode::KDNode(vector<uint64_t>& vx, 
			vector<uint64_t>& vy, 
			pair<uint64_t, uint64_t> range, 
			int32_t depth): m_rect(0, 0, 0, 0){

	m_split = (depth % 2 == 1) ? true : false;
	m_depth = depth;
	m_leftChild = NULL;
	m_rightChild = NULL;

	m_PointNode = true;
	m_point = new vector<pair<uint64_t, uint64_t> >();
	for(uint64_t i = range.first; i <= range.second; i++){
		m_point->push_back(make_pair(vx[i], vy[i]));
	}
}

//deconstructor.
KDNode::~KDNode(){
	if(isPointNode()){
		assert(m_point != NULL);
		delete m_point;
	}
}

uint64_t KDNode::getX(){return m_x;}
uint64_t KDNode::getY(){return m_y;}

void KDNode::setX(uint64_t x) { m_x = x;}
void KDNode::setY(uint64_t y) { m_y = y;}
void KDNode::set(uint64_t x, uint64_t y) { m_x = x; m_y = y;}

bool KDNode::getSplit() {return m_split;}
void KDNode::setSplit(bool split) { m_split = split;}

bool KDNode::isLeaf() {return ((m_leftChild == NULL) && (m_rightChild == NULL));}

KDNode* KDNode::getLeftChild() {return m_leftChild;}
KDNode* KDNode::getRightChild() {return m_rightChild;}

void KDNode::setLeftChild(KDNode* left) { m_leftChild = left;}
void KDNode::setRightChild(KDNode* right) {m_rightChild = right;}


Rect 		KDNode::getRect() { return this->m_rect;}
int32_t 	KDNode::getDepth(){ return this->m_depth; }

uint64_t KDNode::getSplitValue() {
	if(m_split) {
		return m_x;
	} else {
		return m_y;
	}
}


bool KDNode::isPointNode() {return m_PointNode;}

uint64_t KDNode::getPointNum() {
	if(m_point == NULL || !isPointNode()){
		assert(false);
	}
	return m_point->size();
}

pair<uint64_t, uint64_t> KDNode::getPoint(uint64_t i){
	if(m_point == NULL || i >= getPointNum())
		assert(false);

	pair<uint64_t, uint64_t> p = (*m_point)[i];
	return p;
}

/******
1. 当我想设定一个成员函数为 inline的时候，inline应该放在哪个地方。
2. 如何判断叶子节点：
	这里有两种方式，第一种是设定一个标志位，表示它是否是叶子节点
	第二种：若左右子树同时为空，那么它也是叶子节点。

如果采用标志位的方法，那么会出现一致性的问题：如果你把一个标志位设置成了true，表示这是一个叶子节点，但是同时左右孩子不为空，应该怎么办?

这就是一致性问题，因为冗余才会引起一致性的问题。
在分布式的环境中，冗余是必须的，比如说每个node上都有一个 cache data。如何保证这些data是一致性的呢？


*******/















