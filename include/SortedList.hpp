#ifndef SORTED_LIST_HPP
#define SORTED_LIST_HPP

#include "Utilities.hpp"

template <typename T>
class SortedList
{
private:
    // Constants
    static constexpr szint ResizeFactor = 2;
    static constexpr szint MinCapacity = 2;

    // Fields & Componenets
    T* items;
    szint capacity;
    szint count;

    // Private Methods
    void ensureCapacity(szint capacity);
    void reduceCapacity(szint capacity);
    T* binarySearch(const T& item) const;
    void binaryInsert(const T& item);

public:
    // Constructors
    SortedList();
    SortedList(const SortedList<T>& sortedList);
    SortedList(SortedList<T>&& sortedList);
    SortedList(szint capacity);

    // Destructor
    ~SortedList();

    // Operators
    void operator = (const SortedList<T>& sortedList);
    void operator = (SortedList<T>&& sortedList);

    // Getters
    inline szint getCount() const;

    // Public Methods
    bool isEmpty() const;
    void clear();
    void trimExcess();
    void add(const T& item);
    void remove(const T& item);
    T* find(const T& item) const;

    // Iterator Methods
    T* begin();
    const T* begin() const;
    T* end();
    const T* end() const;
};

/* Private Methods */

template <typename T>
void SortedList<T>::ensureCapacity(szint capacity)
{
    if (this->capacity < capacity)
    {
        capacity = Utilities::max(capacity, this->capacity * ResizeFactor);
        Utilities::reallocate(items, capacity);
        this->capacity = capacity;
    }
}

template <typename T>
void SortedList<T>::reduceCapacity(szint capacity)
{
    if (this->capacity > MinCapacity && this->capacity > ResizeFactor * capacity)
    {
        capacity = Utilities::min(capacity, this->capacity / ResizeFactor);
        Utilities::reallocate(items, capacity);
        this->capacity = capacity;
    }
}

template <typename T>
T* SortedList<T>::binarySearch(const T& value) const
{
    bool less;
    T* left = items;
    T* right = items + count - 1;
    T* middle;

    while (left <= right)
    {
        middle = left + ((right - left) >> 1);
        if (*middle == value)
        {
            return middle;
        }
        else
        {
            less = (*middle < value);
            left = less ? (middle + 1) : left;
            right = less ? right : (middle - 1);
        }
    }

    return nullptr;
}

template <typename T>
void SortedList<T>::binaryInsert(const T& value)
{

    if (value > items[count - 1])
    {
        items[count] = value;
    }
    else
    {
        bool less;
        T* left = items;
        T* right = items + count - 1;
        T* middle;

        while (left < right)
        {
            middle = left + ((right - left) >> 1);
            less = (value < *middle);
            left = less ? left : (middle + 1);
            right = less ? middle : right;
        }
        Utilities::move(left, count - (left - items), 1);
        *left = value;
    }
}

/* Constructors */

template <typename T>
SortedList<T>::SortedList()
{
    count = 0;
    capacity = MinCapacity;
    items = Utilities::allocate<T>(capacity);
}

template <typename T>
SortedList<T>::SortedList(const SortedList<T>& sortedList)
{
    count = sortedList.count;
    capacity = sortedList.capacity;
    items = Utilities::allocate<T>(capacity);
    Utilities::copy(items, sortedList.items, count);
}

template <typename T>
SortedList<T>::SortedList(SortedList<T>&& sortedList)
{
    count = sortedList.count;
    capacity = sortedList.capacity;
    items = sortedList.items;
}

template <typename T>
SortedList<T>::SortedList(szint capacity)
{
    count = 0;
    capacity = capacity;
    items = Utilities::allocate<T>(capacity);
}

/* Destructor */

template <typename T>
SortedList<T>::~SortedList()
{
    count = 0;
    capacity = 0;
    Utilities::release(items);
    items = nullptr;
}

/* Operators */

template <typename T>
void SortedList<T>::operator = (const SortedList<T>& sortedList)
{
    count = sortedList.count;
    capacity = sortedList.capacity;
    Utilities::release(items);
    items = Utilities::allocate<T>(capacity);
    Utilities::copy(items, sortedList.items, count);
}

template <typename T>
void SortedList<T>::operator = (SortedList<T>&& sortedList)
{
    count = sortedList.count;
    capacity = sortedList.capacity;
    Utilities::release(items);
    items = sortedList.items;
}

/* Getters */

template <typename T>
inline szint SortedList<T>::getCount() const
{
    return count;
}

/* Public Methods */

template <typename T>
bool SortedList<T>::isEmpty() const
{
    return (count == 0);
}

template <typename T>
void SortedList<T>::clear()
{
    Utilities::release(items);
    count = 0;
    capacity = MinCapacity;
    items = Utilities::allocate<T>(capacity);
}

template <typename T>
void SortedList<T>::trimExcess()
{
    if (count == capacity) return;

    if (count != 0) capacity = count;
    else capacity = MinCapacity;

    Utilities::reallocate(items, capacity);
}

template <typename T>
void SortedList<T>::add(const T& item)
{
    ensureCapacity(count + 1);
    binaryInsert(item);
    ++count;
}

template <typename T>
void SortedList<T>::remove(const T& item)
{
    T* pointer = binarySearch(item);
    if (pointer++ != nullptr)
    {
        Utilities::move(pointer, count - (pointer - items), -1);
        reduceCapacity(--count);
    }
}

template <typename T>
T* SortedList<T>::find(const T& item) const
{
    return binarySearch(item);
}

/* Iterator Methods */

template <typename T>
T* SortedList<T>::begin()
{
    return items;
}

template <typename T>
const T* SortedList<T>::begin() const
{
    return items;
}

template <typename T>
T* SortedList<T>::end()
{
    return items + count;
}

template <typename T>
const T* SortedList<T>::end() const
{
    return items + count;
}

#endif // SORTED_LIST_HPP
