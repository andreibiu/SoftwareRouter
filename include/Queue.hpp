#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "Utilities.hpp"

template <typename T>
class Queue
{
private:
    // Constants
    static constexpr szint ResizeFactor = 2;
    static constexpr szint MinCapacity = 4;

    // Fields & Components & Flags
    T* items;
    szint count;
    szint capacity;
    szint firstIndex;
    szint endIndex;
    
    // Private Methods
    void ensureCapacity(szint capacity);
    void reduceCapacity(szint capacity);

public:
    // Constructors 
    Queue();
    Queue(const Queue<T>& queue);
    Queue(Queue<T>&& queue);
    Queue(szint capacity);

    // Destructor
    ~Queue();

    // Operators
    void operator = (const Queue<T>& queue);
    void operator = (Queue<T>&& queue);

    // Getters
    inline szint getCount() const;

    // Public Methods
    bool isEmpty() const;
    void clear();
    void enqueue(const T& item);
    T dequeue();
    const T& peek();
    bool contains(const T& item) const;
};

/* Private Methods */

template <typename T>
void Queue<T>::ensureCapacity(szint capacity)
{
    if (this->capacity < capacity)
    {
        capacity = Utilities::max(capacity,
                       this->capacity * ResizeFactor);
        T* items = Utilities::allocate<T>(capacity);


        if (firstIndex < endIndex)
        {
            Utilities::move(items, this->items + firstIndex,
                endIndex - firstIndex);
        }
        else
        {
            szint count = this->capacity - firstIndex;
            Utilities::move(items, this->items + firstIndex, count);
            Utilities::move(items + count, this->items,
                this->count - count);
        }

        Utilities::release(this->items);
        this->items = items;
        this->capacity = capacity;
        firstIndex = 0;
        endIndex = count;
    }
}

template <typename T>
void Queue<T>::reduceCapacity(szint capacity)
{
    if (this->capacity > MinCapacity &&
        this->capacity > ResizeFactor * capacity)
    {
        capacity = Utilities::min(capacity, this->capacity / ResizeFactor);
        T* items = Utilities::allocate<T>(capacity);

        if (firstIndex < endIndex)
        {
            Utilities::move(items, this->items + firstIndex,
                endIndex - firstIndex);
        }
        else
        {
            szint count = this->capacity - firstIndex;
            Utilities::move(items, this->items + firstIndex,  count);
            Utilities::move(items + count, this->items,
                this->count - count);
        }

        Utilities::release(this->items);
        this->items = items;
        this->capacity = capacity;
        firstIndex = 0;
        endIndex = count;
    }
}

/* Constructors */

template <typename T>
Queue<T>::Queue()
{
    this->items = Utilities::allocate<T>(MinCapacity);
    this->capacity = MinCapacity;
    count = 0;
    firstIndex = 0;
    endIndex = 0;
}

template <typename T>
Queue<T>::Queue(const Queue<T>& queue)
{
    items = Utilities::allocate<T>(capacity);
    count = queue.count;
    capacity = queue.capacity;
    firstIndex = 0;
    endIndex = queue.count;

    Utilities::copy(items, queue.items, capacity);
}

template <typename T>
Queue<T>::Queue(Queue<T>&& queue)
{
    capacity = queue.capacity;
    count = queue.count;
    firstIndex = queue.firstIndex;
    endIndex = queue.endIndex;
    items = queue.items;
}

template <typename T>
Queue<T>::Queue(szint capacity)
{
    items = Utilities::allocate<T>(capacity);
    capacity = capacity;
    firstIndex = 0;
    endIndex = -1;
    count = 0;
}

/* Destructor */

template <typename T>
Queue<T>::~Queue()
{
    capacity = count = 0;
    firstIndex = 0;
    endIndex = -1;
    Utilities::release(items);
    items = nullptr;
}

/* Operators */

template <typename T>
void Queue<T>::operator = (const Queue<T>& queue)
{
    Utilities::release(items);

    capacity = queue.capacity;
    count = queue.count;
    items = new T[capacity];

    Utilities::copy(items, queue.items, capacity);

    this->firstIndex = 0;
    this->endIndex = count - 1;
}

template <typename T>
void Queue<T>::operator = (Queue<T>&& queue)
{
    capacity = queue.capacity;
    count = queue.count;
    firstIndex = queue.firstIndex;
    Utilities::release(items);
    items = queue.items;
    endIndex = queue.endIndex;
    items = queue.items;
    queue.items = nullptr;
}

/* Getters */

template <typename T>
inline szint Queue<T>::getCount() const
{
    return count;
}


/* Public Methods */

template <typename T>
bool Queue<T>::isEmpty() const
{
    return (count == 0);
}

template <typename T>
void Queue<T>::clear()
{
    count = 0;
    firstIndex = 0;
    endIndex = 0;
    capacity = MinCapacity;
    Utilities::release(items);
    items = new T[capacity];
}

template <typename T>
void Queue<T>::enqueue(const T& item)
{
    ensureCapacity(count + 1);
    items[endIndex] = item;
    endIndex = (endIndex + 1) & (capacity - 1);
    count++;
}

template <typename T>
T Queue<T>::dequeue()
{
    if (count == 0)
        throw "Cannot dequeue from an empty Queue.";

    T item = items[firstIndex];

    firstIndex = (firstIndex + 1) & (capacity - 1);
    count--;
    reduceCapacity(count);

    return item;
}

template <typename T>
const T& Queue<T>::peek()
{
    if (count == 0)
        throw "Cannot peek from an empty Queue.";

    return items[firstIndex];
}

template <typename T>
bool Queue<T>::contains(const T& item) const
{
    if (firstIndex < endIndex)
    {
        for (szint index = firstIndex; index < endIndex; index++)
            if (items[index] == item) return true;
    }
    else
    {
        for (szint index = firstIndex; index < capacity; index++)
            if (items[index] == item) return true;
        for (szint index = 0; index < endIndex; index++)
            if (items[index] == item) return true;
    }

    return false;
}

#endif // QUEUE_HPP
