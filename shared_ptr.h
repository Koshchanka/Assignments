#ifndef SHARED_PTR_H_
#define SHARED_PTR_H_

#include <utility>

namespace pointers {

template<typename T>
class GeneralPtr {
 public:
  [[nodiscard]] T* Get() {
    return this->ptr_;
  }
  [[nodiscard]] const T* Get() const {
    return this->ptr_;
  }

  virtual ~GeneralPtr() = default;

 protected:
  T* ptr_{nullptr};

  struct Counter {
    explicit Counter(int use_count) : use_count(use_count) {}
    int use_count{0};
    int weak_use_count{0};
  };
  Counter* counter_{nullptr};

  void CopyPointersFrom(const GeneralPtr& rhs) {
    this->ptr_ = rhs.ptr_;
    this->counter_ = rhs.counter_;
  }

  void Unbind() {
    this->ptr_ = nullptr;
    this->counter_ = nullptr;
  }

  GeneralPtr() = default;
};

template<typename T>
class SharedPtr : public GeneralPtr<T> {
 public:
  SharedPtr() {
    this->InitNullptr();
  }
  explicit SharedPtr(T* ptr) {
    this->ptr_ = ptr;
    this->counter_ = new typename GeneralPtr<T>::Counter(1);
  }
  SharedPtr(const SharedPtr& source) {
    *this = source;
  }
  SharedPtr(SharedPtr&& source) noexcept {
    *this = source;
  }
  ~SharedPtr() override {
    this->ReleaseMemory();
  }

  SharedPtr& operator=(const SharedPtr& rhs) {
    if (this == &rhs) {
      return *this;
    }
    this->ReleaseMemory();
    this->CopyPointersFrom(rhs);
    ++this->counter_->use_count;
    return *this;
  }
  SharedPtr& operator=(SharedPtr&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }
    this->ReleaseMemory();
    this->CopyPointersFrom(rhs);
    rhs.Unbind();
    return *this;
  }

  T& operator*() {
    return *this->ptr_;
  }
  const T& operator*() const {
    return *this->ptr_;
  }
  T* operator->() {
    return this->ptr_;
  }
  const T* operator->() const {
    return this->ptr_;
  }

  bool operator==(const SharedPtr& rhs) const {
    return this->ptr_ == rhs.ptr_;
  }
  bool operator==(const T* rhs) const {
    return this->ptr_ == rhs;
  }
  bool friend operator==(const T* lhs, const SharedPtr& rhs) {
    return rhs == lhs;
  }

  bool operator!=(const SharedPtr& rhs) const {
    return !(*this == rhs);
  }
  bool operator!=(const T* rhs) const {
    return !(*this == rhs);
  }
  bool friend operator!=(const T* lhs, const SharedPtr& rhs) {
    return !(lhs == rhs);
  }

  void Reset() {
    this->ReleaseMemory();
    this->InitNullptr();
  }

 private:
  template<typename>
  friend class WeakPtr;

  void InitNullptr() {
    this->ptr_ = nullptr;
    this->counter_ = new typename GeneralPtr<T>::Counter(1);
  }

  void ReleaseMemory() {
    if (this->counter_ == nullptr) {
      // `this` is already invalid
      return;
    }
    --this->counter_->use_count;
    if (this->counter_->use_count == 0) {
      delete this->ptr_;
      if (this->counter_->weak_use_count == 0) {
        delete this->counter_;
      }
    }
    this->Unbind();
  }

  SharedPtr(T* ptr, typename GeneralPtr<T>::Counter* imposed_counter) {
    this->ptr_ = ptr;
    this->counter_ = imposed_counter;
    if (this->counter_ != nullptr) {
      ++this->counter_->use_count;
    }
  }
};

}  // namespace pointers

#endif  // SHARED_PTR_H_
