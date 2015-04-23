#ifndef INCLUDED_KDNODE_H
#define INCLUDED_KDNODE_H

/**
*KD-树的一个节点
**/
class KDNode {

private:
	int m_x;
	int m_y;
	bool m_split;  // m_split= ture分割轴为x轴，否则为y轴

	KDNode* m_leftChild;
	KDNode* m_rightChild;


public:
	
	//构造函数
	KDNode(int x, int y, bool split);
	
	int getX();
	int getY();
	void setX(int x);
	void setY(int y);
	void set(int x, int y);

	bool getSplit();
	void setSplit(bool split);
	bool isLeaf();  //判断是否为叶子节点
	// void setLeaf(bool leaf);
	
	KDNode* getLeftChild();
	KDNode* getRightChild();
	void setLeftChild(KDNode* left);
	void setRightChild(KDNode* right);

};



#endif

