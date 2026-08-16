#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <functional>
#include <cmath>
#include <iostream>
#include <cassert>

using namespace std;
using ValuePtr = shared_ptr<Value> //shared pointer so that when the memory increase we can increase the size of heap 

class Value : public enable_shared_from_this<Value>{
    private:
        inline static size_t currentID = 0; //to keep track of how many nodes are there in the tree
        float data; //a or b
        float grad; //gradient of the node
        string op; //operand +,-,*,/
        size_t id; //id of the nodes to distinguish between them
        vector<Value> prev; //when we do back propagation we need to know the previous node or the children node
    private:
        Value(float data, const string &op, size_t id) : data(data), op(op), id(id) {};
    public:
        static ValuePtr create(float data, const string &op = ""){
            return shared_ptr<Value>(new Value(data,op,Value::currentID++));
        }
        ~Value(){ 
            --Value::currentID; //destructor
        }
};




