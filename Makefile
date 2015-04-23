CC := g++
CFLAG := -Wall -c

OBJECT := KDNode.o KDTree.o rect.o test_kdtree.o

all:$(OBJECT)
	$(CC) -Wall $(OBJECT) -o test_kdtree

KDNode.o: KDNode.cpp KDNode.h rect.h
	$(CC) $(CFLAG) $< -o $@

KDTree.o: KDTree.cpp KDNode.h rect.h
	$(CC) $(CFLAG) $< -o $@

rect.o: rect.cpp rect.h
	$(CC) $(CFLAG) $< -o $@

test_kdtree.o: test_kdtree.cpp KDTree.h
	$(CC) $(CFLAG) $< -o $@

clean:
	rm $(OBJECT) test_kdtree








