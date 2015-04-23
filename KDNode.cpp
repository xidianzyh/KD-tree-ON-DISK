#include "KDNode.h"
#include <iostream>

//构造函数
KDNode::KDNode(int x, int y, bool split){
	m_x = x;
	m_y = y;
	m_split = split;

}


int KDNode::getX(){return m_x;}
int KDNode::getY(){return m_y;}

void KDNode::setX(int x) { m_x = x;}
void KDNode::setY(int y) { m_y = y;}
void KDNode::set(int x, int y) { m_x = x; m_y = y;}

bool KDNode::getSplit() {return m_split;}
void KDNode::setSplit(bool split) { m_split = split;}


bool KDNode::isLeaf() {return ((m_leftChild == NULL) && (m_rightChild == NULL));}



KDNode* KDNode::getLeftChild() {return m_leftChild;}
KDNode* KDNode::getRightChild() {return m_rightChild;}

void KDNode::setLeftChild(KDNode* left) { m_leftChild = left;}
void KDNode::setRightChild(KDNode* right) {m_rightChild = right;}


/******
1. 当我想设定一个成员函数为 inline的时候，inline应该放在哪个地方。
2. 如何判断叶子节点：
	这里有两种方式，第一种是设定一个标志位，表示它是否是叶子节点
	第二种：若左右子树同时为空，那么它也是叶子节点。

如果采用标志位的方法，那么会出现一致性的问题：如果你把一个标志位设置成了true，表示这是一个叶子节点，但是同时左右孩子不为空，应该怎么办?

这就是一致性问题，因为冗余才会引起一致性的问题。
在分布式的环境中，冗余是必须的，比如说每个node上都有一个 cache data。如何保证这些data是一致性的呢？


*******/















