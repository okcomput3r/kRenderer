#pragma once

#include <cstdint>
#include <iostream>
#include <utility>
#include <utils/logs.hpp>

namespace Memory::Containers {

//  ________________________________________________________________________________
// /--------------------------------------------------------------------------------\
// |- -|
// |- -|
// |-                              << STACK ARRAY >> -|
// |- -|
// |- -|
// \--------------------------------------------------------------------------------/

/***
 *
 * The stack_array Container is supose to act like a collection of elements
 * contiguious in memory. The memory is allocated in the stack and it can
 * be specify how big it should be, in order to optimize space.
 *
 */

//////////////////////////////
// ____  DECLARATION  ____ //
/////////////////////////////

template <typename T, uint32_t N> struct stack_array {

  typedef T       *iterator;
  typedef const T *const_iterator;

  inline iterator begin() { return &p_items[0]; }
  inline iterator end()   { return &p_items[size]; }
  inline const_iterator begin() const { return &p_items[0]; }
  inline const_iterator end()   const { return &p_items[size]; }

  inline T           &operator[](uint32_t index);
  inline const T     &operator[](uint32_t index) const;
  inline stack_array &operator=(const stack_array &other);

  T p_items[N];
  uint32_t size;

  stack_array();
  stack_array(stack_array &other);
};

//////////////////////////////
// ____ IMPLEMENTATION ____ //
/////////////////////////////

template <typename T, uint32_t N> 
stack_array<T, N>::stack_array() { size = N; }

template <typename T, uint32_t N>
stack_array<T, N>::stack_array(stack_array &other) {
  if (size != other.size) { /* DEBUG ASSERT */
  }

  for (uint32_t i = 0; i < other.size; i++) {
    p_items[i] = other.p_items[i];
  }
  size = other.size;
}

template <typename T, uint32_t N>
inline const T &stack_array<T, N>::operator[](uint32_t index) const {
  if (index < 0 || index >= size) { /* DEBUG TRACE */ }
  return p_items[index];
}

template <typename T, uint32_t N>
inline T &stack_array<T, N>::operator[](uint32_t index) {
  if (index < 0 || index >= size) { /* DEBUG TRACE */ }
  return p_items[index];
}

template <typename T, uint32_t N>
inline stack_array<T, N> & stack_array<T, N>::operator=(const stack_array &other) {
  if (size != other.size) { /* DEBUG ASSERT */ }

  for (uint32_t i = 0; i < other.size; ++i) {
    p_items[i] = other.p_items[i];
  }
  size = other.size;

  return *this;
}

// END OF MEMORY::CONTAINER::stack_array<>

//  ________________________________________________________________________________
// /--------------------------------------------------------------------------------\
// |- -|
// |- -|
// |-                                 << VECTOR >> -|
// |- -|
// |- -|
// \--------------------------------------------------------------------------------/

/***
 *
 *  The vector Container is a generic array with the ability to change
 *  size to acomodate new data.
 *
 *  The maximum size of this array is 2^32 = 4.294.967.296 (~4.3 billion items)
 *  but it is likely that you run out of memory way before reaching such
 *  number.
 *
 *  Usage:
 *
 *    -> There are two ways to create a vector:
 *        1. Calling the defalut constructor, wich allocates
 *        <s_default_vector_size> elements into the vector
 *           ej.. vector<int> default_vec {};
 *
 *        2. Calling the constructor and specifying the number of elements that needs allocation.
 *           ej.. vector<int> custom_vec {10}; <-- this allocates 10 ints;
 *
 *    ->  To push an item into the array, there are two ways:
 *        1. By calling the push_back() function, witchs moves (if posible) the element into the
 *           next position of the array.
 *
 *        2. By calling the emplace_back() function, witchs creates the object inside the desired
 *           position of the vector, thus avoiding an extra construction and posible memory allocation.
 *           To call this function, you need to give to the function the parameters required to
 *           create that structure.
 *           ej..
 *
 *            struct int&float
 *            {
 *              int   int_number;
 *              float float_number;
 *
 *              int&float(int a, float b)
 *               : int_number(a), float_number(b) {}
 *            }
 *
 *            int main()
 *            {
 *                vector<int&float> newVec {}
 *
 *                // push_back
 *                newVec.push_back(int&float(1, 0.0f)) <-- creates the object and then moves it to the vector
 *
 *               // emplace_back
 *                newVec.emplace_back(1, 0.0f);        <-- passes the argument to the vector directly
 *            }
 *
 *    ->  By default, the vector grows half its size each time it needs to resize, this can be changed
 *        by calling the function change_growth_ratio(float newRatio).
 *
 *  TODO:
 *  - implementation of srinking capabilities
 *  - implementation of policies for srinking and growing
 *  - fix the fuckass std::string implementation god I hate that shit dude
 *  - optimize for small vectors -> up to a certain threshold allocate on the
 *    stack
 *
 */

//////////////////////////////
// ____  DECLARATION  ____ //
/////////////////////////////

static const uint32_t s_default_vector_size{10};

template <typename T> struct vector {

  // Members
  T *p_items{};
  float growth_ratio{0.5f};
  uint32_t size{0};
  uint32_t capacity{0};
  uint32_t explicit_padding; // 32 bits -> 4 bytes

  // Iterators
  typedef T *iterator;
  typedef const T *const_iterator;

  inline iterator begin() { return &p_items[0]; }
  inline iterator end() { return &p_items[size]; }
  inline const_iterator begin() const { return &p_items[0]; }
  inline const_iterator end() const { return &p_items[size]; }

  // Overloaded operators
  inline const T &operator[](uint32_t index) const;
  inline T &operator[](uint32_t index);
  inline vector &operator=(const vector &other);
  inline vector &operator=(const vector &&other);

  // constructor / destructor
  vector();
  vector(uint32_t init_size);
  vector(vector &other);
  vector(vector &&other);

  ~vector();

  // Methods
  template <typename... Args> T &emplace_back(Args &&...item);
  void push_back(T &item);
  void push_back(T &&item);
  void pop_back();
  void clear();
  void resize(uint32_t newCapacity);
  inline void change_growth_ratio(float newRatio) { growth_ratio = newRatio; }
  inline bool is_empty() { return size == 0; } // returns size of the vector 
  inline uint32_t top() { return size == 0 ? 0 : size-1;}      // returns the position of the last element in the vector 
};

//////////////////////////////
// ____ IMPLEMENTATION ____ //
/////////////////////////////

template <typename T> vector<T>::vector() {

  resize(s_default_vector_size);
  size = 0;
  capacity = s_default_vector_size;
}

template <typename T> vector<T>::vector(uint32_t init_size) {

  resize(init_size);
  size = 0;
  capacity = init_size;
}

template <typename T> vector<T>::vector(vector &other) {

  T *p_newMem = static_cast<T *>(::operator new(other.capacity * sizeof(T)));

  for (int i = 0; i < other.size; ++i)
    p_newMem[i] = other.p_items[i];

  for (int i = 0; i < size; i++)
    p_items[i].~T();

  ::operator delete(p_items, sizeof(T) * capacity);

  p_items = p_newMem;
  size = other.size;
  capacity = other.capacity;
  growth_ratio = other.growth_ratio;
}

template <typename T> vector<T>::vector(vector &&other) {

  T *p_newMem = static_cast<T *>(::operator new(other.capacity * sizeof(T)));

  for (int i = 0; i < other.size; ++i)
    p_newMem[i] = std::move(other.p_items[i]);

  for (int i = 0; i < size; ++i)
    p_items[i].~T();

  ::operator delete(p_items, sizeof(T) * capacity);

  p_items = p_newMem;
  size = other.size;
  capacity = other.capacity;
  growth_ratio = other.growth_ratio;
}

template <typename T> vector<T>::~vector() {
  clear();
  ::operator delete(p_items, sizeof(T) * capacity);
}

template <typename T> void vector<T>::push_back(T &item) {
  if (size >= capacity)
    resize(capacity + capacity * growth_ratio);

  p_items[size] = item;
  ++size;
}

template <typename T> void vector<T>::push_back(T &&item) {
  if (size >= capacity)
    resize(capacity + capacity * growth_ratio);

  p_items[size] = std::move(item);
  ++size;
}

template <typename T>
template <typename... Args>
T &vector<T>::emplace_back(Args &&...args) {

  if (size >= capacity)
    resize(capacity + capacity * growth_ratio);

  new (&p_items[size]) T(std::forward<Args>(args)...);
  return p_items[size++];
}

template <typename T> void vector<T>::pop_back() {
  if (size > 0)
    p_items[size--].~T();
}

template <typename T> void vector<T>::resize(uint32_t newCapacity) {

  T *p_newMem = static_cast<T *>(::operator new(newCapacity * sizeof(T)));

  for (uint32_t i = 0; i < size; ++i)
    p_newMem[i] = std::move(p_items[i]);

  for (uint32_t i = 0; i < size; ++i)
    p_items[i].~T();

  ::operator delete(p_items, sizeof(T) * capacity);
  p_items = p_newMem;
  capacity = newCapacity;
}

template <typename T> void vector<T>::clear() {

  for (uint32_t i = 0; i < size; ++i)
    p_items[i].~T();

  size = 0;
}

template <typename T>
inline const T &vector<T>::operator[](uint32_t index) const {
  if (index >= size || index < 0) { /*DEBUG TRACE*/}

  return p_items[index];
}

template <typename T> inline T &vector<T>::operator[](uint32_t index) {
  if (index >= size || index < 0) { /*DEBUG TRACE*/
  }

  return p_items[index];
}

template <typename T>
inline vector<T> &vector<T>::operator=(const vector &other) {

  T *p_newMem = static_cast<T *>(::operator new(other.capacity * sizeof(T)));

  for (uint32_t i = 0; i < other.size; ++i)
    p_newMem[i] = other.p_items[i];

  for (uint32_t i = 0; i < size; i++)
    p_items[i].~T();

  ::operator delete(p_items, sizeof(T) * capacity);

  p_items = p_newMem;
  size = other.size;
  capacity = other.capacity;
  growth_ratio = other.growth_ratio;

  return *this;
}

template <typename T>
inline vector<T> &vector<T>::operator=(const vector &&other) {

  T *p_newMem = static_cast<T *>(::operator new(other.capacity * sizeof(T)));

  for (uint32_t i = 0; i < other.size; ++i)
    p_newMem[i] = std::move(other.p_items[i]);

  for (uint32_t i = 0; i < size; ++i)
    p_items[i].~T();

  ::operator delete(p_items, sizeof(T) * capacity);

  p_items = p_newMem;
  size = other.size;
  capacity = other.capacity;
  growth_ratio = other.growth_ratio;

  return *this;
}

// cute little std::string boilerplate because it has its own
// fuckass constructors that fucks with my fuckass mallocs and throws errors
// everywhere

template <> 
vector<std::string>::~vector() { delete[] p_items; }

template <> 
void vector<std::string>::pop_back() {
  if (size > 0)
    size--;
}

template <>
void vector<std::string>::resize(uint32_t newCapacity) {
  std::string *p_newMem = new std::string[newCapacity];

  for (uint32_t i = 0; i < size; ++i)
    p_newMem[i] = std::move(p_items[i]);

  delete[] p_items;

  p_items = p_newMem;
  capacity = newCapacity;
}

// END OF MEMORY::CONTAINERS::vector<>

//  ________________________________________________________________________________
// /--------------------------------------------------------------------------------\
// |
// |
// |                              << FLAT LIST >>
// |
// |
// \--------------------------------------------------------------------------------/

/***
 *
 * flat_list is (suposedly) an Implementation of a double linked list, where all its
 * nodes are contiguious in memory. This approach eliminates the venefit of
 * disperse memory allocation, in favor of a faster iteration thought its nodes.
 * Hopefully, if the implementation makes sense and in the end is done right,
 * this structure is suposed to serve as fundation to build tools that allows
 * for fast herachy trees transversation, meant for real time applications.
 *
 *  Now, in my head, this structure needs to acomplish a coupple of things:
 *    -> fast Addition and Deleting at any point of the list : O(1)
 *    -> optimized iteration throught the container : contiguious memory O(n)
 *
 * If I get lucky, we might even get SIMD instrucctions hanging arround, but
 * probably not.
 *
 * The downside to this trade is that, unlike in a linked list where, when a
 * node is removed, the memory management is usually trivial, with this
 * structure we cannot free the memory of a single node, so we are left with an
 * empty space in between nodes, wich would lead to internal fragmentation over
 * time. To mitigate this, all the addresses to the deleted nodes will be stored
 * into an auxiliar buffer, so that when a new node is needed, an existing
 * allocated memory address will be used. Sadly, this comes as a space penalty,
 * where as the spatial cost of a linked list would come as O(n * sizeof(T)),
 * here given our duplicate vector, this will leave us with a wors case scenario
 * of O(n * sizeof(T) + n * 8) on most 64 bit machines, given that a pointer
 * should be 8 bits long.
 *
 * CHANGE -> instead of a full vector only for unused nodes, i implemented a
 * dirty bit in the node to indicate if its being used. <- THIS WOULDNT WORK because
 * addition of a new node would be O(n) DUMBASSS
 *
 * TODO:
 *  - Implementation of reverse iterator
 *
 */

#define FLATLIST_INVALID_INDEX 0xFFFFFFFF

static const uint32_t s_default_list_size{100};
static const uint32_t s_default_freeSlots_size{10};

template <typename T> struct list_node {
 
  T value;

  /*
   * 8 bit mask that represets:
   *  [0] -> hasNext
   *  [1] -> hasPrevious
   *  [2] -> unused
   *  [3] -> free
   *  [4] -> free
   *  [5] -> free
   *  [6] -> free
   *  [7] -> free
   *
   */
  uint8_t state_mask {0};
 
  uint32_t next      {FLATLIST_INVALID_INDEX};
  uint32_t previous  {FLATLIST_INVALID_INDEX};

  // Default Constructor
  list_node(T &_value) : value (_value)            {}
  list_node(T &&_value): value (std::move(_value)) {}

  list_node(T &_value, uint32_t &_previous) 
    : value (_value),state_mask ((1 << 1) | (1 << 2)), previous (_previous) {}

  list_node(T &&_value, uint32_t &_previous) 
    : value (std::move(_value)), state_mask ((1 << 1) | (1 << 2)) , previous (_previous) {}

  list_node(T &_value, uint32_t &_previous, uint32_t &_next)
    : value (_value), next (_next), previous (_previous)
  {

    state_mask = (1) | (1 << 1) | (1 << 2);
  }

  ~list_node() = default;

  // shifts the value 8 bits to get the correct bit
  inline bool has_next()     { return state_mask & (1); }
  inline bool has_previous() { return state_mask & (1 << 1); }
  inline bool in_use()       {return false;} // UNUSED
  inline void set_next     (bool state) { (state == true) ? state_mask |= (1)      : state_mask &= ~(1); }
  inline void set_previous (bool state) { (state == true) ? state_mask |= (1 << 1) : state_mask &= ~(1 << 1); }
  inline void set_used     (bool state) { (state == true) ? state_mask |= (1 << 2) : state_mask &= ~(1 << 2); }
};

template<typename T> struct flatList_Iterator{
 
  typedef T value_type;

  list_node<value_type> *m_base;
  list_node<value_type> *m_rf;

  flatList_Iterator () {}

  flatList_Iterator (T *ptr) {}

  flatList_Iterator (list_node<value_type> *ptr)
    : m_base(ptr) , m_rf(ptr) {}

  flatList_Iterator (list_node<value_type> *base, list_node<value_type> *node)
    : m_base(base) , m_rf(node) {}

  flatList_Iterator  operator++(int) {
 
    flatList_Iterator tmp = *this;
    m_rf = static_cast<list_node<value_type>*>(m_base + m_rf->next);
    return tmp;
  }
  flatList_Iterator& operator++() {

    m_rf = static_cast<list_node<value_type>*>(m_base + m_rf->next);
    return *this;
  }

  flatList_Iterator operator--(int) {

    flatList_Iterator tmp = *this;
    m_rf = static_cast<list_node<value_type>*>(m_base + m_rf->previous);

    return tmp;
  }
 
  flatList_Iterator& operator--() {

    m_rf = static_cast<list_node<value_type>*>(m_base + m_rf->previous);
    return *this;
  }

  list_node<value_type>* operator->() {
    return m_rf;
  }
 
  //list_node<value_type>& operator*() {
  //  return *m_rf;
  //}

  value_type& operator*() {
    return m_rf->value; 
  }
  
  bool operator!=(const flatList_Iterator &other) const {
    return !(*this == other);
  }

  bool operator==(const flatList_Iterator &other) const {
    return (m_rf == other.m_rf);
  }

}; // End of flat_list iterator


template <typename T> struct flat_list {

 
  // Members

  vector<list_node<T>> bufferList {};
  vector<uint32_t>     freeNodes  {};                      // buffer that contains all the indices of the available nodes
  uint32_t i_root   {FLATLIST_INVALID_INDEX};              // index of the first element in the list
  uint32_t i_top    {FLATLIST_INVALID_INDEX};              // index of the last element in the list
  uint32_t m_size   {0};                                   // ammount of active nodes 
 
  // Iterators
 
  typedef flatList_Iterator<T> iterator;

  inline iterator begin() { return iterator { bufferList.p_items , bufferList.p_items + (i_root) }; }
  inline iterator end()   { return iterator { bufferList.p_items , bufferList.p_items + FLATLIST_INVALID_INDEX }; }


  // Constructors / destructor

  flat_list();
  flat_list(uint32_t item_count);
  flat_list(flat_list  &other);
  flat_list(flat_list &&other);

  ~flat_list() = default;

  // Overloaded operators
  inline flat_list &operator=(const flat_list &other);
  inline flat_list &operator=(const flat_list &&other);

  
  // Methods

  uint32_t add(T  &value);
  uint32_t add(T &&value);
  //void add(list_node<T> &node);

  uint32_t add_infront(T  &value, uint32_t victim_index);
  uint32_t add_infront(T &&value, uint32_t victim_index);
  uint32_t add_behind(T   &value, uint32_t victim_index);
  uint32_t add_behind(T  &&value, uint32_t victim_index);

  //void remove(const T &value);
  void remove(uint32_t index);
  void remove_top();

  void clear();

  list_node<T>& retrieve_node(uint32_t index);
  inline list_node<T>& get_root() { if (is_empty()) { /* TRACE */ } return retrieve_node(i_root); }
  inline list_node<T>& get_top()  { if (is_empty()) { /* TRACE */ } return retrieve_node(i_top);  }

  inline const bool is_empty() { return m_size == 0; }
  inline const uint32_t size() { return m_size; }

  
};

//////////////////////////////
// ____ IMPLEMENTATION ____ //
/////////////////////////////


// Constructors

template <typename T>
flat_list<T>::flat_list() {

  bufferList.resize(s_default_list_size);
  freeNodes.resize(s_default_freeSlots_size);
  m_size = 0;

}

template <typename T> 
flat_list<T>::flat_list(uint32_t item_count) {

  bufferList.resize(item_count);
  freeNodes.resize(s_default_freeSlots_size);
  m_size = 0;

}
  
template <typename T>
flat_list<T>::flat_list(flat_list<T> &other) {
  bufferList = other.bufferList;
  freeNodes  = other.freeNodes;

  i_root = other.i_root;
  i_top  = other.i_top;
  m_size = other.m_size;
}

template <typename T>
flat_list<T>::flat_list(flat_list<T> &&other){
  bufferList = std::forward<vector<list_node<T>>>(other.bufferList);
  freeNodes  = std::forward<vector<uint32_t>>(other.freeNodes);

  i_root = other.i_root;
  i_top  = other.i_top;
  m_size = other.m_size;
}



// Overloaded operators

template <typename T>
inline flat_list<T> &flat_list<T>::operator=(const flat_list<T> &other){
  bufferList = other.bufferList;
  freeNodes = other.freeNodes;

  i_root = other.i_root;
  i_top  = other.i_top;
  m_size = other.m_size;

}

template <typename T>
inline flat_list<T> &flat_list<T>::operator=(const flat_list<T> &&other){
  bufferList = std::forward<vector<list_node<T>>>(other.bufferList);
  freeNodes  = other.freeNodes;

  i_root = other.i_root;
  i_top  = other.i_top;
  m_size = other.m_size;
}

// Methods

/***
 *  flat_list<T>::add(T &val) takes a reference from the object that we want to add and 
 *  stores it in the buffer.
 *
 *  It works this way:
 *
 *  1. Checks if a free node is available, and if so, it reuses the slot. Later it returns.
 *
 *  2. If no free nodes are available,  then it checks if the list is empty, if so then it 
 *     emplace the new item into the buffer and sets it as the root node (for later transversal)
 *     and as the top node (for later fast insertions)
 *
 *  3. If the list is not empty, then the new item is pushed into the buffer, and the pointers 
 *     get updated. the new node is now the top node, and the previous top node now points to 
 *     this one. Furthermore, the new node points back to the previous top node, given that it
 *     is a double linked list.
 *
 *  Returns:
 *     this function returns a handler to the value added to the list.
 *
 *  NOTES:
 *    - There is another add() method that takes a temporal (&&) object instead of an already 
 *      constructed object, reducing the ammount of constructions that are taking place.
 *
 */

template <typename T> 
uint32_t flat_list<T>::add(T &val) {
  
  uint32_t newSlot_index {0};

  // if a free node is available we reuse it
  if(!freeNodes.is_empty()){

    newSlot_index = freeNodes[freeNodes.top()]; // get index from the top of the buffer

    // Configure new node to be the top of the list
    list_node<T> &newNode = bufferList[newSlot_index];
    newNode.value = val;
    if(is_empty()){
      newNode.previous = FLATLIST_INVALID_INDEX;
      newNode.set_previous(false);
    }
    else{
      newNode.previous = i_top;
      newNode.set_previous(true);
    }
    newNode.next  = FLATLIST_INVALID_INDEX;
    newNode.set_next(false);  // set node bitmask to show that it has no next node

    // Configure previous top of the list to link to the new top
    list_node<T> &top_node = retrieve_node(i_top);
    top_node.next = newSlot_index;
    top_node.set_next(true);

    i_top = newSlot_index;   // update top node

    freeNodes.pop_back();
    m_size++;
    return newSlot_index;

  }

  // first element case
  if (is_empty()) {

    bufferList.emplace_back(std::forward<T>(val));
    newSlot_index = bufferList.top();

    i_root = newSlot_index;
    i_top  = newSlot_index;

    m_size++;

  }
  else {
 
    bufferList.emplace_back(std::forward<T>(val), i_top); // emplace new node into the buffer
    newSlot_index = bufferList.top();

    list_node<T>& topNode = retrieve_node(i_top);
    topNode.next = newSlot_index;
    topNode.set_next(true);

    i_top = newSlot_index; // update top index

    m_size++;
  }

  return newSlot_index;
}

template <typename T>
uint32_t flat_list<T>::add(T &&val){

  assert_paranoid( size()-1 == FLATLIST_INVALID_INDEX ,"exceded MAX_ITEM_COUNT");

  uint32_t newSlot_index {FLATLIST_INVALID_INDEX};

  // if a free node is available we reuse it
  if(!freeNodes.is_empty()){
    newSlot_index = freeNodes[freeNodes.top()]; // get index from the top of the buffer
 
    // Configure new node to be the top of the list
    list_node<T> &newNode = bufferList[newSlot_index];
    newNode.value = std::move(val);

    // if buffer is empty then the new node has no previous
    if(is_empty()){
      newNode.previous = FLATLIST_INVALID_INDEX;
      newNode.set_previous(false);
    }
    else{
      newNode.previous = i_top;
      newNode.set_previous(true);
    }
    newNode.next  = FLATLIST_INVALID_INDEX;
    newNode.set_next(false);  // set node bitmask to show that it has no next node

    // Configure previous top of the list to link to the new top
    list_node<T> &top_node = retrieve_node(i_top);
    top_node.next = newSlot_index;
    top_node.set_next(true);

    i_top = newSlot_index;   // update top node

    freeNodes.pop_back();
    m_size++;

    return newSlot_index;
  }

  // first element case
  if (is_empty()) {

    bufferList.emplace_back(std::forward<T>(val));
    newSlot_index = bufferList.top();

    i_root = newSlot_index;
    i_top  = newSlot_index;

    m_size++;
  }
  else {
 
    bufferList.emplace_back(std::forward<T>(val), i_top); // emplace new node into the buffer
    newSlot_index = bufferList.top();

    list_node<T>& topNode = retrieve_node(i_top);
    topNode.next = newSlot_index;
    topNode.set_next(true);

    i_top = newSlot_index; // update top index

    m_size++;
  }

  return newSlot_index;
}


/***
 *
 * flat_list<T>::add_infront(&value, index) takes as arguments the value that
 * the user wants to add, and the index of the node where it wants to place it
 * infront of.
 *
 * note : This index can be obtained either at the creation of the node, where an
 * index to the node created is returned in order to keep track of it, or rather iterating 
 * through the list until a match is found.
 *
 * It works this way:
 *
 *  1. A new node is created at the top of the list, this new node is then un-linked from the top
 *     of the list and instead remains as an unlinked node.
 *
 *  2. if the victims_node (the node were we want to place a new one infront of) has another node
 *     linked after it, this link is severed, and the new node is placed between them.
 *
 *     if the victim_node does not have another node linked after it, it means its the top node.
 *     In this case, the new node becomes again tbe top node, and no node comes after it.
 *
 *  3. finally, the new block is linked to the previous block of the victim's, and the connection 
 *     is restored.
 *
 *
 * @parameter value -> T& : reference to the object intended to be added to the list.
 * @parameter victim_index -> uint32_t : handler to the node of the list the new item is going to be
 *            placed in front of.
 *
 * @return -> this function returns a handler to the value added to the list.
 *
 */

template <typename T>
uint32_t flat_list<T>::add_infront(T  &value, uint32_t victim_index){

  // new top, needs to be updated
  uint32_t new_block_index = add(value);
  list_node<T> &new_block = retrieve_node(new_block_index);
  
  // unlink the new block from the top, so the previous one becomes the new top
  list_node<T> &new_top = retrieve_node(new_block.previous);
  new_top.set_next(false);
  new_top.next = FLATLIST_INVALID_INDEX;
  i_top = new_block.previous;


  list_node<T> &victim = retrieve_node(victim_index);

  if (victim.has_next())
  {
    list_node<T> &victim_next = retrieve_node(victim.next);

    // update new block->next
    new_block.next = victim.next;
    new_block.set_next(true);

    // update victim's (last) next->previous
    victim_next.previous = new_block_index;
  }
  else {
    i_top = new_block_index;
    victim.set_next(true);
  }

  victim.next = new_block_index;
  
  new_block.previous = victim_index;
  new_block.set_previous(true);

  return new_block_index;
}


template <typename T>
uint32_t flat_list<T>::add_infront(T &&value, uint32_t victim_index){

  // new top, needs to be updated
  uint32_t new_block_index = add(std::forward<T>(value));
  list_node<T> &new_block = retrieve_node(new_block_index);
  
  // unlink the new block from the top, so the previous one becomes the new top
  list_node<T> &new_top = retrieve_node(new_block.previous);
  new_top.set_next(false);
  new_top.next = FLATLIST_INVALID_INDEX;
  i_top = new_block.previous;


  list_node<T> &victim = retrieve_node(victim_index);

  if (victim.has_next())
  {
    list_node<T> &victim_next = retrieve_node(victim.next);

    // update new block->next
    new_block.next = victim.next;
    new_block.set_next(true);

    // update victim's (last) next->previous
    victim_next.previous = new_block_index;
  }
  else {
    i_top = new_block_index;
    victim.set_next(true);
  }

  victim.next = new_block_index;
  
  new_block.previous = victim_index;
  new_block.set_previous(true);

  return new_block_index;
}

template <typename T>
uint32_t flat_list<T>::add_behind(T &value, uint32_t victim_index){

  // new top, needs to be updated
  uint32_t new_block_index = add(value);
  list_node<T> &new_block = retrieve_node(new_block_index);
  
  // unlink the new block from the top, so the previous one becomes the new top
  list_node<T> &new_top = retrieve_node(new_block.previous);
  new_top.set_next(false);
  new_top.next = FLATLIST_INVALID_INDEX;
  i_top = new_block.previous;


  list_node<T> &victim = retrieve_node(victim_index);

  if (victim.has_previous())
  {
    list_node<T> &victim_previous = retrieve_node(victim.previous);

    // update new block->previous
    new_block.previous = victim.previous;
    new_block.set_previous(true);

    // update victim's (last) previous->next
    victim_previous.next = new_block_index;
  }
  else {
    i_root = new_block_index;
    victim.set_previous(true);
  }

  victim.previous = new_block_index;
  
  new_block.next = victim_index;
  new_block.set_previous(true);

  return new_block_index;


}

template <typename T>
uint32_t flat_list<T>::add_behind(T  &&value, uint32_t victim_index){

  // new top, needs to be updated
  uint32_t new_block_index = add(std::forward<T>(value));
  list_node<T> &new_block = retrieve_node(new_block_index);
  
  // unlink the new block from the top, so the previous one becomes the new top
  list_node<T> &new_top = retrieve_node(new_block.previous);
  new_top.set_next(false);
  new_top.next = FLATLIST_INVALID_INDEX;
  i_top = new_block.previous;


  list_node<T> &victim = retrieve_node(victim_index);

  if (victim.has_previous())
  {
    list_node<T> &victim_previous = retrieve_node(victim.previous);

    // update new block->previous
    new_block.previous = victim.previous;
    new_block.set_previous(true);

    // update victim's (last) previous->next
    victim_previous.next = new_block_index;
  }
  else {
    i_root = new_block_index;
    victim.set_previous(true);
  }

  victim.previous = new_block_index;
  
  new_block.next = victim_index;
  new_block.set_previous(true);

  return new_block_index;
} 

/***
 *
 *  flat_list<T>::remove(uint32_t) allows for fast removal of a node from the buffer.
 *
 *  It works this way:
 *
 *  1. First it checks if the buffer is empty, if not it retrieves a reference to the node in the index
 *     specified.
 *
 *  2. If the index is valid then the deleted node is unlinked from the rest of the buffer so the linked 
 *     list is preserved intact. After the node is unlinked from the list it should not be accessible 
 *     anymore.
 *
 *  3.
 *     Then the deleted node is pushed into the freeNode buffer, and the size of the list buffer is 
 *     decremented.
 *
 */
template <typename T>
void flat_list<T>::remove(uint32_t index){
  if (is_empty()) { return; }

  list_node<T> &victim = retrieve_node(index);
 
  //unlink the nodes 
 
  if(victim.has_next()) {
    if(victim.has_previous()) // next & previous
    {
      retrieve_node(victim.previous).next  = victim.next;
      retrieve_node(victim.next).previous = victim.previous;
    }
    else // next & NO previous
    {
      list_node<T> &next_node = retrieve_node(victim.next);
      next_node.previous = FLATLIST_INVALID_INDEX;
      next_node.set_previous(false);
      i_root = victim.next;
    }
  }
  else if (victim.has_previous()){ // NO next & previous
      list_node<T> &prev_node = retrieve_node(victim.previous);

      prev_node.next = FLATLIST_INVALID_INDEX;
      prev_node.set_next(false);
      i_top = victim.previous;
  }
  else { // NO next & NO previous 
    // nothing
  }
  // end unlinking nodes

  freeNodes.push_back(index);
  m_size--;
}

template <typename T>
void flat_list<T>::remove_top()
{
  if (is_empty()) { return; }

  list_node<T> &victim = retrieve_node(i_top);
  list_node<T> &prev_node = retrieve_node(victim.previous);
  prev_node.next = FLATLIST_INVALID_INDEX;
  prev_node.set_next(false);

  freeNodes.push_back(i_top);
  i_top = victim.previous;

  m_size--;
}

template <typename T>
void flat_list<T>::clear()
{
  bufferList.clear();
  freeNodes.clear();
  m_size = 0;
  i_root = FLATLIST_INVALID_INDEX;
  i_top  = FLATLIST_INVALID_INDEX;
}


template <typename T>
list_node<T>& flat_list<T>::retrieve_node(uint32_t index) {
  if (index < 0 || index > bufferList.size) { std::cout << "invalid index" << std::endl;/* ASSERT */ }

  // if node is not being used throw debug trace, but not in release
  // DEBUG_TRACE (if (bufferList[index].maskbit & 8 == true)

  return bufferList[index];
}




// end of MEMORY::CONTAINERS::flat_list<>





//  ________________________________________________________________________________
// /--------------------------------------------------------------------------------\
// |
// |
// |                              << FRT TREE >>
// |
// |
// \--------------------------------------------------------------------------------/

/***
 *
 *  FRT (Fast Retrieve & Traverse) Tree is a tree like structure designed for UI
 *
 *
 *
 *
 *  TODO:
 *  - everything lol
 *
 */

const uint32_t s_max_node_childs   {10};
const uint32_t s_default_tree_size {100};

template <typename T> struct tree_node {

  T value{};
  stack_array<tree_node *, s_max_node_childs> p_childs{};
  uint32_t node_id{0};
  uint32_t childCount{0};
  /*
   * 8 bit mask that represets:
   *  [0] -> haschilds
   *  [1] -> dirty bit
   *  [2] ->
   *  [3] ->
   *  [4] ->
   *  [5] ->
   *  [6] ->
   *  [7] ->
   *
   */
  uint8_t state_mask{0};

  // Default Constructor
  tree_node(T &_value) {
    value = _value;
    childCount = 0;
    node_id = 0;
  }
  ~tree_node() {}

  // shifts the value 8 bits to get the correct bit
  inline bool hasChilds() { return state_mask & (1); }
};

template <typename T> struct frt_tree {

  vector<tree_node<T>> p_buffer {};
  tree_node<T>        *p_root   {nullptr};

  // Iterators

  typedef T       *iterator;
  typedef const T *const_iterator;

  inline iterator begin() { return &p_buffer[0]; }
  inline iterator end()   { return &p_buffer[p_buffer.size]; }
  inline const_iterator begin() const { return &p_buffer[0]; }
  inline const_iterator end()   const { return &p_buffer[p_buffer.size]; }

  frt_tree();
  frt_tree(uint32_t init_size);

  ~frt_tree();

  void addNode(T &item, tree_node<T> &parent);
  void addNode(tree_node<T> &node, tree_node<T> &parent);

  tree_node<T> &retrieveNode(/* ID? */);
};

template <typename T> frt_tree<T>::frt_tree() {
  p_buffer.resize(s_default_tree_size);
}

template <typename T> frt_tree<T>::~frt_tree() {}

template <typename T>
void frt_tree<T>::addNode(T &item, tree_node<T> &parent) {}

template <typename T> tree_node<T> &frt_tree<T>::retrieveNode(/* ID */) {}

// end of MEMORY::CONTAINERS::frt_tree<>

} // namespace Memory::Containers

namespace Memory::Arenas {

} // namespace Memory::Arenas
