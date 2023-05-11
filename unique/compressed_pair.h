#pragma once

#include <memory>
#include <type_traits>

template <typename T, bool = !std::is_empty_v<T> || std::is_final_v<T>>
class CompressedFirstElement {
    CompressedFirstElement() : element_(T{}) {
    }

    CompressedFirstElement(const T& element) : element_(element) {
    }

    CompressedFirstElement(T&& element) : element_(std::move(element)) {
    }

    const T& GetElement() const {
        return element_;
    }

    T& GetElement() {
        return element_;
    }

private:
    template <typename F, typename S>
    friend class CompressedPair;

    T element_;
};

template <typename T>
class CompressedFirstElement<T, false> : public T {
    CompressedFirstElement() : T(T{}) {
    }

    CompressedFirstElement(const T& element) : T(element) {
    }

    CompressedFirstElement(T&& element) : T(std::move(element)) {
    }

    const T& GetElement() const {
        return *this;
    }

    T& GetElement() {
        return *this;
    }

private:
    template <typename F, typename S>
    friend class CompressedPair;
};

template <typename T, bool = !std::is_empty_v<T> || std::is_final_v<T>>
class CompressedSecondElement {
    CompressedSecondElement() : element_(T{}) {
    }

    CompressedSecondElement(const T& element) : element_(element) {
    }

    CompressedSecondElement(T&& element) : element_(std::move(element)) {
    }

    const T& GetElement() const {
        return element_;
    }

    T& GetElement() {
        return element_;
    }

private:
    template <typename F, typename S>
    friend class CompressedPair;

    T element_;
};

template <typename T>
class CompressedSecondElement<T, false> : public T {
    CompressedSecondElement() : T(T{}) {
    }

    CompressedSecondElement(const T& element) : T(element) {
    }

    CompressedSecondElement(T&& element) : T(std::move(element)) {
    }

    const T& GetElement() const {
        return *this;
    }

    T& GetElement() {
        return *this;
    }

private:
    template <typename F, typename S>
    friend class CompressedPair;
};

template <typename F, typename S>
class CompressedPair : private CompressedFirstElement<F>, private CompressedSecondElement<S> {
public:
    CompressedPair() : CompressedFirstElement<F>(F{}), CompressedSecondElement<S>(S{}) {
    }

    template <typename V, typename U>
    CompressedPair(V&& first, U&& second)
        : CompressedFirstElement<F>(std::forward<V>(first)),
          CompressedSecondElement<S>(std::forward<U>(second)) {
    }

    F& GetFirst() {
        return CompressedFirstElement<F>::GetElement();
    }

    const F& GetFirst() const {
        return CompressedFirstElement<F>::GetElement();
    }

    S& GetSecond() {
        return CompressedSecondElement<S>::GetElement();
    }

    const S& GetSecond() const {
        return CompressedSecondElement<S>::GetElement();
    };
};
