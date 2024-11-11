//
//  StrongTypeDefModules.h
//  TraffikTrak


#ifndef StrongTypeDefModules_h
#define StrongTypeDefModules_h

#include <iostream>
#include <utility>


/**
 *  \brief the collections of classes allow for the easy building of strong types
 *
 *  The base class defines constructors, move constructor, conversion operators, and a swap function
 *  The class only allows for explicit type conversion, and the only implicit method is the default constructor
 *  From there, several modules give the strong type additional properties if required, such as arithmetic operators,
 *  increment/decrement, and  comparability. To achieve this structure in a general sense while limiting the scope of names,
 *  the operators were implemented in their own classes using friend functions defined within those classes.
 *  however, since the assignment operator cannot use a friend function, assignability has to be manually written in a
 *  strong type subclass if it wants to be used. However, this functionality can be replicated by first casting the type
 *  to get a reference, and then assigning the value itself.
 *  Note: derived types will be editable through T& operator. while the swap function cannot be deleted, the convertion operators
 *  can be deleted. delete these when the intent is to have an immutable type. std::swap can be used on the entire object regardless
 *  of inherent immutablility of the type. Therefore, since there is only 1 member of the class, enabling a swap function that swaps
 *  just the values is simpler and doesn't expose the type any more than it already is.
 */

namespace StrongTypes {
    
    /** \class
     *  \brief provides the base of the strong type, wrapping the underlying value type and providing constructors, conversion operators, and swaps
     *
     */
    template <typename T>
    class StrongTypeDefBase {
        
    private:
        T value_;
        
    public:
        StrongTypeDefBase() : value_() { }
        explicit StrongTypeDefBase(const T& val) : value_(val) { }
        explicit StrongTypeDefBase(T&& val) noexcept : value_(std::move(val)) { }
        operator const T& () const noexcept { return value_; }
        T& ref() noexcept { return value_; }
        void swap(StrongTypeDefBase& other) {
            std::swap(this->ref(), other.ref());
        }
        template <class StrongType> friend class Inputtable;
        
    };
    
    
    /** \fn
     *  \brief function used to retrieve the underlying type of the strong type
     *
     */
    template <typename T>
    T get_underlying_type(StrongTypeDefBase<T>);
    
    
    /** \class
     *  \brief provides arithmetic addition operators + and +=
     *
     */
    template <class StrongType>
    class Addable {
        
    public:
        friend StrongType& operator += (StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return StrongType(a.ref() += static_cast<const type&>(b));
        }
        friend StrongType operator + (const StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return StrongType(static_cast<const type&>(a) + static_cast<const type&>(b));
        }
        
    };
    
    
    /** \class
     *  \brief provides arithmetic subtraction operators - and -=
     *
     */
    template <class StrongType>
    class Subtractable {
        
    public:
        friend StrongType& operator -= (StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return StrongType(a.ref() -= static_cast<const type&>(b));
        }
        friend StrongType operator - (const StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return StrongType(static_cast<const type&>(a) - static_cast<const type&>(b));
        }
        
    };
    
    
    /** \class
     *  \brief provides arithmetic multiplication operators * and *=
     *
     */
    template <class StrongType>
    class Multipliable {
        
    public:
        friend StrongType& operator *= (StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return StrongType(a.ref() *= static_cast<const type&>(b));
        }
        friend StrongType operator * (const StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return StrongType(static_cast<const type&>(a) * static_cast<const type&>(b));
        }
        
    };
    
    
    /** \class
     *  \brief provides arithmetic division operators / and /=
     *
     */
    template <class StrongType>
    class Dividable {
        
    public:
        friend StrongType& operator /= (StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return StrongType(a.ref() /= static_cast<const type&>(b));
        }
        friend StrongType operator / (const StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return StrongType(static_cast<const type&>(a) / static_cast<const type&>(b));
        }
        
    };
    
    
    /** \class
     *  \brief provides modulo operator %
     *
     */
    template <class StrongType>
    class Modable {
        
    public:
        friend StrongType operator % (const StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return StrongType(static_cast<const type&>(a) % static_cast<const type&>(b));
        }
        
    };
    
    
    /** \class
     *  \brief provides the increment operator ++
     *
     */
    template <class StrongType>
    class Incrementable {
        
    public:
        friend void operator ++ (StrongType& a) {
            a.ref()++;
        }
    
    };
    
    
    /** \class
     *  \brief provides the decrement operator --
     *
     */
    template <class StrongType>
    class Decrementable {
        
    public:
        friend void operator -- (StrongType& a) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            a.ref()--;
        }
        
    };
    
    
    /** \class
     *  \brief provides the compare operators <, <=, ==, !=, >=, >
     *
     */
    template <class StrongType>
    class Comparable {
        
    public:
        friend bool operator < (const StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return static_cast<const type&>(a) < static_cast<const type&>(b);
        }
        
        friend bool operator <= (const StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return static_cast<const type&>(a) <= static_cast<const type&>(b);
        }
        
        friend bool operator == (const StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return static_cast<const type&>(a) == static_cast<const type&>(b);
        }
        
        friend bool operator != (const StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return static_cast<const type&>(a) != static_cast<const type&>(b);
        }
        
        friend bool operator >= (const StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return static_cast<const type&>(a) >= static_cast<const type&>(b);
        }
        
        friend bool operator > (const StrongType& a, const StrongType& b) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            return static_cast<const type&>(a) > static_cast<const type&>(b);
        }
        
    };
    
    
    /** \class
     *  \brief provides the << operator
     *
     */
    template <class StrongType>
    class Outputtable {
        
    public:
        friend std::ostream& operator << (std::ostream& os, const StrongType& a) {
            using type = decltype(get_underlying_type(std::declval<StrongType>()));
            os << static_cast<const type&>(a);
            return os;
        }
        
    };
    
    /** \class
     *  \brief provides the >> operator
     *
     */
    template <class StrongType>
    class Inputtable {
        
    public:
        friend std::istream& operator >> (std::istream& is, StrongType& a) {
            is >> a.value_;
            return is;
        }
        
    };

    
}

#endif /* StrongTypeDefModules_h */
