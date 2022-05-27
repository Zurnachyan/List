#ifndef __List__
#define __List__

#include <iostream>
#include <iterator>
#include <vector>

namespace LAZ {
    template<typename T>
    class List {
    private:
        struct Node {
        public:
            Node() : _next{nullptr}, _prev{nullptr}, _value{} {}
            Node(const T& elem) : _value{elem}, _next{nullptr}, _prev{nullptr} {}
        public:
            Node* _next;
            Node* _prev;
            T _value;
        };
    public:
        class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
        public:
            Iterator() : _iter{nullptr} {}
            Iterator(Node* iter) : _iter{iter} {}
            Iterator(const Iterator& oth) : _iter{oth._iter} {}
        public:
            Iterator& operator=(const Iterator& oth) { _iter = oth._iter; return *this; }
            Iterator& operator=(Node* iter) { _iter = iter; return *this; }
            T& operator*() { return _iter->_value; }
            T* operator->() { return &(_iter->_value); }
            bool operator==(const Iterator& rhs) { return (_iter == rhs._iter); }
            bool operator!=(const Iterator& rhs) { return (_iter != rhs._iter); }
            Iterator& operator++() { _iter = _iter->_next; return *this; }
            Iterator& operator--() { _iter = _iter->_prev; return *this; }
            Iterator operator++(int) { Iterator tmp(*this); _iter = _iter->_next; return tmp; }
            Iterator operator--(int) { Iterator tmp(*this); _iter = _iter->_prev; return tmp; }
            Node* getIter() const { return _iter; }
        private:
            Node* _iter;
        };
    public:
        ~List() { clear(); }
        List() : _head{nullptr} {}
        List(int n);
        List(int n, T value);
        List(std::initializer_list<T> list);
        List(const List<T>& oth);
        List(List<T>&& oth);
        template<typename U>
        List(const List<U>& oth);
    public:
        List<T>& operator=(const List<T>& rhs);
        List<T>& operator=(List<T>&& rhs);
        template<typename U>
        List<T>& operator=(const List<U>& rhs);
        bool operator==(const List<T>& rhs);
        bool operator!=(const List<T>& rhs);
        bool operator<(const List<T>& rhs);
        bool operator<=(const List<T>& rhs);
        bool operator>(const List<T>& rhs);
        bool operator>=(const List<T>& rhs);
    public:
        bool empty() const { return (_head == nullptr); }
        int size() const;
        void swap(List<T>& ob2);
        void assign(int n, const T& value);
        void assign(std::initializer_list<T> list);
        template<typename Iter>
        void assign(Iter begin, Iter end);
        void pushBack(const T& value);
        void pushFront(const T& value);
        void popBack();
        void popFront();
        Iterator insert(Iterator pos, const T& value);
        template<typename Iter>
        Iterator insert(Iterator pos, Iter begin, Iter end);
        Iterator insert(Iterator pos, int n, const T& value);
        Iterator insert(Iterator pos, std::initializer_list<T> list);
        template<typename... Args>
        Iterator emplace(Iterator pos, Args... args);
        template<typename... Args>
        void emplaceBack(Args... args);
        template<typename... Args>
        void emplaceFront(Args... args);
        List<T>::Iterator erase(Iterator pos);
        void remove(const T& value);
        template<typename Operation>
        void removeIf(Operation op);
        void clear();
        void unique();
        template<typename Operation>
        void unique(Operation op);
        void resize(int n);
        void resize(int n, const T& value);
    public:
        Iterator begin() const { return _begin; }
        Iterator end() const { return _end; }
    private:
        Node* _head;
        Iterator _begin;
        Iterator _end;
    };

    // CONSTRUCTORS
    template<typename T>
    List<T>::List(int n) {
        _head = new Node();
        Node* tmp = _head;
        for(int i = 0; i < n - 1; ++i) {
            tmp->_next = new Node();
            tmp->_next->_prev = tmp;
            tmp = tmp->_next;
        }
        _begin = _head;
        tmp->_next = new Node();
        tmp->_next->_prev = tmp;
        _end = tmp->_next;
    }

    template<typename T>
    List<T>::List(int n, T value) {
        _head = new Node(value);
        Node* tmp = _head;
        for(int i = 0; i < n - 1; ++i) {
            tmp->_next = new Node(value);
            tmp->_next->_prev = tmp;
            tmp = tmp->_next;
        }
        _begin = _head;
        tmp->_next = new Node();
        tmp->_next->_prev = tmp;
        _end = tmp->_next;
    }

    template<typename T>
    List<T>::List(std::initializer_list<T> list) {
        auto j = list.begin();
        _head = new Node(*j);
        ++j;
        Node* tmp = _head;
        for(int i = 1; i < list.size(); ++i, ++j) {
            tmp->_next = new Node(*j);
            tmp->_next->_prev = tmp;
            tmp = tmp->_next;
        }
        _begin = _head;
        tmp->_next = new Node();
        tmp->_next->_prev = tmp;
        _end = tmp->_next;
    }

    template<typename T>
    List<T>::List(const List<T>& oth) {
        Node* tmp = oth._head;
        _head = new Node(tmp->_value);
        Node* tmp1 = _head;
        while(tmp->_next != nullptr) {
            tmp = tmp->_next;
            tmp1->_next = new Node(tmp->_value);
            tmp->_next->_prev = tmp;
            tmp1 = tmp1->_next;
        }
        _begin = _head;
        _end = tmp1;
    }

    template<typename T>
    List<T>::List(List<T>&& oth) {
        _head = oth._head;
        _begin = oth._begin;
        _end = oth._end;
        oth._head = nullptr;
        oth._begin = nullptr;
        oth._end = nullptr;
    }

    template<typename T>
    template<typename U>
    List<T>::List(const List<U>& oth) {
        auto it = oth.begin();
        _head = new Node(*it);
        Node* tmp = _head;
        while(++it != oth.end()) {
            tmp->_next = new Node(*it);
            tmp->_next->_prev = tmp;
            tmp = tmp->_next;
        }
        _begin = _head;
        tmp->_next = new Node();
        tmp->_next->_prev = tmp;
        _end = tmp->_next;
    }

    // OPERATORS
    template<typename T>
    List<T>& List<T>::operator=(const List<T>& rhs) {
        if(this == &rhs) {
            return *this;
        }
        clear();
        _head = new Node(rhs._head->_value);
        Node* tmp = rhs._head;
        Node* tmp1 = _head;
        while(tmp->_next != nullptr) {
            tmp = tmp->_next;
            tmp1->_next = new Node(tmp->_value);
            tmp->_next->_prev = tmp;
            tmp1 = tmp1->_next;
        }
        _begin = _head;
        _end = tmp1;
        return *this;
    }

    template<typename T>
    List<T>& List<T>::operator=(List<T>&& rhs) {
        _head = rhs._head;
        _begin = rhs._begin;
        _end = rhs._end;
        rhs._head = nullptr;
        rhs._begin = nullptr;
        rhs._end = nullptr;
        return *this;
    }

    template<typename T>
    template<typename U>
    List<T>& List<T>::operator=(const List<U>& rhs) {
        clear();
        auto it = rhs.begin();
        _head = new Node(*it);
        Node* tmp = _head;
        while(++it != rhs.end()) {
            tmp->_next = new Node(*it);
            tmp->_next->_prev = tmp;
            tmp = tmp->_next;
        }
        _begin = _head;
        tmp->_next = new Node();
        tmp->_next->_prev = tmp;
        _end = tmp->_next;
        return *this;
    }

    template<typename T>
    bool List<T>::operator==(const List<T>& rhs) {
        auto it = _begin;
        auto iter = rhs.begin();
        while(it != _end) {
            if(*it != *iter) {
                return false;
            }
            ++it;
            ++iter;
        }
        return true;
    }

    template<typename T>
    bool List<T>::operator!=(const List<T>& rhs) {
        return !(*this == rhs);
    }

    template<typename T>
    bool List<T>::operator<(const List<T>& rhs) {
        return std::lexicographical_compare(_begin, _end, rhs._begin, rhs._end);
    }

    template<typename T>
    bool List<T>::operator<=(const List<T>& rhs) {
        return !(rhs < *this);
    }

    template<typename T>
    bool List<T>::operator>(const List<T>& rhs) {
        return (rhs < *this);
    }

    template<typename T>
    bool List<T>::operator>=(const List<T>& rhs) {
        return !(*this < rhs);
    }

    // FUNCTIONS
    template<typename T>
    int List<T>::size() const {
        if(empty()) {
            return 0;
        }
        int size{};
        auto it = _begin;
        while(it++ != _end) {
            ++size;
        }
        return size;
    }

    template<typename T>
    void List<T>::swap(List<T>& ob2) {
        Node* tmp = _head;
        _head = ob2._head;
        ob2._head = tmp;
        Iterator it = _begin;
        _begin = ob2._begin;
        ob2._begin = it;
        it = _end;
        _end = ob2._end;
        ob2._end = it;
    }
        
    template<typename T>
    void List<T>::assign(int n, const T& value) {
        clear();
        _head = new Node(value);
        Node* tmp = _head;
        for(int i = 0; i < n - 1; ++i) {
            tmp->_next = new Node(value);
            tmp->_next->_prev = tmp;
            tmp = tmp->_next;
        }
        _begin = _head;
        tmp->_next = new Node();
        tmp->_next->_prev = tmp;
        _end = tmp->_next;
    }

    template<typename T>
    void List<T>::assign(std::initializer_list<T> list) {
        clear();
        auto j = list.begin();
        _head = new Node(*j);
        ++j;
        Node* tmp = _head;
        for(int i = 1; j != list.end(); ++i, ++j) {
            tmp->_next = new Node(*j);
            tmp->_next->_prev = tmp;
            tmp = tmp->_next;
        }
        _begin = _head;
        tmp->_next = new Node();
        tmp->_next->_prev = tmp;
        _end = tmp->_next;
    }

    template<typename T>
    template<typename Iter>
    void List<T>::assign(Iter begin, Iter end) {
        clear();
        _head = new Node(*begin);
        Node* tmp = _head;
        ++begin;
        while(begin != end) {
            tmp->_next = new Node(*begin);
            tmp->_next->_prev = tmp;
            tmp = tmp->_next;
            ++begin;
        }
        _begin = _head;
        tmp->_next = new Node();
        tmp->_next->_prev = tmp;
        _end = tmp->_next;
    }
        
    template<typename T>    
    void List<T>::pushBack(const T& value) {
        if(empty()) {
            _head = new Node(value);
            _head->_next = new Node();
            _begin = _head;
            _end = _head->_next;
        } else {
            Node* tmp = _end.getIter();
            tmp->_value = value;
            tmp->_next = new Node();
            tmp->_next->_prev = tmp;
            _end = tmp->_next;
        }
    }

    template<typename T>
    void List<T>::pushFront(const T& value) {
        if(empty()) {
            _head = new Node(value);
            _head->_next = new Node();
            _begin = _head;
            _end = _head->_next;
        } else {
            _head->_prev = new Node(value);
            Node* tmp = _head;
            _head = _head->_prev;
            _head->_next = tmp;
            _begin = _head;
        }
    }

    template<typename T>
    void List<T>::popBack() {
        if(!empty()) {
            Node* tmp = _end.getIter()->_prev;
            delete _end.getIter();
            tmp->_value = {};
            tmp->_next = nullptr;
            _end = tmp;
        }
    }

    template<typename T>
    void List<T>::popFront() {
        if(!empty()) {
            Node* tmp = _head;
            _head = _head->_next;
            _begin = _head;
            _head->_prev = nullptr;
            tmp->_value = {};
            delete tmp;
        }
    }

    template<typename T>
    typename List<T>::Iterator List<T>::insert(Iterator pos, const T& value) {
        Node* tmp;
        if(pos.getIter()->_prev != nullptr) {
            tmp = pos.getIter()->_prev;
            tmp->_next = new Node(value);
            tmp->_next->_prev = tmp;
            tmp->_next->_next = pos.getIter();
            pos.getIter()->_prev = tmp->_next;
        } else {
            pos.getIter()->_prev = new Node(value);
            tmp = pos.getIter()->_prev;
            tmp->_next = pos.getIter();
            _head = tmp;
            _begin = _head;
        }
        return Iterator(tmp->_next);
    }

    template<typename T>
    template<typename Iter>
    typename List<T>::Iterator List<T>::insert(Iterator pos, Iter begin, Iter end) {
        Node* tmp = new Node(*begin);
        Node* tmp1 = tmp;
        ++begin;
        while(begin != end) {
            tmp->_next = new Node(*begin);
            tmp->_next->_prev = tmp;
            tmp = tmp->_next;
            ++begin;
        }
        if(pos.getIter()->_prev != nullptr) {
            Node* a = pos.getIter()->_prev;
            a->_next = tmp1;
            tmp1->_prev = a;
            pos.getIter()->_prev = tmp;
            tmp->_next = pos.getIter();
        } else {
            pos.getIter()->_prev = tmp;
            tmp->_next = pos.getIter();
            _head = tmp1;
            _begin = _head;
        }
        return Iterator(tmp1);
    }

    template<typename T>
    typename List<T>::Iterator List<T>::insert(Iterator pos, int n, const T& value) {
        Node* tmp = new Node(value);
        Node* tmp1 = tmp;
        for(int i = 1; i < n; ++i) {
            tmp->_next = new Node(value);
            tmp->_next->_prev = tmp;
            tmp = tmp->_next;
        }
        if(pos.getIter()->_prev != nullptr) {
            Node* a = pos.getIter()->_prev;
            a->_next = tmp1;
            tmp1->_prev = a;
            pos.getIter()->_prev = tmp;
            tmp->_next = pos.getIter();
        } else {
            pos.getIter()->_prev = tmp;
            tmp->_next = pos.getIter();
            _head = tmp1;
            _begin = _head;
        }
        return Iterator(tmp1);
    }

    template<typename T>
    typename List<T>::Iterator List<T>::insert(Iterator pos, std::initializer_list<T> list) {
        auto begin = list.begin();
        Node* tmp = new Node(*begin);
        Node* tmp1 = tmp;
        ++begin;
        while(begin != list.end()) {
            tmp->_next = new Node(*begin);
            tmp->_next->_prev = tmp;
            tmp = tmp->_next;
            ++begin;
        }
        if(pos.getIter()->_prev != nullptr) {
            Node* a = pos.getIter()->_prev;
            a->_next = tmp1;
            tmp1->_prev = a;
            pos.getIter()->_prev = tmp;
            tmp->_next = pos.getIter();
        } else {
            pos.getIter()->_prev = tmp;
            tmp->_next = pos.getIter();
            _head = tmp1;
            _begin = _head;
        }
        return Iterator(tmp1);
    }

    template<typename T>
    template<typename... Args>
    void List<T>::emplaceBack(Args... args) {
        pushBack(T(args...));
    }

    template<typename T>
    template<typename... Args>
    void List<T>::emplaceFront(Args... args) {
        pushFront(T(args...));
    }

    template<typename T>
    template<typename... Args>
    typename List<T>::Iterator List<T>::emplace(Iterator pos, Args... args) {
        return insert(pos, T(args...));
    }

    template<typename T>
    typename List<T>::Iterator List<T>::erase(Iterator pos) {
        Node* tmp1 = pos.getIter()->_prev;
        Node* tmp2 = pos.getIter()->_next;
        delete pos.getIter();
        if(tmp1 != nullptr) {
            tmp1->_next = tmp2;
            tmp2->_prev = tmp1;
            return Iterator(tmp1);
        } else {
            _head = tmp2;
            _begin = _head;
            return _begin;
        }
    }

    template<typename T>
    void List<T>::remove(const T& value) {
        while(*_begin == value) {
            popFront();
        }
        Iterator tmp = _begin;
        ++tmp;
        while(tmp != _end) {
            if(*tmp == value) {
                tmp = erase(tmp);
            }
            ++tmp;
        }
    }

    template<typename T>
    template<typename Operation>
    void List<T>::removeIf(Operation op) {
        while(op(*_begin)) {
            popFront();
        }
        Iterator tmp = _begin;
        ++tmp;
        while(tmp != _end) {
            if(op(*tmp)) {
                tmp = erase(tmp);
            }
            ++tmp;
        }
    }

    template<typename T>
    void List<T>::clear() {
        if(_head != nullptr) {
            Node* tmp = _head->_next;
            delete _head;
            _head = nullptr;
            while(tmp->_next != nullptr) {
                _head = tmp;
                tmp = tmp->_next;
                delete _head;
                _head = nullptr;
            }
        }
        _begin = nullptr;
        _end = nullptr;
    }

    template<typename T>
    void List<T>::unique() {
        List<T> ls;
        int size1 = size();
        auto first = _begin;
        while(first != _end) {
            auto after = first;
            ++after;
            while(after != _end) {
                if(*first == *after) {
                    after = erase(after);
                    ++after;
                } else {
                    ++after;
                }
            }
            ++first;
        }
    }

    template<typename T>
    template<typename Operation>
    void List<T>::unique(Operation op) {
        List<T> ls;
        int size1 = size();
        auto first = _begin;
        while(first != _end) {
            auto after = first;
            ++after;
            while(after != _end) {
                if(op(*first, *after)) {
                    after = erase(after);
                    ++after;
                } else {
                    ++after;
                }
            }
            ++first;
        }
    }

    template<typename T>
    void List<T>::resize(int n) {
        int size1 = size();
        while(size1 > n) {
            popBack();
            ++n;
        }
        if(size1 == n) {
            return;
        }
        while(size1 < n) {
            pushBack(T());
            --n;
        }
    }

    template<typename T>
    void List<T>::resize(int n, const T& value) {
        int size1 = size();
        while(size1 > n) {
            popBack();
            ++n;
        }
        if(size1 == n) {
            return;
        }
        while(size1 < n) {
            pushBack(value);
            --n;
        }
    }
};

#endif