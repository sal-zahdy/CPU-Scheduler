#include "LinkedList.h"

#include <iostream>
using namespace std;

double universalTime = 0;
double clockCycle    = 0.05;
double quantumTime   = 0.0;

LinkedList ganttLog;
Node*      currentGanttNode = nullptr;


// ── LinkedList methods ──────────────────────────────

LinkedList::LinkedList() : head(nullptr), next_id(1) {}

Node* LinkedList::createNode(double burst, int priority , int arrival ) {
    Node* node       = new Node();
    node->id         = next_id++;
    node->burst      = burst;
    node->priority   = priority;
    node->next       = nullptr;
    node->arrival    = arrival;
    return node;
}

Node* LinkedList::createNode(double burst, int priority) {
    Node* node     = new Node();
    node->id       = next_id++;
    node->burst    = burst;
    node->priority = priority;
    node->next     = nullptr;
    return node;
}

void LinkedList::insertAtTail(double burst, int priority,int arrival) {
    Node* node = createNode(burst, priority , arrival );
    if (!head) {
        head = node;
    } else {
        Node* cur = head;
        while (cur->next) cur = cur->next;
        cur->next = node;
    }
}

void LinkedList::insertAfterID(int target_id, double burst, int priority) {
    Node* cur = head;
    while (cur && cur->id != target_id) cur = cur->next;
    if (!cur) return;
    Node* node = createNode(burst, priority);
    node->next = cur->next;
    cur->next  = node;
}

void LinkedList::deleteByID(int target_id) {
    if (!head) return;
    if (head->id == target_id) {
        Node* temp = head;
        head = head->next;
        delete temp;
        return;
    }
    Node* prev = head;
    Node* cur  = head->next;
    while (cur && cur->id != target_id) { prev = cur; cur = cur->next; }
    if (!cur) return;
    prev->next = cur->next;
    delete cur;
}

Node* LinkedList::searchByID(int target_id) const {
    Node* cur = head;
    while (cur) { if (cur->id == target_id) return cur; cur = cur->next; }
    return nullptr;
}

Node* LinkedList::getLastNode() {
    Node* ptr = head;
    while (ptr->next) ptr = ptr->next;
    return ptr;
}

void LinkedList::updateTimeByID(int target_id, double burst) {
    Node* node = searchByID(target_id);
    if (!node) return;
    node->burst = burst;
}

void LinkedList::updateByID(int target_id, double burst, int priority) {
    Node* node = searchByID(target_id);
    if (!node) return;
    node->burst    = burst;
    node->priority = priority;
}

int LinkedList::size() const {
    int count = 0;
    Node* cur = head;
    while (cur) { ++count; cur = cur->next; }
    return count;
}

bool LinkedList::isEmpty() const { return head == nullptr; }

void LinkedList::clear() {
    Node* cur = head;
    while (cur) { Node* temp = cur; cur = cur->next; delete temp; }
    head    = nullptr;
    next_id = 1;
}

void LinkedList::print() const {
    if (!head) { cout << "[LIST] (empty)\n"; return; }
    Node* cur = head;
    while (cur) {
        cout << "[ ID=" << cur->id << " | burst=" << cur->burst
             << " | priority=" << cur->priority << " ]";
        if (cur->next) cout << " -> ";
        cur = cur->next;
    }
    cout << " -> NULL\n";
}

void LinkedList::sortByTime() {
    if (!head || !head->next) return;
    bool swapped;
    do {
        swapped = false;
        Node *prev = nullptr, *curr = head;
        while (curr->next) {
            Node* next = curr->next;
            if (curr->burst > next->burst) {
                curr->next = next->next;
                next->next = curr;
                if (prev) prev->next = next; else head = next;
                prev = next; swapped = true;
            } else { prev = curr; curr = curr->next; }
        }
    } while (swapped);
}

void LinkedList::sortByPriority() {
    if (!head || !head->next) return;
    bool swapped;
    do {
        swapped = false;
        Node *prev = nullptr, *curr = head;
        while (curr->next) {
            Node* next = curr->next;
            if (curr->priority > next->priority) {
                curr->next = next->next;
                next->next = curr;
                if (prev) prev->next = next; else head = next;
                prev = next; swapped = true;
            } else { prev = curr; curr = curr->next; }
        }
    } while (swapped);
}

LinkedList::~LinkedList() { clear(); }

// ── Gantt + Algorithms ──────────────────────────────

void logToGantt(int runningID) {
    if (!currentGanttNode || currentGanttNode->id != runningID) {
        ganttLog.insertAtTail(0, 0,0);
        currentGanttNode          = ganttLog.getLastNode();
        currentGanttNode->id      = runningID;
        currentGanttNode->arrival = universalTime;
        currentGanttNode->burst   = 0;
    }
    currentGanttNode->burst += clockCycle;
}

void SJF(LinkedList* list, bool x) {
    if (!list->head) return;
    if (!x) {
        list->sortByTime();
    } else {
        if (!list->head->currentlyWorking) {
            list->sortByTime();
            list->head->currentlyWorking = 1;
        }
    }
    logToGantt(list->head->id);
    list->head->burst -= clockCycle;
    if (list->head->burst <= clockCycle * 0.5) list->deleteByID(list->head->id);
}

// linkedlist.cpp — replace your roundRobin with this:
void roundRobin(LinkedList* list, double qTime) {
    if (!list->head) return;

    logToGantt(list->head->id);
    list->head->burst -= clockCycle;
    quantumTime       -= 1.0;   // decrement by 1 tick, not by clockCycle

    bool burstDone   = list->head->burst  <= clockCycle * 0.5;
    bool quantumDone = quantumTime        <= 0.5;

    if (burstDone) {
        list->deleteByID(list->head->id);
        quantumTime = qTime;
    } else if (quantumDone) {
        Node* oldHead   = list->head;        // ✅ save BEFORE moving
        list->head      = list->head->next;
        oldHead->next   = nullptr;
        Node* tail      = list->head;
        while (tail->next) tail = tail->next;
        tail->next      = oldHead;
        quantumTime     = qTime;
    }
}

void Priority(LinkedList* list, bool x) {
    if (!list->head) return;
    if (!x) {
        list->sortByPriority();
    } else {
        if (!list->head->currentlyWorking) {
            list->sortByPriority();
            list->head->currentlyWorking = 1;
        }
    }
    logToGantt(list->head->id);
    list->head->burst -= clockCycle;
    if (list->head->burst <= clockCycle * 0.5) list->deleteByID(list->head->id);
}

void FCFS(LinkedList* list) {
    if (!list->head) return;
    if (!list->head->currentlyWorking)
        list->head->currentlyWorking = 1;
    logToGantt(list->head->id);
    list->head->burst -= clockCycle;
    if (list->head->burst <= clockCycle * 0.5) {
        int finishedID = list->head->id;
        list->deleteByID(finishedID);
        if (list->head) list->head->currentlyWorking = 0;
    }
}

void step(LinkedList* list , int x , double quantumTime){

    if(x == 5) roundRobin(list , quantumTime);
    universalTime+= clockCycle;

}

void step(LinkedList* list, int x)
{
    if (x == 0) FCFS(list);
    if (x == 1) SJF(list, 1);
    if (x == 2) SJF(list, 0);
    if (x == 3) Priority(list, 1);
    if (x == 4) Priority(list, 0);
    if (x == 5) roundRobin(list, quantumTime);

    universalTime += clockCycle;
}