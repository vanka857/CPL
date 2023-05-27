#include <csetjmp>
#include <iostream>
#include <string>
 

enum error 
{
    OK = 0,
    IO_ERROR = 1,
    MATH_ERROR = 2,
    //...
};

std::jmp_buf my_jump_buffer;

#define TRY                                         \
{                                                   \
        switch (setjmp(my_jump_buffer)) {           \
            case error::OK: { 


#define CATCH(exception)                            \
                break;                              \
            }                                       \
            case exception: {                       \


#define THROW(exception)                            \
            longjmp(my_jump_buffer, exception);


#define END_TRY                                     \
            }                                       \
        }                                           \
    }


class MyClass {
private:
    std::string m_name;
public:
    explicit MyClass(std::string name) {
        m_name = name;
        std::cout << "MyClass constructor called for " << m_name << std::endl;
    }

    ~MyClass() {
        std::cout << "MyClass destructor called for " << m_name << std::endl;
    }
};


int c(int num, int denum) {
    MyClass c_1("c_1");
    if (denum == 0) {
        MyClass c_2("c_2");
        THROW(error::MATH_ERROR);
        MyClass c_3("c_3");
    }
    return num / denum;
}

void b() {
    TRY {
        std::cout << "TRY B" << std::endl;        
        
        MyClass b_1("b_1");
        c(0, 0);
        MyClass b_2("b_2");
    } 
    CATCH(error::MATH_ERROR) {
        MyClass b_3("b_3");
        std::cout << "CATCH B MATH_ERROR" << std::endl;
        THROW(error::MATH_ERROR);
        MyClass b_4("b_4");
    } END_TRY
}

void a() {
    MyClass a_1("a_1");

    TRY {
        std::cout << "TRY A" << std::endl;        
        
        MyClass a_2("a_2");
        b();
        MyClass a_3("a_3");
    } 
    CATCH(error::IO_ERROR) {
        std::cout << "CATCH A IO_ERROR" << std::endl;
        MyClass a_4("a_4");
    } 
    CATCH(error::MATH_ERROR) {
        std::cout << "CATCH A MATH_ERROR" << std::endl;
        MyClass a_5("a_5");
    } END_TRY
}

int main() {
    a();
    return 0;
}
