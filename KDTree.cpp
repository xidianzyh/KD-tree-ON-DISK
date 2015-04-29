#include "KDTree.h"

#include <cassert>
#include <limits>
#include <utility>
#include <cstddef>
#include <queue>
#include <algorithm>	//max
#include <cstdio>

using namespace std;

int32_t KDTree::m_B = -1;
int32_t KDTree::m_xBitWidth	= -1;
int32_t KDTree::m_yBitWidth = -1;
int32_t KDTree::m_splitValueBitWidth = -1;
int32_t KDTree::m_numPoint = -1;

void KDTree::Set(int32_t B, int32_t xBitWidth, int32_t yBitWidth){
	m_B = B;
	m_xBitWidth = xBitWidth;
	m_yBitWidth = yBitWidth;
	m_splitValueBitWidth = std::max(m_xBitWidth, m_yBitWidth);

	//调用KDPointDisk的静态函数，进行全局设置。
	KDPointDisk::Set(B, xBitWidth, yBitWidth);
	KDDisk::Set(B, m_splitValueBitWidth);
	//一个disk page中最多可以存储的点的个数。
	m_numPoint = KDPointDisk::GetMaxPoint();  
}

//析构函数
KDTree::~KDTree(){
	//递归调用删除整颗树在内存中的节点。
	removeTree(m_root);
}

void KDTree::removeTree(KDNode* root){
	if(root == NULL)
		return;
	if(root->getLeftChild() != NULL)
		removeTree(root->getLeftChild());
	if(root->getRightChild() != NULL)
		removeTree(root->getRightChild());
	
	delete root;
}

KDTree::KDTree(vector<uint64_t>& vx, vector<uint64_t>& vy){
	assert(vx.size() == vy.size());
	if(0 == vx.size()) {  // There is no element in the vector.
		m_root = NULL;  // NULL defined in file <cstddef>
		return;
	}

	//寻找vx，vy中的最大值和最小值，来构造整个KDTree的Rect.
	int64_t n = vx.size();
	uint64_t xmin, xmax, ymin, ymax;
	xmin = xmax = vx[0];
	ymin = ymax = vy[0];
	for(int64_t i = 0; i < n; i++){
		if(vx[i] < xmin)
			xmin = vx[i];
		if(vx[i] > xmax)
			xmax = vx[i];
		if(vy[i] < ymin)
			ymin = vy[i];
		if(vy[i] > ymax)
			ymax = vy[i];
	}
	
	Rect r(xmin, xmax, ymin, ymax);
	m_rect = r;
	//Construct the KDTree recursively
	int32_t depth = 1;
	pair<uint64_t, uint64_t> range;
	range.first = 0;
	range.second = vx.size() - 1;
	
	m_root = construct(vx, vy, range, depth, r);

}// end of member function KDTree(vector<int>& vx, vector<int>& vy) 


KDNode* KDTree::construct(vector<uint64_t>& vx, 
						vector<uint64_t>& vy,
						pair<uint64_t, uint64_t> range,
						int32_t depth,
						Rect r) {
	
	//如果点的个数可以存放进一个磁盘块中
	if(range.second - range.first + 1 <= this->m_numPoint) {
		//创建一个存放点的block.
		KDNode* root = new KDNode(vx, vy, range, depth);
		return root;
	}
	
	// Get the index of the median number
	uint64_t median = getMedian(vx, vy, range, depth);
	KDNode* root = new KDNode(vx[median], vy[median], depth, r);
	
	// getMedian will  divide the vector into two parts.
	
	// recursivelyl invoke the construct() function
	pair<uint64_t, uint64_t> rangeLeft = std::make_pair(range.first, median);
	pair<uint64_t, uint64_t> rangeRight = std::make_pair(median + 1, range.second);
	
	Rect rLeft 	= r;
	Rect rRight	= r;
	
	if(depth % 2 == 1) {
		rLeft.m_hx = median;
		rLeft.b_hx = true;
		rRight.m_lx = median;
		rRight.b_lx = false;
	} else {
		rLeft.m_hy = median;
		rLeft.b_hy = true;
		rRight.m_ly = median;
		rRight.b_ly = false;
	}

	KDNode* leftChild = construct(vx, vy, rangeLeft, depth + 1, rLeft);
	KDNode* rightChild = construct(vx, vy, rangeRight, depth + 1, rRight);

	// set it's left and right child
	root->setLeftChild(leftChild);
	root->setRightChild(rightChild);
						
	return root;

}// end of member function construct()

