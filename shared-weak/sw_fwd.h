#pragma once

#include <algorithm>
#include <exception>
#include <new>

// Instead of std::bad_weak_ptr
class BadWeakPtr : public std::exception {};

class ESFTBase {};

class ControlBlock {
public:
    virtual size_t GetUseCount() const = 0;

    virtual void IncreaseUsage() = 0;
    virtual void DecreaseUsage() = 0;

    virtual void IncreaseWeakUsage() = 0;
    virtual void DecreaseWeakUsage() = 0;
};

template <typename T>
class BlockWithPointer final : public ControlBlock {
public:
    BlockWithPointer(T* object) : use_count_(1), weak_count_(0), object_(object) {
    }

    size_t GetUseCount() const {
        return use_count_;
    }

    void IncreaseUsage() {
        ++use_count_;
    }

    void DecreaseUsage() {
        if (--use_count_ == 0) {
            if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
                if (weak_count_ == 1) {
                    object_->weak_this_.UnsafeReset();
                    delete object_;
                    delete this;
                    return;
                }
            }
            delete object_;
        }
        if (use_count_ + weak_count_ == 0) {
            delete this;
        }
    }

    void IncreaseWeakUsage() {
        ++weak_count_;
    }

    void DecreaseWeakUsage() {
        if (use_count_ + --weak_count_ == 0) {
            delete this;
        }
    }

private:
    size_t use_count_;
    size_t weak_count_;
    T* object_;
};

template <typename T>
class BlockWithObject final : public ControlBlock {
public:
    template <typename... Args>
    BlockWithObject(Args&&... args) : use_count_(1), weak_count_(0) {
        new (&buffer_) T(std::forward<Args>(args)...);
    }

    size_t GetUseCount() const {
        return use_count_;
    }

    void IncreaseUsage() {
        ++use_count_;
    }

    void DecreaseUsage() {
        if (--use_count_ == 0) {
            if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
                if (weak_count_ == 1) {
                    GetPointer()->weak_this_.UnsafeReset();
                    GetPointer()->~T();
                    delete this;
                    return;
                }
            }
            GetPointer()->~T();
        }
        if (use_count_ + weak_count_ == 0) {
            delete this;
        }
    }

    void IncreaseWeakUsage() {
        ++weak_count_;
    }

    void DecreaseWeakUsage() {
        if (use_count_ + --weak_count_ == 0) {
            delete this;
        }
    }

    T* GetPointer() {
        return reinterpret_cast<T*>(&buffer_);
    }

private:
    size_t use_count_;
    size_t weak_count_;
    std::aligned_storage_t<sizeof(T), alignof(T)> buffer_;
};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;
