#include <iostream>
#include <string>
#include <vector>

template<typename valueType>
class Foo {
private:
    valueType val;

public:
    Foo(const valueType &val) {
        this->val = val;
    }

    Foo() {}

    Foo(const Foo &rhs) {
        this->val = rhs.val;
    }

    Foo&    operator=(const Foo &rhs) {
        if (this != &rhs) {
            this->val = rhs.val;
        }
        return *this;
    }

    const valueType&   getValue() const {
        return  val;
    }
    
    ~Foo() {}

    friend std::ostream&    operator<<(std::ostream& os, const Foo<valueType> &rhs) {
        const auto& value = rhs.getValue();
    
        for (const auto &num : value) {
            os << num;
        }
        return os;
    }
};

// template <typename valueType>


int main() {
    std::vector<int> arr(10, 7);
    std::string str = "help";
    int num = 1;

    Foo<std::vector<int>> obj = Foo(arr);
    Foo<std::string> FooStr = Foo(str);
    Foo<int> Fooint = Foo(num);

    std::cout << obj;
    std::cout << std::endl;
    std::cout << FooStr;
    std::cout << std::endl;
    std::cout << Fooint;

    return 0;
}