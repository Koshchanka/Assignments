#ifndef BINARY_SEARCH_TREE_H_
#define BINARY_SEARCH_TREE_H_

#include <vector>
#include <queue>
#include <stack>

template<typename T>
class BinarySearchTree {
 private:
  struct TreeNode {
    template<typename U>
    explicit TreeNode(U&& source) : value(std::forward<U>(source)) {}
    template<typename... Ts>
    explicit TreeNode(Ts... args) : value(args...) {}

    void EntangleLeft(TreeNode* child) {
      left = child;
      if (child) {
        child->parent = this;
      }
    }
    void EntangleRight(TreeNode* child) {
      right = child;
      if (child) {
        child->parent = this;
      }
    }
    void ReleaseParent() {
      if (!parent) {
        return;
      }
      (parent->right == this ? parent->right : parent->left) = nullptr;
      parent = nullptr;
    }

    T value;
    TreeNode* left{nullptr};
    TreeNode* right{nullptr};
    TreeNode* parent{nullptr};
  };

 public:
  class ConstIterator
      : public std::iterator<std::bidirectional_iterator_tag, T> {
   public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    reference operator*() const { return ptr_->value; }
    pointer operator->() const { return &ptr_->value; }
    ConstIterator& operator++();
    ConstIterator operator++(int);
    ConstIterator& operator--();
    ConstIterator operator--(int);

    bool operator==(ConstIterator) const;
    bool operator!=(ConstIterator rhs) const { return !(*this == rhs); }

   private:
    explicit ConstIterator(TreeNode* ptr, const BinarySearchTree* parent)
        : ptr_(ptr), parent_(parent) {}

    friend class BinarySearchTree;

    TreeNode* ptr_;
    const BinarySearchTree* parent_;
  };

  BinarySearchTree() = default;
  BinarySearchTree(std::initializer_list<T>);
  BinarySearchTree(const BinarySearchTree& source) {
    CopyFrom(source);
  }
  BinarySearchTree(BinarySearchTree&& source) noexcept {
    MoveFrom(std::move(source));
  }
  ~BinarySearchTree() { ReleaseMemoryAndReset(); }
  BinarySearchTree& operator=(const BinarySearchTree&);
  BinarySearchTree& operator=(BinarySearchTree&&) noexcept;

  [[nodiscard]] int size() const { return size_; }
  [[nodiscard]] bool empty() const { return size() == 0; }
  [[nodiscard]] bool contains(const T& value) const {
    return find(value) != end();
  }
  [[nodiscard]] int count(const T&) const;
  [[nodiscard]] std::vector<T> to_vector() const;

  [[nodiscard]] ConstIterator begin() const {
    return ConstIterator(begin_, this);
  }
  [[nodiscard]] ConstIterator end() const {
    return ConstIterator(nullptr, this);
  }
  [[nodiscard]] ConstIterator find(const T&) const;

  template<typename U>
  void insert(U&& value) { insert(new TreeNode(T(std::forward<U>(value)))); }
  template<typename... Ts>
  void emplace(Ts... args) { insert(new TreeNode(args...)); }
  void erase(const T& value) { erase(find(value)); }
  void erase(ConstIterator);

  bool operator==(const BinarySearchTree&) const;
  bool operator!=(const BinarySearchTree& rhs) const {
    return !(*this == rhs);
  }

 private:
  void ReleaseMemoryAndReset();
  void CopyFrom(const BinarySearchTree&);
  void MoveFrom(BinarySearchTree&&);
  void CopyFieldsFrom(const BinarySearchTree&);
  void CopyUnder(TreeNode**, const TreeNode*);
  void UpdateBeginAndRbegin();
  void insert(TreeNode*);
  void ReturnToTree(TreeNode*);