uint64_t KDTree::getMedian(vector<uint64_t>& vx, 
						vector<uint64_t>& vy, 
						pair<uint64_t, uint64_t>range, 
						int32_t depth){
	// Get the median, and split the vector vx[range.first, range.second], vy[range.first, range.second]
	//into vx[range.first, i] and vx[i+1, range.second]
	//     vy[range.first, i] and vy[i+1, range.second]
	uint64_t kth = (range.second - range.first + 1 + 1) / 2;
	uint64_t idx = quickSelect(vx, vy, range, depth, kth);
	
	//TODO:
	vector<uint64_t>& v =vx;
	if(depth % 2 == 0) v = vy;
	
	//处理相等元素，使得和分裂值相等的元素位于一个节点上。
	uint64_t i = idx;
	++i;
	while(i <= range.second && v[i] == v[idx]) ++i;
	return --i;
}

/****
* Get the kth value of v[range.first, range.second];  1 <= k and k <= range.second - range.first + 1
* @return idx,  返回下标[range.first, range.second];
* v[range.fist idx-1] <= v[idx]
* v[idx] <= v[idx + 1, v.range.second]
*
* 注意，仅仅通过 quickSelect() , 不能保证 v[idx+1, v.range.second]的值是完全大于v.[idx]的。这种
* 情况在KD-tree中不允许：在这个节点分裂的时候，和分裂值相等的元素只能位于这个节点的一个孩子中，不能
* 同时位于两个孩子中。
******/

uint64_t KDTree::quickSelect(vector<uint64_t>& vx, 
							vector<uint64_t>& vy,
							pair<uint64_t, uint64_t>range, 
							int32_t depth, 
							uint64_t kth) {

	assert(kth >= 1 && kth <= (range.second - range.first + 1));
	
	uint64_t div = partition(vx,vy, range, depth);  //分割符的index; range.first <= div <= range.second
	uint64_t xth = (div - range.first + 1);			//分割符的 rank.

	if(xth < kth) {
		kth -= xth;					// update the value of kth
		range.first = div + 1;		// update the range.first
		return quickSelect(vx, vy, range, depth, kth);
	}

	else if(xth > kth) {
		range.second = div -1;		// update the range.second
		return quickSelect(vx, vy, range, depth, kth);
	}

	else {
		return div;
	}
					
}// end of KDTree::quickSelect(vector<int>&,vector<int>&, pair<int, int>, depth, kth)


/**
* 类似于 快排的一次划分的过程。
* @return index of the partition value.  range.first <= idx <= range.second
* v[range.first, idx-1] <= v[idx]
* v[idx] < v[range.idx + 1, v.second]
**/
uint64_t KDTree::partition(vector<uint64_t>& vx, 
						vector<uint64_t>& vy, 
						pair<uint64_t, uint64_t>range, 
						int32_t depth) {
	
	uint64_t left, right, mid, div, idx;
	vector<uint64_t>& v1 = vx;
	vector<uint64_t>& v2 = vy;
	if(depth % 2 == 0) {
		v1 = vy;
		v2 = vx;
	}
	
	left = v1[range.first];
	right = v1[range.second];
	mid = v1[(range.first + range.second) / 2];

	//三分取中方法
	if(left <= mid){
		if	(mid <= right){
			div = mid;  //the value
			idx = (range.first + range.second) / 2; // the index
		}
		else if	(left <= right ) {
			div = right;
			idx = range.second;
		} else {
			div = left;
			idx = range.first;
		}
	}
	else {
	
		if(left <= right) {
			div = left;
			idx = range.first;
		} 
		else if(mid <= right) {
			div = right;
			idx = range.second;
		} else {
			div = mid;
			idx = (range.first + range.second) / 2;
		}
	}
	
	left = mid = right = range.first;
	//对数组进行一次调整
	uint64_t tmp;
	/***
	* 一次完整的划分过程，确保不会出错。
	* left -----> mid ----> right
	* left 指向下一个 等于或者大于 div的元素
	* mid  指向下一个 大于 div的元素
	* right 指向下一个未测试的元素。
	***/
	while(right <= range.second) {
		
		if(v1[right] > div) {
			right ++;
			if(v1[left] < div) {
				left ++;
			}
			if(v1[mid] <= div) {
				mid++;
			}
		}
		else if(v1[right] == div) {
			tmp = v1[mid];
			v1[mid] = v1[right];
			v1[right] = tmp;
			
			tmp = v2[mid];
			v2[mid] = v2[right];
			v2[right] = tmp;

			right++;
			mid++;
		}

		else if (v1[right] < div) {
			if (left < mid && mid < right) {
				tmp = v1[left];
				v1[left] = v1[right];
				v1[right] = v1[mid];
				v1[mid] = tmp;

				tmp = v2[left];
				v2[left] = v2[right];
				v2[right]=v2[mid];
				v2[mid] = tmp;
			}
			else if(left <= mid && mid <= right) {
				tmp = v1[left];
				v1[left] = v1[right];
				v1[right] = tmp;

				tmp = v2[left];
				v2[left] = v2[right];
				v2[right] = tmp;
			}

			left ++;
			mid ++;
			right ++;
		}

	}// end of while()

	return mid - 1;

} // end of member function KDTree::partition()


