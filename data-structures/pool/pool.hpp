#ifndef POOL_HPP
#define POOL_HPP

#include "resize.cpp"
#include <new>
#include <vector>
#include <iostream>


// class templates, nested classes, variadic templates, perfect forwarding, placement new, alignment, RAII and move semantics

/*
- the Pool class is not templated, for ease of calling it 
    without type definitions
- the underlying object class would hold all complexity, and things
    that user should not touch
- the pool is like a wrapper around pool::object it provides to user
    API to resize and acquire from pool
- when Pool goes out of scope (RAII) all pool::object gotta call destructor
    of all pool::object objects
*/
template<typename TType>
class Pool {
/* -------------------------------------------------------------------------- */
/*                            Pool Private Members                            */
/* -------------------------------------------------------------------------- */
private:
    /*
    - this vector may actually be the pool, and number of object in it
    and resize method would call it to resize it
    */
    struct Node {
        alignas(TType) unsigned char    data[sizeof(TType)];
        bool                            isFree;
    };

    std::vector<Node> poolArena;


/* -------------------------------------------------------------------------- */
/*                            Pool Public Members                             */
/* -------------------------------------------------------------------------- */
public:
    class   Object;

/* -------------------------------------------------------------------------- */
/*                             Resize && Acquire                              */
/* -------------------------------------------------------------------------- */
    // Allocates a certain number of TType objects withing the Pool.
    void    resize(const size_t& numberOfObjectStored) {
        if (numberOfObjectStored > this->poolArena.max_size()) {
            std::cerr << "Error: " << __FILE__ << __LINE__ <<
            ": Requested size " << numberOfObjectStored 
            << " exceeds maximum allowable container size.\n";
            return;
        }
        this->poolArena.reserve(numberOfObjectStored);
    }
    
    // Creates a Pool::Object containing
    // a pre-allocated object, using the constructor with parameters
    // as defined by TArgs definition.
	/* if no slot is left pool doubles its size, based on first resize call's value */
    template<typename ... TArgs>
    Object acquire(TArgs&&... p_args) {
		/*
		- i gotta use constructor of Object class
		- i gotta use std::forward<TArgs>(p_args)
		- to pass args as they were passed to me i gotta use std::forward
		*/
		auto	freeNode = [&] () -> Node& {
			for (Node& node: this->poolArena) {
				if (node.isFree) {
					return node;
				}
			}
			this->resize(this->poolArena.capacity() * 2);
			return this->poolArena[this->poolArena.size()];
		};
		Node&	node = freeNode();
		TType*	userObj = new (node.data) TType(std::forward<TArgs>(p_args)...);
		return Object(userObj);
    }

/* -------------------------------------------------------------------------- */
/*                             Pool Rule Of Five                              */
/* -------------------------------------------------------------------------- */

    Pool(){}
    Pool(size_t numberOfObjectStored) {
        resize(numberOfObjectStored);
    }
    Pool(Pool&  other) = delete;
    Pool&   operator=(Pool& other) = delete;
    Pool(Pool&& other){
        /*
        - check not this object
        - nullptr objects pointers in this pool
        - assign objects of other to this
        */
	   this->poolArena = other.poolArena;
	   other.poolArena.erase(other.poolArena.begin(), other.poolArena.end());
    }
    Pool&   operator=(Pool&& other){
		if (this != &other) {
			this->poolArena = other.poolArena;
			other.poolArena.erase(other.poolArena.begin(), other.poolArena.end());
		}
        return *this;
    }
    ~Pool(){
		// for (Node& node: this->poolArena) {
			// this->poolArena.~vector();
		// }
        std::cout << "Pool destructor called" << std::endl;
        /*
        -  dealocate all memory allocated fo TType object
        */
    }

/* -------------------------------------------------------------------------- */
/*                                Object Class                                */
/* -------------------------------------------------------------------------- */

    /*
    - this is the actual wrapper around TType object being passed to Pool
        which Pool itself does pass it to Object
    */
    class Object {
/* -------------------------------------------------------------------------- */
/*                           Object Private Members                           */
/* -------------------------------------------------------------------------- */
        private:
            /*
            - this can be a smart pointer, like a unique_ptr to automatically
                managing it's dealocation, but what if user wanna share same 
                object with different pointers, in that case we gotta use
                shared_ptr
            */
            friend class Pool<TType>;

            TType   *ptr;
           
        //    template<typename ... TArgs>
           Object(TType *obj): ptr(obj) {
			
           /*
           - i gotta build the object in the pre-allocated
               memory, for that i gotta use new placement syntax would
               look like: this->ptr = new (buffer) TType(Args...) 
           - cause object passed as arg is alread
           - i gotta use std::forward<TArgs>(p_args)
           - i gotta update size of pool (increase it)
           */
           }

/* -------------------------------------------------------------------------- */
/*                           Object Public Members                            */
/* -------------------------------------------------------------------------- */
        public:
/* -------------------------------------------------------------------------- */
/*                            Operator Overloading                            */
/* -------------------------------------------------------------------------- */
            // Returns the pointer stored withing the Pool::Object
            TType* operator -> () {
                return this->ptr;
            }

/* -------------------------------------------------------------------------- */
/*                            Object Rule Of Five                             */
/* -------------------------------------------------------------------------- */
            /*
            - the copy assignement should not be allowed to be called
                because in destructor we call TType object destructor 
            */
            Object(Object&  ) = delete;
            Object&   operator=(Object& ) = delete;
            /*
            - Every requests and releases of a pre-allocated objects must be
                handled by Pool::Object, not by the user 
            */
            Object(Object&& ){
            /*
            - move operations would allow me to change ownership of an 
                object between pool and user without destructing Pool memory
                so it is ready for next object to acquire it 
            */
            }
            Object&   operator=(Object&& ){
            /*
            - move operations would allow me to change ownership of an 
                object between pool and user without destructing Pool memory
                so it is ready for next object to acquire it 
            */
                return *this;
            }
            ~Object(){
                std::cout << "Object destructor called" << std::endl;
				this->ptr->~TType();
            /*
            - i gotta called destructor of TType object explicitly (RAII)
            - nullptr the raw ptr of this object
            - i gotta update size of pool (decrease it)
            - release the slot
            */
            }
    };
};


#endif