#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "Arduino.h"
#include "AudioStream.h"

class RingBuffer {
public:
    /**
     * @brief Construct a new RingBuffer object
     * 
     * @param size the size of the buffer
     */
    RingBuffer(int size) : _size(size), _head(0), 
        _tail(0), _count(0), _data(new audio_block_t*[size](nullptr)) {}

    /**
     * @brief Destroy the Ring Buffer object
     * 
     */
    ~RingBuffer() {
        delete[] _data;
    }
    
    /**
     * @brief Pushes an item onto the ringbuffer, evicting the oldest item if full
     * 
     * @param item the item to push
     * @return an item that was evicted to make space, or nullptr if no item was evicted
     */
    audio_block_t* push(audio_block_t* item) {
        audio_block_t *ret = nullptr;
        if (isFull()) {
            ret = pop();
        }
   
        _data[_head] = item;
        _head = (_head + 1) % _size;     
        _count++;
        return ret;
    }
    
    /**
     * @brief Peeks the item at the tail of the ringbuffer
     * 
     * @return The oldest item in the ringbuffer
     */
    audio_block_t* peek() {
        return _data[_tail];
    }

    /**
     * @brief Peeks the item that is n items away from the front (important for delay)
     * 
     * @param n the index from which to peek (i.e. 0 for head, 1 for next item, etc.)
     * @return the (n+1)'th newest item in the ringbuffer
     */
    audio_block_t* peekFront(int n) {
        // not sure if % handles negative numbers correctly... add _size to be sure
        return _data[(_head - n + _size) % _size];
    }

    /**
     * @brief Pops an item from the ringbuffer
     * 
     * @return The oldest item in the ringbuffer 
     */
    audio_block_t* pop() {
        audio_block_t* item = peek();
        _tail = (_tail + 1) % _size;
        if (_count > 0) {
            _count--;
        }
        return item;
    }
    
    /**
     * @brief Gets the number of items in the ringbuffer
     * 
     * @return The number of items in the ringbuffer
     */
    int count() {
        return _count;
    }
    
    /**
     * @brief Checks whether the ringbuffer is empty
     * 
     * @return true the ringbuffer is empty
     * @return false the ringbuffer is not empty
     */
    bool isEmpty() {
        return _count == 0;
    }
    
    /**
     * @brief Checks whether the ringbuffer is full
     * 
     * @return true the ringbuffer is full
     * @return false the ringbuffer is not full
     */
    bool isFull() {
        return _count == _size;
    }

private:
    uint32_t _size; // allocated size
    
    // technically should not need all 3 but it's a little easier
    uint32_t _head;
    uint32_t _tail;
    uint32_t _count;

    audio_block_t** _data;
};


#endif // RINGBUFFER_H