KDNode* KDTree::getRoot(){
	return m_root;
}

//判断点(x, y)是否在矩形 (px.first, px.second)x(py.first, py.second)
bool KDTree::isInRange(pair<int ,int>& px, pair<int, int>& py, int x, int y) {

	if((x >= px.first && x <= px.second) && (y >= py.first && y <= py.second))
		return true;

	return false;
}


// Range Query, 找到出现在矩形 (px.first, px.second)x(py.first, py.second)
// 中的所有的点
vector<pair<int, int> >* KDTree::locate(pair<int, int>& px, pair<int, int>& py) {

	vector<pair<int ,int> >* result = new vector<pair<int, int> >();
	
	int lx, hx, ly, hy;
	lx = ly = std::numeric_limits<int>::min();
	hx = hy = std::numeric_limits<int>::max();
	Rect node_rect(lx, hx, ly, hy, true, true, true, true); //根节点对应的矩形
	Rect query_rect(px.first, px.second, py.first, py.second, true, true, true, true); //查询矩形

	locate(query_rect, m_root, node_rect, 1, result);  //KD-树的查询

	return result;   //返回找到的所有的点
}


void KDTree::locate(Rect& qrect, 
				KDNode* root,
				Rect nrect,
				int depth, 
				vector<pair<int, int> >* result) {
	
	int x = root->getX();
	int y = root->getY();

	int& val = x;

	if(depth % 2 == 0) {
		val = y;
	}
	
	if(root->isLeaf()){
		if (qrect.isInRange(x, y)) {
			// Add the leaf into the result
			result->push_back(std::make_pair(x, y));
		}
		return;
	}
	
	Rect lrect = nrect;  //左孩子节点对应的矩形
	Rect rrect = nrect;	 //右孩子节点对应的矩形

	// 设定左右孩子节点对应的矩形。
	if(depth % 2 == 1) {
		//split on x
		assert(val >= nrect.getLowX() && val <= nrect.getHighX());
		lrect.setHighX(val, true);  //左孩子包含边界，也就是说在分割数据的时候和val相等的数据分配到了左孩子。
		rrect.setLowX(val, false);
	} else {
		//split on y
		assert(val >= nrect.getLowY() && val <= nrect.getHighY());
		lrect.setHighY(val, true); //左孩子包含边界。
		rrect.setLowY(val, false);
	}

	/**递归查询孩子节点**/

	if (root->getLeftChild() != NULL) {
		//如果查询矩形qrect完全包含左孩子对应的矩形lrect
		if(qrect.isContained(lrect)){
			//递归查询其中所有的子节点
			locateAllChild(root->getLeftChild(), result);
		}

		//如果矩形qrect和lrect相交
		if (qrect.isIntersected(lrect)) {
			locate(qrect, root->getLeftChild(), lrect, depth + 1, result);
		}
	}

	if (root->getRightChild() != NULL) {
		//if qrect fully contained rrect.
		if(qrect.isContained(rrect)){
			//递归查询所有的孩子节点
			locateAllChild(root->getRightChild(), result);
		}

		//if qrect intersects rrect.
		if (qrect.isIntersected(rrect)) {
			locate(qrect, root->getRightChild(), rrect, depth + 1, result);
		}
	}

}// end of KDTree::locate(pair<int ,int>&, pair<int, int>&, KDTree* root, int depth, *result )


void KDTree::locateAllChild(KDNode* root, vector<pair<int, int> >* result){
	if(root == NULL)
		return;

	if(root->isLeaf()){
		//收集叶子节点
		int x = root->getX();
		int y = root->getY();
		result->push_back(std::make_pair(x, y));
		return;
	}

	//递归查询孩子节点
	locateAllChild(root->getLeftChild(), result);
	locateAllChild(root->getRightChild(), result);
}


/**将一颗KDTree存储在磁盘上 **
*
*@param name :   The file to save.
*@param B    :   The size of a disk page in bytes.
*
*方法如下:
*1. 构造KDB-tree的时候，如果切分之后的点的个数若是小于B个，就停止划分。
*2. 按照BFS（Breadth First Search）深度优先的方式访问KDTree。
*3. 当一个磁盘中可以装入的节点的个数已经满，那么就停止装入节点。
*4. 对它的每个孩子节点依次如此访问。
*
* An internal node consist of:
* a) The Shape of the kdtree.
* b) the data in the kdtree node.
* c) the child pointer of the internal node.
******************************/
void KDTree::SaveToDisk(char* name){
	FILE* out = fopen(name, "w");
	if(!out){
		fprintf(stderr, "Open file '%s' error!\n", name);
		exit(1);
	}
	DiskFile df(out, 0);
	m_header.writeToDisk(&df);
	int32_t rootNum = SaveNodeToDisk(m_root, &df);
	int32_t allNum = df.m_diskNum;
	m_header.set(rootNum, allNum);
	m_header.writeToDisk(&df);

	fclose(out);
}

