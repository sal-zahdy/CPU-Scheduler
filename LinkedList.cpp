#include "LinkedList.h"
#include <iostream>
using namespace std;

double universalTime   = 0;
double clockCycle      = 0.05;
int    quantumTime     = 0;

double totalTurnaround = 0;
double totalWaiting    = 0;
int    finishedCount   = 0;

LinkedList ganttLog;
LinkedList waitingPool;
Node*      currentGanttNode = nullptr;


LinkedList::LinkedList() : head(nullptr), next_id(1) {}

Node* LinkedList::createNode(double burst, int priority, double arrival) {
    Node* node             = new Node();
    node->id               = next_id++;
    node->burst            = burst;
    node->originalBurst    = burst;
    node->priority         = priority;
    node->arrival          = arrival;
    node->currentlyWorking = false;
    node->next             = nullptr;
    return node;
}

Node* LinkedList::createNode(double burst, int priority) {
    Node* node             = new Node();
    node->id               = next_id++;
    node->burst            = burst;
    node->originalBurst    = burst;
    node->priority         = priority;
    node->arrival          = 0;
    node->currentlyWorking = false;
    node->next             = nullptr;
    return node;
}

void LinkedList::insertAtTail(double burst, int priority, double arrival) {
    Node* node = createNode(burst, priority, arrival);
    if (!head) {
        head = node;
    } else {
        Node* cur = head;
        while (cur->next) cur = cur->next;
        cur->next = node;
    }
}

