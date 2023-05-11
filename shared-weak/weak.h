#pragma once

#include "sw_fwd.h"

template <typename T>
class WeakPtr {
public:
    WeakPtr() : ptr_(nullptr), block_(nullptr) {
    }

    WeakPtr(const WeakPtr& other) : ptr_(other.ptr_), block_(other.block_) {
        SafeWeakIncrease();
    }

    template <typename Y>
    WeakPtr(const WeakPtr<Y>& other) : ptr_(other.ptr_), block_(other.block_) {
        SafeWeakIncrease();
    }

    WeakPtr(WeakPtr&& other) : ptr_(other.ptr_), block_(other.block_) {
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    }

    template <typename Y>
    WeakPtr(WeakPtr<Y>&& other) : ptr_(other.ptr_), block_(other.block_) {
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    }

    WeakPtr(const SharedPtr<T>& other) : ptr_(other.ptr_), block_(other.block_) {
        SafeWeakIncrease();
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (other.ptr_ == ptr_) {
            return *this;
        }
        SafeWeakDecrease();
        ptr_ = other.ptr_;
        block_ = other.block_;
        SafeWeakIncrease();
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(const WeakPtr<Y>& other) {
        if (other.ptr_ == ptr_) {
            return *this;
        }
        SafeWeakDecrease();
        ptr_ = other.ptr_;
        block_ = other.block_;
        SafeWeakIncrease();
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
        if (other.ptr_ == ptr_) {
            return *this;
        }
        SafeWeakDecrease();
        Swap(other);
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(WeakPtr<Y>&& other) {
        if (other.ptr_ == ptr_) {
            return *this;
        }
        SafeWeakDecrease();
        Swap(other);
        return *this;
    }

    WeakPtr& operator=(const SharedPtr<T>& other) {
        if (other.ptr_ == ptr_) {
            return *this;
        }
        SafeWeakDecrease();
        ptr_ = other.ptr_;
        block_ = other.block_;
        SafeWeakIncrease();
        return *this;
    }

    ~WeakPtr() {
        SafeWeakDecrease();
    }

    void Reset() {
        SafeWeakDecrease();
    }

    void Swap(WeakPtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    size_t UseCount() const {
        if (block_ != nullptr) {
            return block_->GetUseCount();
        }
        return 0;
    }

    bool Expired() const {
        return UseCount() == 0;
    }

    SharedPtr<T> Lock() const {
        if (Expired()) {
            return SharedPtr<T>{};
        }
        block_->IncreaseUsage();
        SharedPtr<T> result;
        result.ptr_ = ptr_;
        result.block_ = block_;
        return result;
    }

private:
    template <class Y>
    friend class WeakPtr;

    template <class Y>
    friend class SharedPtr;

    template <class Y>
    friend class BlockWithPointer;

    template <class Y>
    friend class BlockWithObject;

    T* ptr_;
    ControlBlock* block_;

    void SafeWeakDecrease() {
        if (block_ != nullptr) {
            block_->DecreaseWeakUsage();
        }
        ptr_ = nullptr;
        block_ = nullptr;
    }

    void SafeWeakIncrease() {
        if (block_ != nullptr) {
            block_->IncreaseWeakUsage();
        }
    }

    void UnsafeReset() {
        ptr_ = nullptr;
        block_ = nullptr;
    }
};