/*****将一个节点为根的树保存到磁盘上
*@param root	数的根
*@param out     file descripter
*@return 		The disk number into which it saves.
************************************/
int32_t KDTree::SaveNodeToDisk(KDNode* root, DiskFile* diskOut) {
	
	if(NULL == root){
		assert(false);
	}

	if(root->isPointNode()) {
		/**节点保存了点：可以存放在一个磁盘块中**/
		/*将这个node中的point保存到一个磁盘块中，并且返回磁盘号*/	
		int64_t nPoint = root->getPointNum();	// the number of points in a node.
		KDPointDisk pDisk(nPoint);	
		for(int64_t i = 0; i < nPoint; i++){
			pair<uint64_t, uint64_t> p = root->getPoint(i);
			pDisk.setPoint(i, p.first, p.second);
		}
		// Save the KDPointDisk into real disk page.
		int32_t numDisk = pDisk.writeToDisk(diskOut);
		return numDisk;
	}
	
	/**从这个节点开始层次遍历**/
	queue<KDNode*> qChild;
	queue<KDNode*> qParent;
	queue<KDNode*> qPNode;

	qChild.push(root);

	while(!qChild.empty()){

		KDNode* tmpNode = qChild.front();
		qChild.pop();
		if(tmpNode->getLeftChild() != NULL){
			if(tmpNode->getLeftChild()->isPointNode())
				qPNode.push(tmpNode->getLeftChild());
			else
				qChild.push(tmpNode->getLeftChild());
		}	
		if(tmpNode->getRightChild() != NULL){
			if(tmpNode->getRightChild()->isPointNode())
				qPNode.push(tmpNode->getRightChild());
			else
				qChild.push(tmpNode->getRightChild());
		}
		//将这个节点加入这个磁盘页中，并且判断磁盘页的空间。
		qParent.push(tmpNode);
		
		//计算加入下一个节点(the next node)之后，所有节点占用的空间
		int32_t nc = qChild.size() + qPNode.size() + 1; /*弹出一个节点，最多加入两个孩子节点*/
		int32_t np = qParent.size() + 1;

		if(KDDisk::OverFlow(np, nc))
			break;
	}
	
	while(!qChild.empty()){
		qPNode.push(qChild.front());
		qChild.pop();
	}
	while(!qPNode.empty()){
		qChild.push(qPNode.front());
		qPNode.pop();
	}

	// qParent 保存了层次遍历的所有需要保存的父节点。
	// qChild  保存了层次遍历所需要的孩子节点。
	int32_t nc = qChild.size();  //孩子节点.
	int32_t np = qParent.size(); //父亲节点.
	
	int32_t idx = 0;
	KDDisk disk(root->getRect(), root->getDepth());
	disk.init(np, nc);

	while(!qParent.empty()){
		KDNode* tmpNode = qParent.front();  
		qParent.pop();
		int32_t shapeNode = 0;
		if(tmpNode->getLeftChild() != NULL){
			if(tmpNode->getRightChild() != NULL){
				shapeNode = 0x3;  //11, two child
			} else {
				shapeNode = 0x1;  //01, only left child
			}
		} else {
			if(tmpNode->getRightChild() != NULL)
				shapeNode = 0x2;  //10; only right child
			else
				shapeNode = 0x0;  //00, zero child
		}
	//TODO: 向disk中写入树形的值。
		disk.writeTreeShape(idx, shapeNode);

	//TODO: 向disk中写入节点的value的值。
		disk.writeSplitValue(idx, tmpNode->getSplitValue());

		idx++;
	}//end of while()

	idx = 0;
	/**确定disk中孩子节点的值**/
	while(!qChild.empty()){
		KDNode* tmpNode = qChild.front();
		qChild.pop();
		int32_t numDisk;
		numDisk = SaveNodeToDisk(tmpNode, diskOut);
		//TODO: 将孩子节点的 numDisk 写入disk中。
		disk.writeChildPointer(idx, numDisk);
		idx++;
	}
	
	/**TODO: 向disk中写入header，这里需要取得磁盘号的值**/
	

	/**TODO: 将disk 写入真正的磁盘中**/
	int32_t numDisk = disk.writeToDisk(diskOut);
	
	/**TODO: 返回写入的磁盘号**/
	return numDisk;
}























