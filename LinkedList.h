#ifndef LINKEDLIST_H
#define LINKEDLIST_H



struct Node {
    int  id;
    double  arrival;
    double  burst;
    int  priority;
    bool currentlyWorking = 0;
    Node* next;
};

struct LinkedList {
    Node* head;
    int   next_id;

    LinkedList();
    Node* createNode(double burst, int priority);
    void  insertAtTail(double burst, int priority,int arrival);
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
    Node* createNode(double burst, int priority , int arrival );


};

extern LinkedList ganttLog;
extern Node*      currentGanttNode;

void logToGantt(int runningID);
void SJF(LinkedList* list, bool x);
void roundRobin(LinkedList* list, int qTime);
void Priority(LinkedList* list, bool x);
void FCFS(LinkedList* list);
void step(LinkedList* list, int x);
void step(LinkedList* list , int x , int quantumTime);


#endif