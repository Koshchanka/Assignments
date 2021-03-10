#ifndef BIDIRECTIONAL_LIST_H_
#define BIDIRECTIONAL_LIST_H_

#include <utility>
#include <vector>
#include <cassert>
#include <iostream>
#include <stack>

namespace containers {

template<typename T>
class BiDirectionalList {
 public:
  struct Node {
   public:
    T value;
   private:
    Node* prev_{nullptr};
    Node* next_{nullptr};

    explicit Node(const T& value) : value(value) {}
    explicit Node(T&& value) : value(std::move(value)) {}

    friend class BiDirectionalList<T>;
  };
  BiDirectionalList() = default;
  BiDirectionalList(std::initializer_list<T> source) {
    for (const T& element : source) {
      this->PushBack(element);
    }
  }
  BiDirectionalList(const BiDirectionalList& source) {
    *this = source;
  }
  BiDirectionalList(BiDirectionalList&& source) noexcept {
    *this = std::move(source);
  }
  ~BiDirectionalList() {
    this->ReleaseMemory();
  }

  BiDirectionalList& operator=(const BiDirectionalList& rhs) {
    if (this == &rhs) {
      return *this;
    }
    this->ReleaseMemory();
    for (Node* iter{rhs.front_}; iter != nullptr; iter = iter->next_) {
      this->PushBack(iter->value);
    }
    return *this;
  }
  BiDirectionalList& operator=(BiDirectionalList&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }
    this->ReleaseMemory();
    this->CopyFieldsFrom(rhs);
    rhs.ResetFields();
    return *this;
  }

  [[nodiscard]] int Size() const {
    return this->size_;
  }
  [[nodiscard]] bool IsEmpty() const {
    return this->size_ == 0;
  }

  std::vector<T> ToVector() const {
    std::vector<T> result;
    for (Node* iter{this->front_}; iter != nullptr; iter = iter->next_) {
      result.push_back(iter->value);
    }
    return result;
  }

  Node* Front() {
    assert(this->size_ > 0);
    return this->front_;
  }
  const Node* Front() const {
    assert(this->size_ > 0);
    return this->front_;
  }
  Node* Back() {
    assert(this->size_ > 0);
    return this->back_;
  }
  const Node* Back() const {
    assert(this->size_ > 0);
    return this->back_;
  }

  void PushFront(const T& value) {
    this->InsertBefore(this->front_, value);
  }
  void PushFront(T&& value) {
    this->InsertBefore(this->front_, std::move(value));
  }
  void PushBack(const T& value) {
    this->InsertAfter(this->back_, value);
  }
  void PushBack(T&& value) {
    this->InsertAfter(this->back_, std::move(value));
  }

  void PopFront() {
    this->Erase(this->front_);
  }
  void PopBack() {
    this->Erase(this->back_);
  }

  void InsertBefore(Node* element, const T& value) {
    Node* new_node(new Node(value));
    this->InsertBefore(element, new_node);
  }
  void InsertBefore(Node* element, T&& value) {
    Node* new_node(new Node(std::move(value)));
    this->InsertBefore(element, new_node);
  }
  void InsertAfter(Node* element, const T& value) {
    Node* new_node(new Node(value));
    this->InsertAfter(element, new_node);
  }
  void InsertAfter(Node* element, T&& value) {
    Node* new_node(new Node(std::move(value)));
    this->InsertAfter(element, new_node);
  }

  void Erase(Node* element) {
    --this->size_;
    if (element == this->back_) {
      this->back_ = element->prev_;
    }
    if (element == this->front_) {
      this->front_ = element->next_;
    }
    if (element->prev_ != nullptr) {
      element->prev_->next_ = element->next_;
    }
    if (element->next_ != nullptr) {
      element->next_->prev_ = element->prev_;
    }
    delete element;
  }

  int Find(const T& value) const {
    int position{0};
    for (Node* iter{this->front_}; iter != nullptr; iter = iter->next_) {
      if (iter->value == value) {
        return position;
      }
      ++position;
    }
    return -1;
  }
  std::vector<T> FindAll(const T& value) const {
    std::vector<T> result;
    int position{0};
    for (Node* iter{this->front_}; iter != nullptr; iter = iter->next_) {
      if (iter->value == value) {
        result.push_back(position);
      }
      ++position;
    }
    return result;
  }

  Node* operator[](int index) {
    assert(index < this->size_);
    Node* iter{this->front_};
    for (int i{0}; i < index; ++i) {
      iter = iter->next_;
    }
    return iter;
  }
  const Node* operator[](int index) const {
    assert(index < this->size_);
    Node* iter{this->front_};
    for (int i{0}; i < index; ++i) {
      iter = iter->next_;
    }
    return iter;
  }

  std::stack<int, std::vector<int>> stack_on_vector;

  bool operator==(const BiDirectionalList& rhs) const {
    std::cout << bool(std::cin) << std::endl;
    if (this->size_ != rhs.size_) {
      return false;
    }
    Node* lhs_iter{this->front_};
    Node* rhs_iter{rhs.front_};
    while (lhs_iter != nullptr) {
      if (lhs_iter->value != rhs_iter->value) {
        return false;
      }
      lhs_iter = lhs_iter->next_;
      rhs_iter = rhs_iter->next_;
    }
    return true;
  }
  bool operator!=(const BiDirectionalList& rhs) const {
    return !(*this == rhs);
  }

 private:
  Node* front_{nullptr};
  Node* back_{nullptr};

  int size_{0};

  void ReleaseMemory() {
    for (Node* iter{this->front_}; iter != nullptr; iter = iter->next_) {
      delete iter->prev_;
    }
    delete this->back_;
    this->ResetFields();
  }
  void ResetFields() {
    this->front_ = nullptr;
    this->back_ = nullptr;
    this->size_ = 0;
  }

  void InitializeWithSingleNode(Node* only_node) {
    this->front_ = only_node;
    this->back_ = only_node;
    this->size_ = 1;
  }

  void InsertAfter(Node* node_in_list, Node* new_node) {
    if (node_in_list == nullptr) {
      this->InitializeWithSingleNode(new_node);
      return;
    }
    ++this->size_;
    new_node->prev_ = node_in_list;
    new_node->next_ = node_in_list->next_;
    node_in_list->next_ = new_node;
    if (new_node->next_ != nullptr) {
      new_node->next_->prev_ = new_node;
    } else {
      this->back_ = new_node;
    }
  }

  void InsertBefore(Node* node_in_list, Node* new_node) {
    if (node_in_list == nullptr) {
      this->InitializeWithSingleNode(new_node);
      return;
    }
    ++this->size_;
    new_node->next_ = node_in_list;
    new_node->prev_ = node_in_list->prev_;
    node_in_list->prev_ = new_node;
    if (new_node->prev_ != nullptr) {
      new_node->prev_->next_ = new_node;
    } else {
      this->front_ = new_node;
    }
  }

  void CopyFieldsFrom(const BiDirectionalList<T> source) {
    this->front_ = source.front_;
    this->back_ = source.back_;
    this->size_ = source.size_;
  }
};

}  // namespace containers

#endif  // BIDIRECTIONAL_LIST_H_
