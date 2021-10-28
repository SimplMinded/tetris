#pragma once

#include <cstddef>

#include "util/assert.h"

template<typename Elem>
struct ListView
{
    size_t capacity;
    size_t count;
    Elem* elems;

    Elem& operator [](size_t index);
    const Elem& operator [](size_t index) const;
};

template<typename Elem>
Elem& ListView<Elem>::operator [](size_t index)
{
    ASSERT(index < count);
    ASSERT(elems != nullptr);
    return elems[index];
}

template<typename Elem>
const Elem& ListView<Elem>::operator [](size_t index) const
{
    ASSERT(index < count);
    ASSERT(elems != nullptr);
    return elems[index];
}

template<typename Elem>
ListView<Elem> makeListView(size_t capacity, Elem* elems)
{
    ASSERT(capacity > 0);
    ASSERT(elems != nullptr);
    return ListView<Elem>{ capacity, 0, elems };
}

template<typename Elem>
void add(ListView<Elem>& listView, const Elem& elem)
{
    ASSERT(listView.count < listView.capacity);
    ASSERT(listView.elems != nullptr);
    listView.elems[listView.count++] = elem;
}

template<typename Elem>
void clear(ListView<Elem>& listView)
{
    listView.count = 0;
}
