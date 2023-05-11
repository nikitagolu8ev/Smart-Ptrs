#pragma once

#include <cstddef>
#include <type_traits>

#include "compressed_pair.h"

template <typename T>
struct SimpleDeleter {
    SimpleDeleter() {
    }

    template <typename Y>
    SimpleDeleter(const SimpleDeleter<Y>&) {
    }

    void operator()(T* x) {
        delete x;
    }
};

template <typename T>
struct SimpleDeleter<T[]> {
    SimpleDeleter() {
    }

    template <typename Y>
    SimpleDeleter(const SimpleDeleter<Y[]>&) {
    }

    void operator()(T* x) {
        delete[] x;
    }
};

template <typename T>
struct RemoveReferenceForVoid {
    typedef T& Type;
};

template <>
struct RemoveReferenceForVoid<void> {
    typedef void Type;
};

template <typename T, typename Deleter = SimpleDeleter<T>>
class UniquePtr {
public:
    explicit UniquePtr(T* ptr = nullptr) : ptr_(ptr, Deleter{}) {
    }

    UniquePtr(T* ptr, const Deleter& deleter) : ptr_(ptr, deleter) {
    }

    UniquePtr(T* ptr, Deleter&& deleter) : ptr_(ptr, std::move(deleter)) {
    }

    template <typename OtherT, typename OtherDeleter>
    UniquePtr(UniquePtr<OtherT, OtherDeleter>&& other) noexcept {
        ptr_.GetFirst() = other.Release();
        ptr_.GetSecond() = std::move(other.GetDeleter());
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (&other == this) {
            return *this;
        }
        TryDelete(ptr_.GetFirst());
        ptr_.GetFirst() = other.Release();
        ptr_.GetSecond() = std::move(other.GetDeleter());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        TryDelete(ptr_.GetFirst());
        ptr_.GetFirst() = nullptr;
        return *this;
    }

    ~UniquePtr() {
        TryDelete(ptr_.GetFirst());
    }

    T* Release() {
        T* released_ptr = ptr_.GetFirst();
        ptr_.GetFirst() = nullptr;
        return released_ptr;
    }

    void Reset(T* ptr = nullptr) {
        T* old_ptr = ptr_.GetFirst();
        ptr_.GetFirst() = ptr;
        TryDelete(old_ptr);
    }

    void Swap(UniquePtr& other) {
        std::swap(ptr_, other.ptr_);
    }

    T* Get() const {
        return ptr_.GetFirst();
    }

    Deleter& GetDeleter() {
        return ptr_.GetSecond();
    }

    const Deleter& GetDeleter() const {
        return ptr_.GetSecond();
    }

    explicit operator bool() const {
        return ptr_.GetFirst() != nullptr;
    }

    typename RemoveReferenceForVoid<T>::Type operator*() const {
        return *ptr_.GetFirst();
    }

    T* operator->() const {
        return ptr_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> ptr_;

    void TryDelete(T* ptr) {
        if (ptr != nullptr) {
            ptr_.GetSecond()(ptr);
        }
    }
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    explicit UniquePtr(T* ptr = nullptr) : ptr_(ptr, Deleter{}) {
    }

    UniquePtr(T* ptr, const Deleter& deleter) : ptr_(ptr, deleter) {
    }

    UniquePtr(T* ptr, Deleter&& deleter) : ptr_(ptr, std::move(deleter)) {
    }

    template <typename OtherT, typename OtherDeleter>
    UniquePtr(UniquePtr<OtherT, OtherDeleter>&& other) noexcept {
        ptr_.GetFirst() = other.Release();
        ptr_.GetSecond() = std::move(other.GetDeleter());
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (&other == this) {
            return *this;
        }
        TryDelete(ptr_.GetFirst());
        ptr_.GetFirst() = other.Release();
        ptr_.GetSecond() = std::move(other.GetDeleter());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        TryDelete(ptr_.GetFirst());
        ptr_.GetFirst() = nullptr;
        return *this;
    }

    ~UniquePtr() {
        TryDelete(ptr_.GetFirst());
    }

    T* Release() {
        T* released_ptr = ptr_.GetFirst();
        ptr_.GetFirst() = nullptr;
        return released_ptr;
    }

    void Reset(T* ptr = nullptr) {
        T* old_ptr = ptr_.GetFirst();
        ptr_.GetFirst() = ptr;
        TryDelete(old_ptr);
    }

    void Swap(UniquePtr& other) {
        std::swap(ptr_, other.ptr_);
    }

    T* Get() const {
        return ptr_.GetFirst();
    }

    Deleter& GetDeleter() {
        return ptr_.GetSecond();
    }

    const Deleter& GetDeleter() const {
        return ptr_.GetSecond();
    }

    explicit operator bool() const {
        return ptr_.GetFirst() != nullptr;
    }

    T& operator*() const {
        return *ptr_.GetFirst();
    }

    T* operator->() const {
        return ptr_.GetFirst();
    }

    T& operator[](size_t ind) const {
        return ptr_.GetFirst()[ind];
    }

private:
    CompressedPair<T*, Deleter> ptr_;

    void TryDelete(T* ptr) {
        if (ptr != nullptr) {
            ptr_.GetSecond()(ptr);
        }
    }
};
