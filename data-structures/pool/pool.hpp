#ifndef POOL_HPP
#define POOL_HPP

#include <new>
#include <vector>
#include <iostream>
#include <memory>

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
	friend class						Object;

	struct Node {
        alignas(TType) unsigned char    data[sizeof(TType)];
        bool                            isFree = true;
    };

    std::vector<std::unique_ptr<Node>>					poolArena;


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
	/* if no slot is left pool doubles its size, based on first resize call's value.
		Note: Intial size = 1
	*/
    template<typename ... TArgs>
    Object acquire(TArgs&&... p_args) {
		/*
		- i gotta use constructor of Object class
		- i gotta use std::forward<TArgs>(p_args)
		- to pass args as they were passed to me i gotta use std::forward
		*/
		auto	freeNode = [&] () -> std::unique_ptr<Node>& {
			for (std::unique_ptr<Node>& node: this->poolArena) {
				if (node->isFree) {
					return node;
				}
			}
			int n = (this->poolArena.capacity() == 0) ? 1 : this->poolArena.capacity();
			this->resize(n * 2);
			return (this->poolArena[this->poolArena.size()]);
		};
		std::unique_ptr<Node>&	node = freeNode();
		TType*	userObj = new (node->data) TType(std::forward<TArgs>(p_args)...);
		return Object(node, userObj);
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
	/*
	- check not this object
	- nullptr objects pointers in this pool
	- assign objects of other to this
	*/
    Pool(Pool&& other){
	   this->poolArena = std::move(other.poolArena);
	   other.poolArena.erase(other.poolArena.begin(), other.poolArena.end());
    }
    Pool&   operator=(Pool&& other){
		if (this != &other) {
			this->poolArena = std::move(other.poolArena);
			other.poolArena.erase(other.poolArena.begin(), other.poolArena.end());
		}
        return *this;
    }
    ~Pool(){
	  std::cout << "Pool destructor called" << std::endl;
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
			friend class Pool<TType>;

			TType   *ptr;
			std::unique_ptr<Node>&	objNode;
			Object(std::unique_ptr<Node>& objNode, TType *ptr): ptr(ptr), objNode(objNode) {}

/* -------------------------------------------------------------------------- */
/*                           Object Public Members                            */
/* -------------------------------------------------------------------------- */
        public:
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
		   /*
		   - move operations would allow me to change ownership of an 
			   object between pool and user without destructing Pool memory
			   so it is ready for next object to acquire it 
		   */
            Object(Object&& other){
				this->objNode = std::move(other.objNode);
				this->ptr = std::move(other.ptr);
				other.objNode = nullptr;
				other.ptr = nullptr;
			}
            /*
            - move operations would allow me to change ownership of an 
                object between pool and user without destructing Pool memory
                so it is ready for next object to acquire it 
            */
            Object&   operator=(Object&& other){
				if (this != &other) {
					this->objNode = std::move(other.objNode);
					this->ptr = std::move(other.ptr);
					other.objNode = nullptr;
					other.ptr = nullptr;
				}
				return *this;
            }
			/*
			- i gotta called destructor of TType object explicitly (RAII)
			- nullptr the raw ptr of this object
			- i gotta update size of pool (decrease it)
			- release the slot
			*/
            ~Object(){
				this->ptr->~TType();
				this->objNode->isFree = true;
                std::cout << "Object destructor called" << std::endl;
            }
/* -------------------------------------------------------------------------- */
/*                            Operator Overloading                            */
/* -------------------------------------------------------------------------- */
            // Returns the pointer stored withing the Pool::Object
            TType* operator -> () {
                return this->ptr;
            }

    };
};


#endif