void LinkedList::insertAtTail(double burst, int priority) {
    Node* node = createNode(burst, priority);
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
        head       = head->next;
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
    if (!head) return nullptr;
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
        cout << "[ ID="       << cur->id
             << " | arrival=" << cur->arrival
             << " | burst="   << cur->burst
             << " | orig="    << cur->originalBurst
             << " | pri="     << cur->priority << " ]";
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
        while (curr && curr->next) {
            Node* next = curr->next;
            if (curr->burst > next->burst) {
                curr->next = next->next;
                next->next = curr;
                if (prev) prev->next = next; else head = next;
                prev    = next;
                swapped = true;
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    } while (swapped);
}

void LinkedList::sortByPriority() {
    if (!head || !head->next) return;
    bool swapped;
    do {
        swapped = false;
        Node *prev = nullptr, *curr = head;
        while (curr && curr->next) {
            Node* next = curr->next;
            if (curr->priority > next->priority) {
                curr->next = next->next;
                next->next = curr;
                if (prev) prev->next = next; else head = next;
                prev    = next;
                swapped = true;
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    } while (swapped);
}

LinkedList::~LinkedList() { clear(); }


void checkArrivals(LinkedList* readyQueue) {
    if (!waitingPool.head) return;

    Node* prev = nullptr;
    Node* cur  = waitingPool.head;

    while (cur) {
        if (cur->arrival <= universalTime) {
            Node* arrived = cur;
            if (prev) prev->next       = cur->next;
            else      waitingPool.head = cur->next;
            cur           = cur->next;
            arrived->next = nullptr;

            Node* node             = new Node();
            node->id               = arrived->id;
            node->burst            = arrived->burst;
            node->originalBurst    = arrived->originalBurst;
            node->priority         = arrived->priority;
            node->arrival          = arrived->arrival;
            node->currentlyWorking = false;
            node->next             = nullptr;
            delete arrived;

            if (!readyQueue->head) {
                readyQueue->head = node;
            } else {
                Node* tail = readyQueue->head;
                while (tail->next) tail = tail->next;
                tail->next = node;
            }
            if (node->id >= readyQueue->next_id)
                readyQueue->next_id = node->id + 1;

            cout << "[ARRIVED] P" << node->id
                 << " arrival=" << node->arrival
                 << " burst="   << node->burst
                 << " pri="     << node->priority << endl;
        } else {
            prev = cur;
            cur  = cur->next;
        }
    }
}


void logToGantt(int runningID) {
    if (!currentGanttNode || currentGanttNode->id != runningID) {
        ganttLog.insertAtTail(0, 0, universalTime);
        currentGanttNode                = ganttLog.getLastNode();
        currentGanttNode->id            = runningID;
        currentGanttNode->arrival       = universalTime;
        currentGanttNode->burst         = 0;
        currentGanttNode->originalBurst = 0;
    }
    currentGanttNode->burst += clockCycle;
}

void logFinished(Node* node) {
    double completionTime = universalTime + clockCycle;
    double turnaround     = completionTime - node->arrival;
    double waiting        = turnaround - node->originalBurst;
    totalTurnaround      += turnaround;
    totalWaiting         += waiting;
    finishedCount++;

    cout << "[FINISHED] P" << node->id
         << " | completion=" << completionTime
         << " | arrival="    << node->arrival
         << " | origBurst="  << node->originalBurst
         << " | TAT="        << turnaround
         << " | WT="         << waiting << endl;
}


void SJF(LinkedList* list, bool x) {
    if (!list->head) return;
    if (!x) {
        list->sortByTime();
    } else {
        if (!list->head->currentlyWorking) {
            list->sortByTime();
            list->head->currentlyWorking = true;
        }
    }
    logToGantt(list->head->id);
    list->head->burst -= clockCycle;
    if (list->head->burst <= clockCycle * 0.5) {
        logFinished(list->head);
        list->deleteByID(list->head->id);
    }
}

void roundRobin(LinkedList* list, int qTime) {
    if (!list->head) return;

    if (quantumTime <= 0)
        quantumTime = (int)(qTime / clockCycle);

    logToGantt(list->head->id);
    list->head->burst -= clockCycle;
    quantumTime--;

    bool burstDone   = list->head->burst <= clockCycle * 0.5;
    bool quantumDone = quantumTime <= 0;

    if (burstDone) {
        logFinished(list->head);
        list->deleteByID(list->head->id);
        quantumTime = (int)(qTime / clockCycle);
    } else if (quantumDone) {
        Node* oldHead = list->head;
        list->head    = list->head->next;
        oldHead->next = nullptr;
        if (list->head) {
            Node* tail = list->head;
            while (tail->next) tail = tail->next;
            tail->next = oldHead;
        } else {
            list->head = oldHead;
        }
        quantumTime = (int)(qTime / clockCycle);
    }
}

void Priority(LinkedList* list, bool x) {
    if (!list->head) return;
    if (!x) {
        list->sortByPriority();
        if (list->head) list->head->currentlyWorking = true;
    } else {
        if (!list->head->currentlyWorking) {
            list->sortByPriority();
            list->head->currentlyWorking = true;
        }
    }
    logToGantt(list->head->id);
    list->head->burst -= clockCycle;
    if (list->head->burst <= clockCycle * 0.5) {
        logFinished(list->head);
        list->deleteByID(list->head->id);
        if (list->head) list->head->currentlyWorking = false;
    }
}

void FCFS(LinkedList* list) {
    if (!list->head) return;
    if (!list->head->currentlyWorking)
        list->head->currentlyWorking = true;
    logToGantt(list->head->id);
    list->head->burst -= clockCycle;
    if (list->head->burst <= clockCycle * 0.5) {
        logFinished(list->head);
        int finishedID = list->head->id;
        list->deleteByID(finishedID);
        if (list->head) list->head->currentlyWorking = false;
    }
}


void step(LinkedList* list, int x) {
    checkArrivals(list);
    if (x == 0) FCFS(list);
    if (x == 1) SJF(list, 1);
    if (x == 2) SJF(list, 0);
    if (x == 3) Priority(list, 1);
    if (x == 4) Priority(list, 0);
    universalTime += clockCycle;
}

void step(LinkedList* list, int x, int qTime) {
    checkArrivals(list);
    if (x == 5) roundRobin(list, qTime);
    universalTime += clockCycle;
}