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


struct Hash{
    size_t operator()(const ValuePtr value) const;
};
class Value : public enable_shared_from_this<Value>{
    public:
        inline static size_t currentID = 0; //to keep track of how many nodes are there in the tree
        float data; //a or b
        float grad; //gradient of the node
        string op; //operand +,-,*,/
        size_t id; //id of the nodes to distinguish between them
        vector<ValuePtr> prev; //when we do back propagation we need to know the previous node or the children node
        function<void()> backward; //backward function
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
            out->backward = [lhs_weak = weak_ptr<Value>(lhs),rhs_weak = weak_ptr<Value>(rhs),out_weak = weak_ptr<Value>(out)](){
                // lhs->grad += out->grad;
                // rhs->grad += out->grad;
                lhs_weak.lock()->grad += out_weak.lock()->grad;
                rhs_weak.lock()->grad += out_weak.lock()->grad;

            };
            return out;
        }

        static ValuePtr multiply(const ValuePtr& lhs,const ValuePtr& rhs){
            auto out = Value::create(lhs->data*rhs->data,"*");
            out->prev = {lhs,rhs};
            out->backward = [lhs_weak = weak_ptr<Value>(lhs),rhs_weak = weak_ptr<Value>(rhs),out_weak = weak_ptr<Value>(out)](){
                // lhs->grad += out->grad;
                // rhs->grad += out->grad;
                lhs_weak.lock()->grad += rhs_weak.lock()->data*out_weak.lock()->grad;
                rhs_weak.lock()->grad += lhs_weak.lock()->data*out_weak.lock()->grad;

            };
            return out;
        }
        static ValuePtr subtract(const ValuePtr& lhs,const ValuePtr& rhs){
            auto out = Value::create(lhs->data-rhs->data,"-");
            out->prev = {lhs,rhs};
            out->backward = [lhs_weak = weak_ptr<Value>(lhs),rhs_weak = weak_ptr<Value>(rhs),out_weak = weak_ptr<Value>(out)](){
                // lhs->grad += out->grad;
                // rhs->grad += out->grad;
                lhs_weak.lock()->grad += out_weak.lock()->grad;
                rhs_weak.lock()->grad -= out_weak.lock()->grad;

            };
            return out;
        }
        
        static ValuePtr pow(const ValuePtr& base, float exponent) {
            float newValue = std::pow(base->data, exponent);
            auto out = Value::create(newValue, "^");
            out->prev = {base};
            out->backward = [base_weak = std::weak_ptr<Value>(base), out_weak = std::weak_ptr<Value>(out), exponent](){
                if (auto base = base_weak.lock()) {
                    base->grad += exponent * std::pow(base->data, exponent - 1) * out_weak.lock()->grad;
                }
            };
            return out;
        }
        
        static ValuePtr divide(const ValuePtr& lhs, const ValuePtr& rhs) {
            auto reciprocal = pow(rhs, -1);
            return multiply(lhs, reciprocal);
        }

        void print(){
            cout << "[data=" <<this->data<<", grad="<<this->grad<<']'<<endl;
        }


        void buildTopo(shared_ptr<Value> v,unordered_set<shared_ptr<Value>,Hash>& visited, vector<shared_ptr<Value>>& topo){
            if (visited.find(v)==visited.end()){
                visited.insert(v);
                for (const auto& child : v->prev){
                    buildTopo(child,visited,topo);
                }
                topo.push_back(v);
            }
        } 


        void backProp(){
            vector<shared_ptr<Value>> topo;
            unordered_set<shared_ptr<Value>,Hash> visited;
            buildTopo(shared_from_this(),visited,topo);//this will create the graph for us

            // for (auto nodes : topo){
            //     nodes->backward();
            // }

            this->grad = 1.0f;
            for (auto it = topo.rbegin(); it != topo.rend();++it){
                if((*it)->backward){
                    (*it)->backward();
                }
                (*it)->print();
            }

        }



};


size_t Hash::operator()(const ValuePtr value) const {
    return hash<string>()(value.get()->op) ^ hash<float>()(value.get()->data);
}


int main(){
    auto a = Value::create(1.0, "");
    auto b = Value::create(2.0, "");
    auto c = Value::add(a,b); //3
    auto d = Value::multiply(c,c); //9
    assert(c->data == 3.0);
    assert(c->op == "+");
    assert(d->data == 9.0);
    assert(d->op == "*");
    // d->print();
    auto loss = Value::add(d,d); //18
    loss->backProp();

}

