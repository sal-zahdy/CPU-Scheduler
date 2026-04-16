#include <iostream>
using namespace std;

int universalTime=0;
int clockCycle =50;



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
    // insertAtHead – O(1)
    // ------------------------------------------------------------------
    void insertAtHead(double burst, int priority) {
        Node* node = createNode(burst, priority);
        node->next = head;
        head       = node;
        std::cout << "[INSERT HEAD] Node ID=" << node->id << " added at head.\n";
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
    // ------------------------------------------------------------------
    // Take step as SJF  takes 0 if premptive, takes 1 if nonpremptive 
    // ------------------------------------------------------------------

void SJF(LinkedList* list , bool x){
    
    if(!list->head) return;         //chech if the list is empty

    if(!x){       // if premptive
            
        list->sortByTime();                      // sort first

    }else{       // if nonpremtive

        if(!list->head->currentlyWorking){      // if in the last tick we were not working on it,
                                                // this means it is a new proccess, we must sort first
            list->sortByTime();                  
            list->head->currentlyWorking = 1;   // set it currently working so in the next step we dont sort                                                                          
        }              
       
    }
    list->head->burst-= clockCycle; // minus 50ms from the proccess life
    if (list->head->burst <= 0)     //check if the proccess is finished
        list->deleteByID(list->head->id);   // if soo then delete the current proccess 
}


int quantumTime;
void roundRobin(LinkedList* list , int qTime){

     if(!list->head) return;         //chech if the list is empty

    list->head->burst-= clockCycle;
    quantumTime-= clockCycle;

    if(quantumTime<=0){
        if (list->head->burst <= 0){     //check if the proccess is finished
            list->deleteByID(list->head->id);   // if soo then delete the current procces
        }else{                                  // if the proccess still needs to run then put it at the end
            
            Node* ptr = list->getLastNode();
            ptr->next = list->head;
            list->head=list->head->next;
            ptr->next->next = nullptr;
        }
        quantumTime=qTime;

    }

}


void Priority(LinkedList* list, bool x)
{

    if(!list->head) return;         //chech if the list is empty

    if(!x)        // if premptive
    {

        list->sortByPriority();                      // sort first

    }
    else         // if nonpremtive
    {

        if(!list->head->currentlyWorking)       // if in the last tick we were not working on it,
        {
            // this means it is a new proccess, we must sort first
            list->sortByPriority();
            list->head->currentlyWorking = 1;   // set it currently working so in the next step we dont sort
        }

    }
    list->head->burst-= clockCycle; // minus 50ms from the proccess life

    std::cout << "[PRIORITY] Running ID=" << list->head->id
              << " | remaining time=" << list->head->burst
              << " | priority=" << list->head->priority << "\n";


    if (list->head->burst <= 0){   //check if the proccess is finished
        std::cout << "[PRIORITY] Finished ID=" << list->head->id << "\n";
        list->deleteByID(list->head->id);   // if soo then delete the current proccess
    }
}

void FCFS(LinkedList* list)
{
    if (!list->head) return;  // safety check

    // If this is a new process starting execution
    if (!list->head->currentlyWorking)
    {
        list->head->currentlyWorking = 1;
        std::cout << "[FCFS] Starting process ID=" << list->head->id << "\n";
    }

    // Execute current process
    list->head->burst -= clockCycle;

    std::cout << "[FCFS] Running process ID=" << list->head->id
              << " remaining time=" << list->head->burst << "\n";

    // If finished → remove and move to next
    if (list->head->burst <= 0)
    {
        int finishedID = list->head->id;
        list->deleteByID(finishedID);

        if (list->head)
        {
            // Next process becomes active
            list->head->currentlyWorking = 0;
        }

        std::cout << "[FCFS] Process ID=" << finishedID << " finished\n";
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
// ─────────────────────────────────────────────
//  main – demonstrates all operations
// ─────────────────────────────────────────────
int main() {
 // ─────────────────────────────────────────────
    // TEST 1: SJF Preemptive – shortest job finishes in one tick
    // ─────────────────────────────────────────────
    cout << "=== TEST 1: SJF Preemptive _ shortest finishes in one tick ===\n";
    {
        LinkedList queue;
        queue.insertAtTail(100.0, 1);
        queue.insertAtTail(30.0,  4);  // shortest, 30 < 50 -> deleted after tick
        queue.insertAtTail(200.0, 3);

        queue.sortByPriority();

        cout << "GUI init:     "; queue.print();
        step(&queue, 2); cout << "After tick 1: "; queue.print();
        // Expected: 30.0 deleted, head is now 100.0
    }

    // ─────────────────────────────────────────────
    // TEST 2: SJF Preemptive – job survives multiple ticks
    // ─────────────────────────────────────────────
    cout << "\n=== TEST 2: SJF Preemptive _ job takes multiple ticks ===\n";
    {
        LinkedList queue;
        queue.insertAtTail(200.0, 1);
        queue.insertAtTail(130.0, 2);  // shortest, needs 3 ticks to finish

        cout << "GUI init:     "; queue.print();
        step(&queue, 2); cout << "After tick 1: "; queue.print();  // 130 -> 80
        step(&queue, 2); cout << "After tick 2: "; queue.print();  // 80  -> 30
        step(&queue, 2); cout << "After tick 3: "; queue.print();  // 30  -> deleted
        // Expected: 130 gone after tick 3, 200 becomes head
    }

    // ─────────────────────────────────────────────
    // TEST 3: SJF Preemptive – new shorter job arrives mid-execution
    // ─────────────────────────────────────────────
    cout << "\n=== TEST 3: SJF Preemptive _ preemption on new arrival ===\n";
    {
        LinkedList queue;
        queue.insertAtTail(150.0, 1);

        cout << "GUI init:      "; queue.print();
        step(&queue, 2); cout << "After tick 1:  "; queue.print();  // 150 -> 100

        queue.insertAtTail(60.0, 2);  // GUI: user submits shorter process
        cout << "New job added: "; queue.print();

        step(&queue, 2); cout << "After tick 2:  "; queue.print();  // preempts, 60 -> 10
        step(&queue, 2); cout << "After tick 3:  "; queue.print();  // 10 deleted, 100 resumes
        // Expected: 60.0 finishes before 100.0 continues
    }

    // ─────────────────────────────────────────────
    // TEST 4: SJF Non-Preemptive – running job is not interrupted
    // ─────────────────────────────────────────────
    cout << "\n=== TEST 4: SJF Non-Preemptive _ no interruption mid-job ===\n";
    {
        LinkedList queue;
        queue.insertAtTail(150.0, 1);
        queue.insertAtTail(200.0, 2);

        cout << "GUI init:      "; queue.print();
        step(&queue, 1); cout << "After tick 1:  "; queue.print();  // picks 150, marks working

        queue.insertAtTail(20.0, 3);  // GUI: shorter job arrives mid-execution
        cout << "New job added: "; queue.print();

        step(&queue, 1); cout << "After tick 2:  "; queue.print();  // should NOT switch to 20.0
        step(&queue, 1); cout << "After tick 3:  "; queue.print();  // still running 150
        // Expected: 20.0 waits, 150 keeps running
    }

    // ─────────────────────────────────────────────
    // TEST 5: SJF Non-Preemptive – after job finishes picks next shortest
    // ─────────────────────────────────────────────
    cout << "\n=== TEST 5: SJF Non-Preemptive _ next shortest picked after finish ===\n";
    {
        LinkedList queue;
        queue.insertAtTail(40.0, 1);  // finishes in 1 tick
        queue.insertAtTail(90.0, 2);
        queue.insertAtTail(60.0, 3);

        cout << "GUI init:     "; queue.print();
        step(&queue, 1); cout << "After tick 1: "; queue.print();  // 40 deleted
        step(&queue, 1); cout << "After tick 2: "; queue.print();  // picks 60, marks working
        step(&queue, 1); cout << "After tick 3: "; queue.print();  // 60 -> 10
        step(&queue, 1); cout << "After tick 4: "; queue.print();  // 10 deleted, picks 90
        // Expected: execution order 40 -> 60 -> 90
    }

    // ─────────────────────────────────────────────
    // TEST 6: Edge – GUI keeps ticking on empty queue
    // ─────────────────────────────────────────────
    cout << "\n=== TEST 6: Empty queue _ GUI keeps ticking safely ===\n";
    {
        LinkedList queue;
        cout << "GUI init:     "; queue.print();
        step(&queue, 2); cout << "After tick 1: "; queue.print();
        step(&queue, 2); cout << "After tick 2: "; queue.print();
        // Expected: no crash, list stays empty
    }

    // ─────────────────────────────────────────────
    // TEST 7: Edge – single node drains completely
    // ─────────────────────────────────────────────
    cout << "\n=== TEST 7: Single node drains to empty ===\n";
    {
        LinkedList queue;
        queue.insertAtTail(120.0, 1);

        cout << "GUI init:     "; queue.print();
        step(&queue, 2); cout << "After tick 1: "; queue.print();  // 120 -> 70
        step(&queue, 2); cout << "After tick 2: "; queue.print();  // 70  -> 20
        step(&queue, 2); cout << "After tick 3: "; queue.print();  // 20  -> deleted
        step(&queue, 2); cout << "After tick 4: "; queue.print();  // empty, safe tick
    }

    return 0;
}