#include <tbb/tbb.h>
#include "graph.hpp"
#include <stdlib.h> 
#include <iostream>

//http://paste.kde.org/pf0fabb5d/

void StarTest(){
    Graph graph;
    Domain domain{20, 20};
    graph.addNode(rand() % domain.w, rand() % domain.h);
    for(unsigned int i = 1; i < 20; ++i) {
        graph.addNode(rand() % domain.w, rand() % domain.h);
        graph.connectNodes(i,0);               
    }
    graph.align(domain, 10000);
    graph.print(domain);
}

void GridTest(){
    Graph graph;
    Domain domain{20, 20};
    unsigned int anzX = 5, anzY = 5;
    for(unsigned int i = 0; i < anzX * anzY; ++i) {
        graph.addNode(rand() % domain.w, rand() % domain.h);
    }
    for(unsigned int i = 0; i < anzX * anzY; ++i) {
        int x = i % anzX;
        int y = i / anzX;
        if(x > 0) {
            graph.connectNodes((x-1) + y*anzX, i);
        }
        if(y > 0) {
            graph.connectNodes(x + (y-1)*anzX, i);
        }
    }
    graph.align(domain, 10000);
    graph.print(domain);
}

 

int main(int argc, char *argv[]){
    tbb::task_scheduler_init init;
    StarTest();
    GridTest();
    return 0;
}