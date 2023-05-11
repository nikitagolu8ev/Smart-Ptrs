#pragma once

#include <cstddef>  // std::nullptr_t

#include "sw_fwd.h"  // Forward declaration

template <typename T>
class EnableSharedFromThis;

template <typename T>
class SharedPtr {
public:
    SharedPtr() : ptr_(nullptr), block_(nullptr) {
    }

    SharedPtr(std::nullptr_t) : ptr_(nullptr), block_(nullptr) {
    }

    explicit SharedPtr(T* ptr) : ptr_(ptr), block_(new BlockWithPointer(ptr)) {
        if constexpr (std::is_constructible_v<ESFTBase*, T*>) {
            InitWeakThis(ptr);
        }
    }

    template <typename Y>
    explicit SharedPtr(Y* ptr) : ptr_(ptr), block_(new BlockWithPointer(ptr)) {
        if constexpr (std::is_constructible_v<ESFTBase*, T*>) {
            InitWeakThis(ptr);
        }
    }

    SharedPtr(const SharedPtr& other) : ptr_(other.ptr_), block_(other.block_) {
        SafeIncrease();
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) : ptr_(other.ptr_), block_(other.block_) {
        SafeIncrease();
    }

    SharedPtr(SharedPtr&& other) : ptr_(other.ptr_), block_(other.block_) {
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) : ptr_(other.ptr_), block_(other.block_) {
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : ptr_(ptr), block_(other.block_) {
        SafeIncrease();
    }

    explicit SharedPtr(const WeakPtr<T>& other) : ptr_(other.ptr_), block_(other.block_) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        SafeIncrease();
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (other.ptr_ == ptr_) {
            return *this;
        }
        SafeDecrease();
        ptr_ = other.ptr_;
        block_ = other.block_;
        SafeIncrease();
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        if (other.ptr_ == ptr_) {
            return *this;
        }
        SafeDecrease();
        Swap(other);
        return *this;
    }

    ~SharedPtr() {
        SafeDecrease();
    }

    void Reset() {
        SafeDecrease();
        ptr_ = nullptr;
        block_ = nullptr;
    }

    void Reset(T* ptr) {
        if (ptr == ptr_) {
            return;
        }
        SafeDecrease();
        ptr_ = ptr;
        if (ptr == nullptr) {
            block_ = nullptr;
        } else {
            block_ = new BlockWithPointer(ptr);
        }
    }

    template <class Y>
    void Reset(Y* ptr) {
        if (ptr == ptr_) {
            return;
        }
        SafeDecrease();
        ptr_ = ptr;
        if (ptr == nullptr) {
            block_ = nullptr;
        } else {
            block_ = new BlockWithPointer(ptr);
        }
    }

    void Swap(SharedPtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    T* Get() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    size_t UseCount() const {
        if (block_ == nullptr) {
            return 0;
        }
        return block_->GetUseCount();
    }

    explicit operator bool() const {
        return ptr_ != nullptr;
    }

private:
    template <typename Y>
    friend class SharedPtr;

    template <typename Y>
    friend class WeakPtr;

    template <typename Y, typename... Args>
    friend SharedPtr<Y> MakeShared(Args&&... args);

    T* ptr_;
    ControlBlock* block_;

    void SafeDecrease() {
        if (block_ != nullptr) {
            block_->DecreaseUsage();
        }
        ptr_ = nullptr;
        block_ = nullptr;
    }

    void SafeIncrease() {
        if (block_ != nullptr) {
            block_->IncreaseUsage();
        }
    }

    template <typename Y>
    void InitWeakThis(EnableSharedFromThis<Y>* e) {
        e->weak_this_ = *this;
    }
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return left.Get() == right.Get();
}

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    BlockWithObject<T>* block = new BlockWithObject<T>(std::forward<Args>(args)...);
    SharedPtr<T> result;
    result.ptr_ = block->GetPointer();
    result.block_ = block;
    if constexpr (std::is_constructible_v<ESFTBase*, T*>) {
        result.InitWeakThis(result.Get());
    }
    return result;
}

// Look for usage examples in tests and seminar

template <typename T>
class EnableSharedFromThis : public ESFTBase {
public:
    ~EnableSharedFromThis() {
        weak_this_ = nullptr;
    }

    SharedPtr<T> SharedFromThis() {
        return weak_this_.Lock();
    }
    SharedPtr<const T> SharedFromThis() const {
        return weak_this_.Lock();
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return weak_this_;
    }

    WeakPtr<const T> WeakFromThis() const noexcept {
        return weak_this_;
    }

private:
    template <typename Y>
    friend class SharedPtr;

    template <typename Y>
    friend class BlockWithPointer;

    template <typename Y>
    friend class BlockWithObject;

    WeakPtr<T> weak_this_;
};
