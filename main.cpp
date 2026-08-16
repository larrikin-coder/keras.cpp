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

class Value;

using ValuePtr = shared_ptr<Value>; //shared pointer so that when the memory increase we can increase the size of heap 

class Value : public enable_shared_from_this<Value>{
    public:
        inline static size_t currentID = 0; //to keep track of how many nodes are there in the tree
        float data; //a or b
        float grad; //gradient of the node
        string op; //operand +,-,*,/
        size_t id; //id of the nodes to distinguish between them
        vector<ValuePtr> prev; //when we do back propagation we need to know the previous node or the children node
    private:
        Value(float data, const string &op, size_t id) : data(data), op(op), id(id) {};
    public:
        static ValuePtr create(float data, const string &op = ""){
            return shared_ptr<Value>(new Value(data,op,Value::currentID++));
        }
        ~Value(){ 
            --Value::currentID; //destructor
        }
        
        static ValuePtr add(const ValuePtr& lhs,const ValuePtr& rhs){
            auto out = Value::create(lhs->data+rhs->data,"+");
            out->prev = {lhs,rhs};
            return out;
        }

        static ValuePtr multiply(const ValuePtr& lhs,const ValuePtr& rhs){
            auto out = Value::create(lhs->data*rhs->data,"*");
            out->prev = {lhs,rhs};
            return out;
        }
        static ValuePtr subtract(const ValuePtr& lhs,const ValuePtr& rhs){
            auto out = Value::create(lhs->data-rhs->data,"-");
            out->prev = {lhs,rhs};
            return out;
        }
        void print(){
            cout << "[data=" <<this->data<<", grad="<<this->grad<<']'<<endl;
        }
};


int main(){
    auto a = Value::create(1.0, "");
    auto b = Value::create(2.0, "");
    auto c = Value::add(a,b);
    auto d = Value::multiply(c,c);
    assert(c->data == 3.0);
    assert(c->op == "+");
    assert(d->data == 9.0);
    assert(d->op == "*");
    d->print();
}

