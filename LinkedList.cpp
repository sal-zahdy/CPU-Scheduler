#include <iostream>
using namespace std;

int universalTime=0;
int clockCycle =50;
LinkedList ganttLog;
Node* currentGanttNode = nullptr;  





// ─────────────────────────────────────────────
//  Node struct
// ─────────────────────────────────────────────
struct Node {
    int    id;             // auto-incremented, assigned on creation
    int arrival;
    int burst;
    int    priority;
    bool   currentlyWorking = 0;
    Node*  next;
};

// ─────────────────────────────────────────────
//  LinkedList struct
// ─────────────────────────────────────────────
struct LinkedList {
    Node* head;
    int   next_id;         // counter used to assign IDs

    // Constructor
    LinkedList() : head(nullptr), next_id(1) {}

    // ------------------------------------------------------------------
    // createNode – allocates a new node and assigns the next available ID
    // ------------------------------------------------------------------
    Node* createNode(double burst, int priority) {
        Node* node       = new Node();
        node->id         = next_id++;
        node->burst = burst;
        node->priority   = priority;
        node->next       = nullptr;
        return node;
    }


    // ------------------------------------------------------------------
    // insertAtTail – O(n)
    // ------------------------------------------------------------------
    void insertAtTail(double burst, int priority) {
        Node* node = createNode(burst, priority);
        if (!head) {
            head = node;
        } else {
            Node* cur = head;
            while (cur->next) cur = cur->next;
            cur->next = node;
        }
        std::cout << "[INSERT TAIL] Node ID=" << node->id << " added at tail.\n";
    }

    // ------------------------------------------------------------------
    // insertAfterID – inserts a new node right after the node with given id
    // ------------------------------------------------------------------
    void insertAfterID(int target_id, double burst, int priority) {
        Node* cur = head;
        while (cur && cur->id != target_id) cur = cur->next;
        if (!cur) {
            std::cout << "[INSERT AFTER] Node with ID=" << target_id << " not found.\n";
            return;
        }
        Node* node  = createNode(burst, priority);
        node->next  = cur->next;
        cur->next   = node;
        std::cout << "[INSERT AFTER] Node ID=" << node->id
                  << " inserted after ID=" << target_id << ".\n";
    }

    // ------------------------------------------------------------------
    // deleteByID – removes the first node whose id matches
    // ------------------------------------------------------------------
    void deleteByID(int target_id) {
        if (!head) {
            std::cout << "[DELETE] List is empty.\n";
            return;
        }
        // Target is the head
        if (head->id == target_id) {
            Node* temp = head;
            head = head->next;
            delete temp;
            std::cout << "[DELETE] Node ID=" << target_id << " deleted.\n";
            return;
        }
        Node* prev = head;
        Node* cur  = head->next;
        while (cur && cur->id != target_id) {
            prev = cur;
            cur  = cur->next;
        }
        if (!cur) {
            std::cout << "[DELETE] Node with ID=" << target_id << " not found.\n";
            return;
        }
        prev->next = cur->next;
        delete cur;
        std::cout << "[DELETE] Node ID=" << target_id << " deleted.\n";
    }

    // ------------------------------------------------------------------
    // search – returns a pointer to the node with the given id (or nullptr)
    // ------------------------------------------------------------------
    Node* searchByID(int target_id) const {
        Node* cur = head;
        while (cur) {
            if (cur->id == target_id) return cur;
            cur = cur->next;
        }
        return nullptr;
    }


    Node* getLastNode(){
        Node* ptr1 = head;
        while (ptr1->next)
        {
            ptr1=ptr1->next;
        }
    return ptr1;    
    }


    // ------------------------------------------------------------------
    // updateTimeByID – changes only the burst of a node by id
    // ------------------------------------------------------------------
    void updateTimeByID(int target_id, double burst) {
        Node* node = searchByID(target_id);
        if (!node) {
            std::cout << "[UPDATE TIME] Node with ID=" << target_id << " not found.\n";
            return;
        }
        node->burst = burst;
        std::cout << "[UPDATE TIME] Node ID=" << target_id
                  << " burst updated to " << burst << ".\n";
    }

    // ------------------------------------------------------------------
    // update – changes burst and/or priority of a node by id
    // ------------------------------------------------------------------
    void updateByID(int target_id, double burst, int priority) {
        Node* node = searchByID(target_id);
        if (!node) {
            std::cout << "[UPDATE] Node with ID=" << target_id << " not found.\n";
            return;
        }
        node->burst = burst;
        node->priority      = priority;
        std::cout << "[UPDATE] Node ID=" << target_id << " updated.\n";
    }

    // ------------------------------------------------------------------
    // size – returns number of nodes
    // ------------------------------------------------------------------
    int size() const {
        int count = 0;
        Node* cur = head;
        while (cur) { ++count; cur = cur->next; }
        return count;
    }

    // ------------------------------------------------------------------
    // isEmpty
    // ------------------------------------------------------------------
    bool isEmpty() const { return head == nullptr; }

