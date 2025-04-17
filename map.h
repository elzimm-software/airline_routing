#ifndef AIRLINE_ROUTING_MAP_H
#define AIRLINE_ROUTING_MAP_H

#include <vector>
#include <string>

using std::vector;
using std::string;

template <typename V>
class HashMap {
private:
    struct Node {
        string key;
        V value;
        bool occupied;
        Node* next;

        Node(string key, V value): key(std::move(key)), value(std::move(value)), occupied(true), next(nullptr) {}
    };

    vector<Node*> buckets;
    size_t size;

    size_t hash(const string& key) const {
        size_t hash = 5381;
        for (char c: key) {
            hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
        }
        return hash % buckets.size();
    }
public:
    explicit HashMap(size_t capacity = 16): buckets(capacity, nullptr), size(0) {}

    ~HashMap() {
        for (Node* head: buckets) {
            while (head) {
                Node* tmp = head;
                head = head->next;
                delete tmp;
            }
        }
    }

    void insert(const string& key, const V& value) {
        size_t index = hash(key);
        Node* head = buckets[index];
        while (head) {
            if (head->key == key) {
                head->value = value;
                return;
            }
            head = head->next;
        }
        Node* new_node = new Node(key, value);
        new_node->next=buckets[index];
        buckets[index] = new_node;
        ++size;
    }

    bool at(const string& key, V& out) const {
        size_t index = hash(key);
        Node* head = buckets[index];
        while (head) {
            if (head->key == key) {
                out = head->value;
                return true;
            }
            head = head->next;
        }
        return false;
    }

    V& get(const string& key) {
        size_t index = hash(key);
        Node* head = buckets[index];
        while (head) {
            if (head->key == key) {
                return head->value;
            }
            head = head->next;
        }
    }

    void remove(const string& key) {
        size_t index = hash(key);
        Node* head = buckets[index];
        Node* prev = nullptr;
        while (head) {
            if (head->key == key) {
                if (prev) {
                    prev->next = head->next;
                } else {
                    buckets[index] = head->next;
                }
                delete head;
                --size;
                return;
            }
            prev = head;
            head = head->next;
        }
    }

    [[nodiscard]] bool contains(const string& key) const {
        V out;
        return at(key, out);
    }

    [[nodiscard]] size_t capacity() const {
        return size;
    }

    [[nodiscard]] bool empty() const {
        return size == 0;
    }
};

#endif
