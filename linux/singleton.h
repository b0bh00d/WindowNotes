#pragma once

#define DeclareSingleton(Type) template <> Type * Singleton<Type>::singletonPtr = nullptr

template<class Type>
class Singleton
{
protected:
    // must be set to 0 prior to creating the object
    static Type *singletonPtr;

public:
    Singleton () { singletonPtr = (Type *) this; }
    ~Singleton () {}

    static Type *instance () { return singletonPtr; }
};

// template<class Type>
// Type *Singleton<Type>::singletonPtr = nullptr;
