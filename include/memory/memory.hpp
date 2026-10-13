#ifndef OK_MEM_HEADER
#define OK_MEM_HEADER


#include <cstdint>
#include <iostream>
#include <utility>

#define _OK_LOGS_IMPL
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

#endif
