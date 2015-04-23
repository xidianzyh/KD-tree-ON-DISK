#include "KDTree.h"

#include <cassert>
#include <limits>
#include <utility>
#include <cstddef>

KDTree::KDTree(vector<int>& vx, vector<int>& vy){

	assert(vx.size() == vy.size());
	if(vx.size() == 0) {  // There is no element in the vector.
		m_root = NULL;  // NULL defined in file <cstddef>
		return;
	}

	//Construct the KDTree recursively
	pair<int, int> range;
	int depth = 1;
	range.first = 0;
	range.second = vx.size() - 1;

	m_root = construct(vx, vy, range, depth);

}// end of member function KDTree(vector<int>& vx, vector<int>& vy) 


KDNode* KDTree::construct(vector<int>& vx, 
						vector<int>& vy,
						pair<int, int> range,
						int depth) {

	if(range.first == range.second) {// there is only one elements in the vector.
		//If depth is odd, the split is x, or the split is y.
		bool split = (depth % 2 == 1) ? true : false;
		KDNode* root = new KDNode(vx[range.first], vy[range.second], split);
		root->setLeftChild(NULL);
		root->setRightChild(NULL);
		return root;
	}
	
	// Get the index of the median number
	int median = getMedian(vx, vy, range, depth);
	bool split = (depth % 2 == 1) ? true : false;
	KDNode* root = new KDNode(vx[median], vy[median], split);
	
	// divide the vector into two parts.
	
	// recursivelyl invoke the construct() function
	pair<int, int> rangeLeft = std::make_pair(range.first, median);
	pair<int, int> rangeRight = std::make_pair(median + 1, range.second);
	KDNode* leftChild = construct(vx, vy, rangeLeft, depth + 1);
	KDNode* rightChild = construct(vx, vy, rangeRight, depth + 1);

	// set it's left and right child
	root->setLeftChild(leftChild);
	root->setRightChild(rightChild);
						
	return root;

}// end of member function construct()

int KDTree::getMedian(vector<int>& vx, vector<int>& vy, pair<int, int>range, int depth){
	// Get the median, and split the vector vx[range.first, range.second], vy[range.first, range.second]
	//into vx[range.first, i] and vx[i+1, range.second]
	//     vy[range.first, i] and vy[i+1, range.second]
	int kth = (range.second - range.first + 1 + 1) / 2;
	int idx = quickSelect(vx, vy, range, depth, kth);
	
	//TODO:
	vector<int>& v =vx;
	if(depth % 2 == 0) v = vy;
	
	//处理相等元素，使得和分裂值相等的元素位于一个节点上。
	int i = idx;
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

int KDTree::quickSelect(vector<int>& vx, vector<int>& vy,
					pair<int, int>range, 
					int depth, 
					int kth) {
	assert(kth >= 1 && kth <= (range.second - range.first + 1));
	
	int div = partition(vx,vy, range, depth);  //分割符的index; range.first <= div <= range.second
	int xth = (div - range.first + 1);			//分割符的 rank.

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
int KDTree::partition(vector<int>& vx, vector<int>& vy, 
					pair<int, int>range, 
					int depth) {
	
	int left, right, mid, div, idx;
	vector<int>& v1 = vx;
	vector<int>& v2 = vy;
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
	int tmp;
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




