  int size_{0};
  TreeNode* root_{nullptr};
  TreeNode* begin_{nullptr};
  TreeNode* rbegin_{nullptr};
};
template<typename T>
typename BinarySearchTree<T>::ConstIterator
BinarySearchTree<T>::ConstIterator::operator++(int) {
  auto copy{*this};
  ++*this;
  return copy;
}
template<typename T>
typename BinarySearchTree<T>::ConstIterator
BinarySearchTree<T>::ConstIterator::operator--(int) {
  auto copy{*this};
  --*this;
  return copy;
}
template<typename T>
bool BinarySearchTree<T>::ConstIterator::operator==(
    BinarySearchTree::ConstIterator rhs) const {
  return ptr_ == rhs.ptr_;
}
template<typename T>
typename BinarySearchTree<T>::ConstIterator&
BinarySearchTree<T>::ConstIterator::operator++() {
  if (ptr_->right) {
    ptr_ = ptr_->right;
    while (ptr_->left) {
      ptr_ = ptr_->left;
    }
    return *this;
  }
  while (ptr_->parent && !(ptr_->value < ptr_->parent->value)) {
    ptr_ = ptr_->parent;
  }
  ptr_ = ptr_->parent;
  return *this;
}
template<typename T>
typename BinarySearchTree<T>::ConstIterator&
BinarySearchTree<T>::ConstIterator::operator--() {
  if (!ptr_) {
    ptr_ = parent_->rbegin_;
    return *this;
  }
  if (ptr_->left) {
    ptr_ = ptr_->left;
    while (ptr_->right) {
      ptr_ = ptr_->right;
    }
    return *this;
  }
  while (ptr_->parent && ptr_->value < ptr_->parent->value) {
    ptr_ = ptr_->parent;
  }
  ptr_ = ptr_->parent;
  return *this;
}
template<typename T>
BinarySearchTree<T>&
BinarySearchTree<T>::operator=(const BinarySearchTree& rhs) {
  if (this == &rhs) {
    return *this;
  }
  ReleaseMemoryAndReset();
  CopyFrom(rhs);
  return *this;
}
template<typename T>
BinarySearchTree<T>&
BinarySearchTree<T>::operator=(BinarySearchTree&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }
  ReleaseMemoryAndReset();
  MoveFrom(std::move(rhs));
  return *this;
}
template<typename T>
std::vector<T> BinarySearchTree<T>::to_vector() const {
  std::vector<T> result;
  for (const T& value : *this) {
    result.push_back(value);
  }
  return result;
}
template<typename T>
void BinarySearchTree<T>::MoveFrom(BinarySearchTree&& source) {
  CopyFieldsFrom(source);
  source.CopyFieldsFrom(BinarySearchTree());
}
template<typename T>
typename BinarySearchTree<T>::ConstIterator
BinarySearchTree<T>::find(const T& target) const {
  TreeNode* candidate{root_};
  while (candidate) {
    if (candidate->value == target) {
      return ConstIterator(candidate, this);
    }
    candidate = candidate->value < target ? candidate->right : candidate->left;
  }
  return end();
}
template<typename T>
BinarySearchTree<T>::BinarySearchTree(std::initializer_list<T> list) {
  for (const T& value : list) {
    insert(value);
  }
}
template<typename T>
int BinarySearchTree<T>::count(const T& value) const {
  ConstIterator search_result{find(value)};
  if (search_result == end()) {
    return 0;
  }
  int accumulator{1};
  TreeNode* node{search_result.ptr_->right};
  while (node && node->value == value) {
    node = node->right;
    ++accumulator;
  }
  return accumulator;
}
template<typename T>
bool BinarySearchTree<T>::operator==(const BinarySearchTree& rhs) const {
  if (size() != rhs.size()) {
    return false;
  }
  ConstIterator this_iter{begin()};
  ConstIterator rhs_iter{rhs.begin()};
  while (this_iter != end()) {
    if (!(*this_iter == *rhs_iter)) {
      return false;
    }
    ++this_iter;
    ++rhs_iter;
  }
  return true;
}
template<typename T>
void BinarySearchTree<T>::CopyFieldsFrom(const BinarySearchTree& source) {
  size_ = source.size_;
  root_ = source.root_;
  begin_ = source.begin_;
  rbegin_ = source.rbegin_;
}
template<typename T>
void BinarySearchTree<T>::ReleaseMemoryAndReset() {
  std::queue<TreeNode*> nodes_to_delete;
  if (root_) {
    nodes_to_delete.push(root_);
  }
  while (!nodes_to_delete.empty()) {
    TreeNode* front{nodes_to_delete.front()};
    if (front->right) {
      nodes_to_delete.push(front->right);
    }
    if (front->left) {
      nodes_to_delete.push(front->left);
    }
    nodes_to_delete.pop();
    delete front;
  }
  size_ = 0;
  root_ = nullptr;
  begin_ = nullptr;
  rbegin_ = nullptr;
}
template<typename T>
void BinarySearchTree<T>::CopyFrom(const BinarySearchTree& source) {
  CopyUnder(&root_, source.root_);
  size_ = source.size();
  begin_ = root_;
  rbegin_ = root_;
  UpdateBeginAndRbegin();
}
template<typename T>
void BinarySearchTree<T>::CopyUnder(BinarySearchTree::TreeNode** target,
                                    const BinarySearchTree::TreeNode* source) {
  if (!source) {
    return;
  }
  *target = new typename BinarySearchTree<T>::TreeNode{source->value};
  CopyUnder(&(*target)->left, source->left);
  if ((*target)->left) {
    (*target)->left->parent = *target;
  }
  CopyUnder(&(*target)->right, source->right);
  if ((*target)->right) {
    (*target)->right->parent = *target;
  }
}
template<typename T>
void BinarySearchTree<T>::UpdateBeginAndRbegin() {
  TreeNode* begin_copy{begin_};
  while (begin_ && begin_->right && begin_->right->value == begin_->value) {
    begin_ = begin_->right;
  }
  if (!begin_ || !begin_->left) {
    begin_ = begin_copy;
  }
  while (begin_ && begin_->left) {
    begin_ = begin_->left;
  }
  while (rbegin_ && rbegin_->right) {
    rbegin_ = rbegin_->right;
  }
}
template<typename T>
void BinarySearchTree<T>::erase(BinarySearchTree::ConstIterator iter) {
  if (iter == end()) {
    return;
  }
  --size_;
  TreeNode* node_to_delete{iter.ptr_};
  if (node_to_delete == begin_) {
    begin_ = (++begin()).ptr_;
  }
  if (node_to_delete == rbegin_) {
    rbegin_ = (--ConstIterator(rbegin_, this)).ptr_;
  }
  if (node_to_delete == root_) {
    root_ = nullptr;
  }
  node_to_delete->ReleaseParent();
  if (node_to_delete->left) {
    ReturnToTree(node_to_delete->left);
  }
  if (node_to_delete->right) {
    ReturnToTree(node_to_delete->right);
  }
  delete node_to_delete;
}
template<typename T>
void BinarySearchTree<T>::ReturnToTree(TreeNode* node) {
  node->ReleaseParent();
  --size_;
  insert(node);
}
template<typename T>
void BinarySearchTree<T>::insert(BinarySearchTree::TreeNode* node) {
  ++size_;
  if (!root_) {
    root_ = node;
    begin_ = node;
    rbegin_ = node;
    UpdateBeginAndRbegin();
    return;
  }
  ConstIterator iter{find(node->value)};
  if (iter != end()) {
    TreeNode* same_node{iter.ptr_};
    if (same_node->parent) {
      if (same_node->parent->right == same_node) {
        same_node->parent->EntangleRight(node);
      } else {
        same_node->parent->EntangleLeft(node);
      }
    }
    node->EntangleLeft(same_node->left);
    same_node->left = nullptr;
    node->EntangleRight(same_node);
    if (same_node == root_) {
      root_ = node;
    }
    if (same_node == begin_) {
      begin_ = node;
    }
    if (same_node == rbegin_) {
      rbegin_ = node;
    }
  } else {
    TreeNode* candidate{root_};
    while (true) {
      if (candidate->value < node->value) {
        if (candidate->right) {
          candidate = candidate->right;
          continue;
        }
        candidate->EntangleRight(node);
        break;
      } else {
        if (candidate->left) {
          candidate = candidate->left;
          continue;
        }
        candidate->EntangleLeft(node);
        break;
      }
    }
  }
  UpdateBeginAndRbegin();
}

#endif  // BINARY_SEARCH_TREE_H_
