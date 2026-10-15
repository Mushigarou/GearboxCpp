#ifndef POOL_HPP
#define POOL_HPP

#include <new>
#include <vector>
#include <iostream>
#include <memory>

// class templates, nested classes, variadic templates, perfect forwarding, placement new, alignment, RAII and move semantics
template<typename TType>
class Pool {
/* -------------------------------------------------------------------------- */
/*                            Pool Private Members                            */
/* -------------------------------------------------------------------------- */
private:
	struct Node {
        alignas(TType) unsigned char    data[sizeof(TType)];
        bool                            isFree = true;
    };
    std::vector<std::unique_ptr<Node>>  poolArena;
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
        this->poolArena.resize(numberOfObjectStored);
        // int i = 0;
        for (auto& pool: this->poolArena) {
            if (pool == nullptr) {
                pool = std::make_unique<Node>();
                // i++;
            }
        }
        // std::cout << "-->" << i << this->poolArena[0]->isFree << std::endl;
    }
    
    // Creates a Pool::Object containing
    // a pre-allocated object, using the constructor with parameters
    // as defined by TArgs definition.
	/* if no slot is left pool doubles its size
	*/
    template<typename ... TArgs>
    Object acquire(TArgs&&... p_args) {
        std::unique_ptr<Node>* n = nullptr;
        // int i = 0;
        for (std::unique_ptr<Node>& node: this->poolArena) {
            if (node && node->isFree) {
                n = &node;
                node->isFree = false;
                break;
            }
            // i++;
        }
        // std::cout << i << std::endl;
        if (n == nullptr || ((*n)->data) == nullptr) {
            std::cerr  << __FILE__ << " " << __LINE__  << " : Error: not enough space. call resize." << std::endl;
            exit(2);
        }
        TType*	userObj = new ((*n)->data) TType(std::forward<TArgs>(p_args)...);
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
    Pool(Pool&& other) noexcept {
	   this->poolArena = std::move(other.poolArena);
	   other.poolArena.erase(other.poolArena.begin(), other.poolArena.end());
    }
    Pool&   operator=(Pool&& other) noexcept {
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
			Object(TType *ptr): ptr(ptr) {}

/* -------------------------------------------------------------------------- */
/*                           Object Public Members                            */
/* -------------------------------------------------------------------------- */
        public:
/* -------------------------------------------------------------------------- */
/*                            Object Rule Of Five                             */
/* -------------------------------------------------------------------------- */
            Object(Object&  ) = delete;
            Object&   operator=(Object& ) = delete;
            Object(Object&& other) noexcept {
				this->objNode = std::move(other.objNode);
				this->ptr = std::move(other.ptr);
				other.objNode = nullptr;
				other.ptr = nullptr;
			}
            Object&   operator=(Object&& other) noexcept {
				if (this != &other) {
					this->objNode = std::move(other.objNode);
					this->ptr = std::move(other.ptr);
					other.objNode = nullptr;
					other.ptr = nullptr;
				}
				return *this;
            }
            ~Object(){
				this->ptr->~TType();
				// this->objNode->isFree = true;
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