#ifndef AIRLINE_ROUTING_MAP_H
#define AIRLINE_ROUTING_MAP_H

#include <vector>
#include <string>
#include <stdexcept>
#include <utility>

// Simple hash-based map using separate chaining
template <typename K, typename V>
class Map {
private:
    struct Node { // linked-list node
        K key;
        V value;
        Node* next;

        Node(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

    std::vector<Node*> buckets; // bucket heads
    std::vector<K> keys;        // insertion order
    size_t bucket_count;        // number of buckets

    size_t hash_key(const K& key) const { // polynomial rolling hash
        size_t h = 0;
        for (char c: key) h = h * 31 + static_cast<unsigned char>(c);
        return h % bucket_count;
    }

public:
    using value_type = std::pair<const K&, V&>;

    explicit Map(size_t capacity = 16) : buckets(capacity, nullptr), bucket_count(capacity) {} // constructor

    ~Map() { clear(); } // destructor

    void clear() { // remove all entries
        for (Node* head: buckets) {
            while (head) {
                Node* next = head->next;
                delete head;
                head = next;
            }
        }
        buckets.clear();
        keys.clear();
    }

    [[nodiscard]] bool empty() const { return keys.empty(); } // check if map has no entries
    [[nodiscard]] size_t size() const { return keys.size(); } // number of entries

    bool contains(const K& key) const { // check key existence
        size_t idx = hash_key(key);
        for (Node* cur = buckets[idx]; cur; cur = cur->next) if (cur->key == key) return true;
        return false;
    }

    void insert(const std::pair<K, V>& p) { insert(p.first, p.second); } // insert pair
    void insert(const K& key, const V& value) { // insert or update
        size_t idx = hash_key(key);
        for (Node* cur = buckets[idx]; cur; cur = cur->next) {
            if (cur->key == key) {
                cur->value = value;
                return;
            }
        }
        Node* node = new Node(key, value);
        node->next = buckets[idx];
        buckets[idx] = node;
        keys.push_back(key);
    }

    V& at(const K& key) { // access or throw
        size_t idx = hash_key(key);
        for (Node* cur = buckets[idx]; cur; cur = cur->next) if (cur->key == key) return cur->value;
        throw std::out_of_range("Key not found");
    }

    const V& at(const K& key) const { // const access
        size_t idx = hash_key(key);
        for (Node* cur = buckets[idx]; cur; cur = cur->next) if (cur->key == key) return cur->value;
        throw std::out_of_range("Key not found");
    }

    V& operator[](const K& key) { // access or insert default
        if (!contains(key)) insert(key, V());
        return at(key);
    }

    class Iterator { // range-based iterator
        const Map* map;
        size_t pos;
    public:
        Iterator(const Map* m, size_t p) : map(m), pos(p) {}

        Iterator& operator++() {
            ++pos;
            return *this;
        }

        bool operator!=(const Iterator& o) const { return map != o.map || pos != o.pos; }

        value_type operator*() const {
            const K& k = map->keys[pos];
            V& v = const_cast<Map*>(map)->at(k);
            return {k, v};
        }
    };

    Iterator begin() const { return Iterator(this, 0); } // start iterator
    Iterator end() const { return Iterator(this, keys.size()); } // end iterator
};


#endif
