#include <omp.h>
#include <iostream>
#include <vector>
#include <queue>

using namespace std;
int tnbr=0;
int nths=0;

// Represents a node of an n-ary tree
struct Node
{
    int value;
    char name;
    vector<Node *>child;
};

// Utility function to create a new tree node
Node *newNode(int value,const char * name)
{
    Node *temp = new Node;
    temp->value = value;
    temp->name = *name;
    return temp;
}

// Prints the n-ary tree level wise
void LevelOrderTraversal(Node * root)
{
    if (root==NULL){
        return;
    }
    queue<Node *> q;
    q.push(root);
    while (!q.empty()){
        int n = q.size();
        // If this node has children
        while (n > 0){
            // Dequeue an item from queue and print it
            Node * p = q.front();
            q.pop();
            cout << p->value << " ";
              // Enqueue all children of the dequeued item
              for (int i=0; i<p->child.size(); i++){
                  q.push(p->child[i]);
              }
              n--;
          }
        cout << endl;
      }
}

void ComputeValueSumSingle(Node * root)
{
    if (root==NULL){
        return;
    }
    //  DFS to reach deepest node recursively
    for (int i=0; i<root->child.size(); i++){
    if(root->child.size()!=0){
        ComputeValueSumSingle(root->child[i]);
    }
    // Node has no child, therefore increase parent value
    #pragma omp critical
          {
            root->value=root->value+root->child[i]->value;
          }
    int id=omp_get_thread_num();
    }
}

void ComputeValueSumMultiple(Node * root)
{
    if (root==NULL){
    return;
    }
    #pragma omp critical
    {
        tnbr++;
    }
    //  DFS to reach deepest node recursively
    for (int i=0; i<root->child.size(); i++){
        if(root->child.size()!=0){
            #pragma omp task
            {
                if(tnbr>=nths){
                    ComputeValueSumSingle(root->child[i]);}
                else{
                    #pragma omp critical
                    {
                        tnbr++;
                    }
                    ComputeValueSumMultiple(root->child[i]);}
            }
        }
        #pragma omp taskwait
        // Node has no child, therefore increase parent value
        #pragma omp critical
            {
                root->value=root->value+root->child[i]->value;
            }
    }
}

// Compute new Value of nodes
void ComputeValueSum(Node * root)
{
    if (root==NULL){
    return;
    }
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            int nths;
            nths=omp_get_num_threads();
            //  Spread Tasks across all Workers
            for (int i=0; i<root->child.size(); i++){
              if(root->child.size()!=0){
                  #pragma omp task shared(tnbr)
                  {
                      //After nths threads are spawned start computing serial
                      if(tnbr>=nths){
                          ComputeValueSumSingle(root->child[i]);}
                      else{
                          #pragma omp critical
                          {
                            tnbr++;
                          }
                          ComputeValueSumMultiple(root->child[i]);}
                  }
              }
              #pragma omp taskwait
               // Node has no child, therefore increase parent value
              #pragma omp critical
                    {
                        root->value=root->value+root->child[i]->value;
                    }
            }
        }
    }
}



int main()
{
    Node *root = newNode(1,"top");
    (root->child).push_back(newNode(3,"foo"));
    (root->child).push_back(newNode(5,"bar"));
    (root->child[0]->child).push_back(newNode(-1,"foo1"));
    (root->child[0]->child).push_back(newNode(4,"bar1"));
    (root->child[1]->child).push_back(newNode(9,"foo2"));
    cout << "Level order traversal\n";
    LevelOrderTraversal(root);
    cout << "Computed Valuesum\n";
    ComputeValueSum(root);
    LevelOrderTraversal(root);
    return 0;
}
