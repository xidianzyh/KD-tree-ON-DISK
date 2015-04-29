#include <iostream>
#include <vector>
#include <algorithm> /*sort, nth_element*/
/* clock_t, clock, CLOCKS_PER_SEC*/
#include <ctime>  // measure time.


#include "KDNode.h"
#include "KDTree.h"


using namespace std;

void Set(int32_t B, int32_t xBitWidth, int32_t yBitWidth) {
	KDTree::Set(B, xBitWidth, yBitWidth);
}

void test();
void test_quickSort();
void test_nth();

int main() {
	
	//test_nth();
	//test_quickSort();
	test();
}

void test() {

	Set(1024*32, 32, 32);

	clock_t build_time = 0, locate_time = 0;

	vector<uint64_t> vx, vy;

	for(int i = 1; i< 25*1024*1024; i++){
		vx.push_back(i);
		vy.push_back(i);
	}

	build_time = std::clock();  // measure time
	KDTree tree(vx, vy);
	tree.SaveToDisk("tmp.kdt");
	build_time = std::clock() - build_time;

	pair<int, int> px = make_pair(0, 6200);
	pair<int, int> py = make_pair(0, 6000);

	/**为什么测试locate的时间很少? 经常就是0，但是结果是正确的**/
	// locate_time = clock();
	// vector<pair<int, int> >* result = tree.locate(px, py);
	// locate_time = clock() - locate_time;

	// for(vector<pair<int, int> >::iterator it = result->begin(); it != result->end(); it++){
	// 	//cout << "(" << (it->first) << ", " << (it->second) << ")" << endl;
	// }

	cout << "build_time = " << ((double)build_time) / CLOCKS_PER_SEC << endl;
	cout << "locate_time = " << ((double)locate_time) / CLOCKS_PER_SEC << ", clocks = " 
			<< locate_time << endl;
	cout << endl;
}




/**测试100万条数据的运行时间 **
*
* 100万条数据，随机排序，运行时间为0.5s。（大大超出了预期）
* 
*****/
void test_quickSort(){
	vector<int> v;
	clock_t time;

	for(int i = 0; i < 1000000; i++) {
		v.push_back(i+1);
	}

	std::random_shuffle(v.begin(), v.end());
	
	time = std::clock();
	std::sort(v.begin(), v.end());
	time = std::clock() - time;

	cout << ((double)time) / CLOCKS_PER_SEC  << endl;
}


/****测试C++取中位数的函数******
*
* 一次测试时间是0.04s，惊呆我了。
* 看来，如果我的kd-tree 改进的话，是不是有很大的改进空间。
* 构造KD-tree 的主要过程就是选取中位数，我的算法太慢了。
********************************/
void test_nth(){
	vector<int> v;
	clock_t time;
	int k;
	
	long n = 1000000;
	for(int i = 1; i <= n; i++)
		v.push_back(i);
	std::random_shuffle(v.begin(), v.end());
	k = (1+n)/2;

	time = std::clock();
	std::nth_element(v.begin(), v.begin() + k, v.end());
	time = std::clock()-time;
	cout <<"nth_element() time = " << ((double)time) / CLOCKS_PER_SEC << endl;
}







