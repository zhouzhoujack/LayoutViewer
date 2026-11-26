 #pragma once

#include <map>
#include <string>

namespace my {

struct RegistryKey {
    int priority;
    std::string name;

    bool operator<(const RegistryKey& other) const {
        if (priority != other.priority)
            return priority < other.priority; 
        return name < other.name;            
    }
};

template <typename T>
class RegisteredClass {
public:
    using const_iterator = typename std::map<RegistryKey, T*>::const_iterator;

    class iterator {
    public:
        using InnerIter = const_iterator;

        explicit iterator(InnerIter it) : it_(it) {}

        T& operator*() const { return *(it_->second); }

        T* operator->() const { return it_->second; }

        const std::string& current_name() const { return it_->first.name; }

        int current_position() const { return it_->first.priority; }

        iterator& operator++() { ++it_; return *this; }
        iterator  operator++(int) { auto t = *this; ++it_; return t; }

        bool operator==(const iterator& o) const { return it_ == o.it_; }
        bool operator!=(const iterator& o) const { return it_ != o.it_; }

    private:
        InnerIter it_;  
    };

    static RegisteredClass& instance() {
        static RegisteredClass inst;
        return inst;
    }

    bool add(T* obj, int priority, const std::string& name) {
        RegistryKey key{priority, name};
        auto result = map_.emplace(key, obj);
        return result.second;
    }

    void remove(const RegistryKey& key) {
        map_.erase(key);
    }

    iterator begin() const {
        return iterator(map_.begin());
    }

    iterator end() const {
        return iterator(map_.end());
    }

private:
    std::map<RegistryKey, T*> map_;
};


template <typename T>
class Registrar {
public:
    Registrar(T* obj, int priority, const std::string& name, bool owned = true)
        : m_key{priority, name},
          mp_obj(obj),
          m_owned(owned),
          m_registered(false) {
        m_registered = RegisteredClass<T>::instance().add(obj, priority, name);
    }

    ~Registrar() {
        if (m_registered) {
            RegisteredClass<T>::instance().remove(m_key);
        }
        if (m_owned) {
            delete mp_obj;
        }
    }

    static typename RegisteredClass<T>::iterator begin(){
        return RegisteredClass<T>::instance().begin();
    }

    static typename RegisteredClass<T>::iterator end(){
        return RegisteredClass<T>::instance().end();
    }

private:
    Registrar(const Registrar&) = delete;
    Registrar& operator=(const Registrar&) = delete;

    RegistryKey m_key;
    T* mp_obj;
    bool m_owned;
    bool m_registered;
};

}  // namespace my
