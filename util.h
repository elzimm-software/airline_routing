
#ifndef AIRLINE_ROUTING_UTIL_H
#define AIRLINE_ROUTING_UTIL_H

template<typename MapType, typename KeyType>
bool contains(const MapType& map, const KeyType& key) {
    return map.find(key) != map.end();
}

#endif
