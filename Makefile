CC := g++ -g
CFLAG := -Wall -c -std=c++11

OBJECT := KDNode.o KDTree.o rect.o KDDisk.o KDPointDisk.o test_kdtree.o DiskFile.o KDTreeDiskHeader.o

all:$(OBJECT)
	$(CC) -Wall  -o test_kdtree -pthread $(OBJECT) libsdsl.a libdivsufsort64.a libdivsufsort.a -lgtest

KDNode.o: KDNode.cpp KDNode.h rect.h
	$(CC) $(CFLAG) -o $@ $<

KDTree.o: KDTree.cpp KDNode.h rect.h
	$(CC) $(CFLAG) -o $@ $< 

rect.o: rect.cpp rect.h
	$(CC) $(CFLAG) -o $@ $< 

KDDisk.o: KDDisk.cpp KDDisk.h rect.h DiskFile.h
	$(CC) $(CFLAG) -o $@ $<

KDPointDisk.o: KDPointDisk.cpp KDPointDisk.h rect.h DiskFile.h
	$(CC) $(CFLAG) -o $@ $<

DiskFile.o: DiskFile.cpp DiskFile.h
	$(CC) $(CFLAG) -o $@ $<

KDTreeDiskHeader.o: KDTreeDiskHeader.cpp KDTreeDiskHeader.h DiskFile.h
	$(CC) $(CFLAG) -o $@ $<

test_kdtree.o: test_kdtree.cpp KDTree.h
	$(CC) $(CFLAG) -o $@ $<



cleanObject:
	rm $(OBJECT) 

cleanAll:
	rm $(OBJECT) test_kdtree






