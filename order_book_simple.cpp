#include <bits/stdc++.h>
#define endl '\n'

using namespace std;

int totalOrderNumber = 0;

struct Order {
    long long id;
    int price, time; ///price - negative if buying
    bool direction; /// 0 - bid(buy), 1 - ask(sell)
    mutable int quantities;

    Order() {}

    Order(long long _id, int _price, bool _direction, int _quantities) {
        id = _id;
        price = _price;
        time = totalOrderNumber ++;
        direction = _direction;
        quantities = _quantities;
    }
};

struct Trade {
    long long id_maker, id_taker;
    int price, quantities;

    Trade() {}

    Trade(long long _idm, long long _idt, int _p, int _q) {
        id_maker = _idm;
        id_taker = _idt;
        price = _p;
        quantities = _q;

    }
};

bool operator<(const Order& P, const Order& Q) {
    if(P.price != Q.price) return P.price < Q.price;
    return P.time < Q.time;
}

struct OrderBook
{
    set<Order> bids;
    set<Order> asks;
    
    map<long long, set<Order>::iterator> positions;

    map<int, int> quantities_bid;
    map<int, int> quantities_ask;

    vector<Trade> trades;

    bool cancelOrder(long long id)
    {
        if(positions.find(id) == positions.end()) return false;
        
        set<Order>::iterator it = positions[id];
        positions.erase(id);

        bool direction = (*it).direction;
        if(direction == false)
        {
            quantities_bid[(*it).price] -= (*it).quantities;
            if(quantities_bid[(*it).price] == 0) quantities_bid.erase(((*it).price));
            bids.erase(it);
        }
        else
        {
            quantities_ask[(*it).price] -= (*it).quantities;
            if(quantities_ask[(*it).price] == 0) quantities_ask.erase(((*it).price));
            asks.erase(it);
        }

        return true;
    }

    bool add(long long id, int price, bool direction, int quantities) {
        
        if(positions.find(id) != positions.end() || price < 0 || quantities < 0) return false;

        Order newOrder = Order(id, price, direction, quantities);
        if(direction == 0) {
            newOrder.price = -newOrder.price;
        }

        while(newOrder.quantities > 0) {
            set<Order>& opp = (direction == 0) ? asks : bids;
            if (opp.empty()) break;
            set<Order>::iterator it = opp.begin();
            const Order& challenger = *it; 

            if(challenger.price + newOrder.price > 0) {
                break;
            }

            else {
                int qt = min(newOrder.quantities, challenger.quantities);
                    
                if( qt < challenger.quantities ) {
                    trades.push_back(Trade(challenger.id, newOrder.id, abs(challenger.price), qt));

                    challenger.quantities -= qt;
                    
                    if(challenger.direction == 1) {
                        quantities_ask[challenger.price] -= qt;
                    }
                    else {
                        quantities_bid[challenger.price] -= qt;
                    }

                    newOrder.quantities -= qt;
                    break;
                }
                    
                else {
                    trades.push_back(Trade(challenger.id, newOrder.id, abs(challenger.price), qt));
                    newOrder.quantities -= challenger.quantities;
                    positions.erase(challenger.id);

                    if(challenger.direction == 1) {
                        quantities_ask[challenger.price] -= challenger.quantities;
                        if(quantities_ask[challenger.price] == 0 ) {
                            quantities_ask.erase(challenger.price);
                        }
                        asks.erase(asks.begin());
                    }
                    else {
                        quantities_bid[challenger.price] -= challenger.quantities;
                        if(quantities_bid[challenger.price] == 0 ) {
                            quantities_bid.erase(challenger.price);
                        }
                        bids.erase(bids.begin());
                    }

                    
                }
            }
        }

        if(newOrder.quantities > 0) {
            if(newOrder.direction == 0) {
                positions[newOrder.id] = bids.insert(newOrder).first;
                quantities_bid[newOrder.price] += newOrder.quantities;
            }
            else {
                positions[newOrder.id] = asks.insert(newOrder).first;
                quantities_ask[newOrder.price] += newOrder.quantities;
            }
        }
        
        return true;
    }

    pair<int, int> best_bid() {
        if(quantities_bid.empty()) return {-1, -1};
        return {-(*quantities_bid.begin()).first, (*quantities_bid.begin()).second}; 
    }

    pair<int, int> best_ask() {
        if(quantities_ask.empty()) return {-1, -1};
        return {(*quantities_ask.begin()).first, (*quantities_ask.begin()).second}; 
    }
};

int main()
{
    return 0;
}