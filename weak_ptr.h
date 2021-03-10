#ifndef WEAK_PTR_H_
#define WEAK_PTR_H_

#include "shared_ptr.h"

namespace pointers {

template<typename T>
class WeakPtr : public GeneralPtr<T> {
 public:
  WeakPtr() = default;
  explicit WeakPtr(const SharedPtr<T>& source) {
    this->CopyPointersFrom(source);
    ++this->counter_->weak_use_count;
  }
  WeakPtr(const WeakPtr& source) {
    *this = source;
  }
  WeakPtr(WeakPtr&& source) noexcept {
    *this = std::move(source);
  }
  ~WeakPtr() override {
    this->SafeDelete();
  }

  WeakPtr& operator=(const WeakPtr& rhs) {
    if (this == &rhs) {
      return *this;
    }
    this->SafeDelete();
    this->CopyPointersFrom(rhs);
    ++this->counter_->weak_use_count;
    return *this;
  }
  WeakPtr& operator=(WeakPtr&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }
    this->SafeDelete();
    this->CopyPointersFrom(rhs);
    rhs.Unbind();
    return *this;
  }

  void Reset() {
    this->SafeDelete();
  }

  [[nodiscard]] bool Expired() const {
    return this->ptr_ == nullptr || this->counter_->use_count == 0;
  }

  SharedPtr<T> Lock() const {
    if (this->Expired()) {
      return SharedPtr<T>();
    }
    return SharedPtr<T>(this->ptr_, this->counter_);
  }

 private:
  void SafeDelete() {
    if (this->counter_ == nullptr) {
      return;
    }
    --this->counter_->weak_use_count;
    if (this->counter_->use_count == 0 && this->counter_->weak_use_count == 0) {
      delete this->counter_;
    }
    this->Unbind();
  }
};

}  // namespace pointers

#endif  // WEAK_PTR_H_