    // ------------------------------------------------------------------
    // clear – deletes all nodes and resets the ID counter
    // ------------------------------------------------------------------
    void clear() {
        Node* cur = head;
        while (cur) {
            Node* temp = cur;
            cur = cur->next;
            delete temp;
        }
        head    = nullptr;
        next_id = 1;
        std::cout << "[CLEAR] All nodes deleted. ID counter reset.\n";
    }

    // ------------------------------------------------------------------
    // print – displays all nodes
    // ------------------------------------------------------------------
    void print() const {
        if (!head) {
            std::cout << "[LIST] (empty)\n";
            return;
        }
        std::cout << "[LIST] ";
        Node* cur = head;
        while (cur) {
            std::cout << "[ ID=" << cur->id
                      << " | time=" << cur->burst
                      << " | priority=" << cur->priority << " ]";
            if (cur->next) std::cout << " -> ";
            cur = cur->next;
        }
        std::cout << " -> NULL\n";
    }
    

    

    // ------------------------------------------------------------------
    // Sort the list from smallest time
    // ------------------------------------------------------------------
    void sortByTime() {
    if (!head || !head->next) return;

    bool swapped;
    do {
        swapped = false;
        Node *prev = nullptr, *curr = head;

        while (curr->next) {
            Node* next = curr->next;

            if (curr->burst > next->burst) {
                // Detach next from curr
                curr->next = next->next;
                next->next = curr;

                // Update prev or head
                if (prev) prev->next = next;
                else head = next;

                prev    = next;
                swapped = true;
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    } while (swapped);

    std::cout << "[SORT] List sorted by burst (ascending).\n";
}
    
    // ------------------------------------------------------------------
    // Sort the list from smallest Priority
    // ------------------------------------------------------------------
     void sortByPriority() {
        if (!head || !head->next) return;

        bool swapped;
        do {
            swapped = false;
            Node *prev = nullptr, *curr = head;

            while (curr->next) {
                Node* next = curr->next;

                if (curr->priority > next->priority) {
                    // Detach next from curr
                    curr->next = next->next;
                    next->next = curr;

                    // Update prev or head
                    if (prev) prev->next = next;
                    else head = next;

                    prev    = next;
                    swapped = true;
                } else {
                    prev = curr;
                    curr = curr->next;
                }
            }
        } while (swapped);

        std::cout << "[SORT] List sorted by priority (ascending).\n";
    }


    // Destructor – ensure no memory leaks
    
    ~LinkedList() { clear(); }

   

};


void logToGantt(int runningID) {
    if (!currentGanttNode || currentGanttNode->id != runningID) {
        ganttLog.insertAtTail(0, 0);
        currentGanttNode          = ganttLog.getLastNode();
        currentGanttNode->id      = runningID;
        currentGanttNode->arrival = universalTime;
        currentGanttNode->burst   = 0;
    }
    currentGanttNode->burst += clockCycle;
}
    // ------------------------------------------------------------------
    // Take step as SJF  takes 0 if premptive, takes 1 if nonpremptive 
    // ------------------------------------------------------------------


void SJF(LinkedList* list , bool x){
    
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
    if (list->head->burst <= 0)
        list->deleteByID(list->head->id);
}


int quantumTime;
void roundRobin(LinkedList* list , int qTime){

   if (!list->head) return;

    logToGantt(list->head->id);

    list->head->burst -= clockCycle;
    quantumTime       -= clockCycle;

    if (quantumTime <= 0) {
        if (list->head->burst <= 0) {
            list->deleteByID(list->head->id);
        } else {
            Node* ptr  = list->getLastNode();
            ptr->next  = list->head;
            list->head = list->head->next;
            ptr->next->next = nullptr;
        }
        quantumTime = qTime;
    }
}


void Priority(LinkedList* list, bool x)
{
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
    if (list->head->burst <= 0)
        list->deleteByID(list->head->id);
}

void FCFS(LinkedList* list)
{
       if (!list->head) return;

    if (!list->head->currentlyWorking) {
        list->head->currentlyWorking = 1;
    }

    logToGantt(list->head->id);

    list->head->burst -= clockCycle;
    if (list->head->burst <= 0) {
        int finishedID = list->head->id;
        list->deleteByID(finishedID);
        if (list->head)
            list->head->currentlyWorking = 0;
    }
}





 // ------------------------------------------------------------------
// The main function that will call the algorithms and take step in the virtual time in the code
// x = 0 FCFS 
// x = 1 SJF        NONPREEMPTIVE
// x = 2 SJF        PREEMPTIVE
// x = 3 PRIORITY   NONPREEMPTIVE
// x = 4 PRIORITY   PREEMPTIVE  
// x = 5 ROUND ROBIN
// ------------------------------------------------------------------

void step(LinkedList* list , int x){
    if(x == 0) FCFS(list);
    if(x == 1) SJF(list, 1);
    if(x == 2) SJF(list, 0);
    if(x == 3) Priority(list , 1);
    if(x == 4) Priority(list , 0);
     
     universalTime+= clockCycle;  

}   
void step(LinkedList* list , int x , int quantumTime){
    
    if(x == 5);roundRobin(list , quantumTime);
     universalTime+= clockCycle;

}
