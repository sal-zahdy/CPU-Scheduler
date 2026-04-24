#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
using namespace std;

extern double universalTime;
extern double clockCycle;
extern int    quantumTime;

extern double totalTurnaround;
extern double totalWaiting;
extern int    finishedCount;

struct Node {
    int    id;
    double arrival;
    double burst;
    double originalBurst;
    int    priority;
    bool   currentlyWorking = false;
    Node*  next;
};

struct LinkedList {
    Node* head;
    int   next_id;

    LinkedList();
    Node* createNode(double burst, int priority, double arrival);
    Node* createNode(double burst, int priority);
    void  insertAtTail(double burst, int priority, double arrival);
    void  insertAtTail(double burst, int priority);
    void  insertAfterID(int target_id, double burst, int priority);
    void  deleteByID(int target_id);
    Node* searchByID(int target_id) const;
    Node* getLastNode();
    void  updateTimeByID(int target_id, double burst);
    void  updateByID(int target_id, double burst, int priority);
    int   size() const;
    bool  isEmpty() const;
    void  clear();
    void  print() const;
    void  sortByTime();
    void  sortByPriority();
    ~LinkedList();
};

extern LinkedList ganttLog;
extern LinkedList waitingPool;
extern Node*      currentGanttNode;

void checkArrivals(LinkedList* readyQueue);
void logToGantt(int runningID);
void logFinished(Node* node);
void SJF(LinkedList* list, bool x);
void roundRobin(LinkedList* list, int qTime);
void Priority(LinkedList* list, bool x);
void FCFS(LinkedList* list);
void step(LinkedList* list, int x);
void step(LinkedList* list, int x, int qTime);

#endif