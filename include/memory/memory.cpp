#include <utils/logs.hpp>
#include <memory/memory.hpp>

using namespace Memory::Containers;
template <typename T, uint32_t N>
stack_array<T, N>::stack_array() { size = N; }

template <typename T, uint32_t N>
stack_array<T, N>::stack_array(stack_array &other) {

  assert_paranoid(size != other.size, "failed copying stack_array");
  //if (size != other.size) { /* DEBUG ASSERT */}

  for (uint32_t i = 0; i < other.size; i++) {
    p_items[i] = other.p_items[i];
  }
  size = other.size;
}

template <typename T, uint32_t N>
inline const T &stack_array<T, N>::operator[](uint32_t index) const {

  //if (index < 0 || index >= size) { /* DEBUG TRACE */ }
  return p_items[index];
}

template <typename T, uint32_t N>
inline T &stack_array<T, N>::operator[](uint32_t index) {

  //if (index < 0 || index >= size) { /* DEBUG TRACE */ }
  return p_items[index];
}

template <typename T, uint32_t N>
inline stack_array<T, N> & stack_array<T, N>::operator=(const stack_array &other) {

  //if (size != other.size) { /* DEBUG ASSERT */ }

  for (uint32_t i = 0; i < other.size; ++i) {
    p_items[i] = other.p_items[i];
  }
  size = other.size;

  return *this;
}



/////////////////////////////////////
// ____ VECTOR IMPLEMENTATION ____ //
////////////////////////////////////

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



///////////////////////////////////////
// ____ FLATLIST IMPLEMENTATION ____ //
//////////////////////////////////////

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



