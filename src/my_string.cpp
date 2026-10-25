// ============================================================================
// 作业 2：String 类的实现文件
//
// 目前本文件是空的：构建时链接阶段会报 "undefined reference to `String::...`"，
// 这是预期现象。请先到 include/my_string.h 中补好私有数据成员，再在这里实现
// 所有声明过的成员函数与运算符。
//
// 如果你想拆成多个 .cpp 文件，请同步修改根目录 CMakeLists.txt 中的
// STRING_SOURCES 列表。
//
// 实现清单（与 include/my_string.h 一一对应）：
//   [ ] String() / String(const char*) / 拷贝构造 / 移动构造 / 析构
//   [ ] 复制赋值 operator=(const String&) / 移动赋值 operator=(String&&)
//   [ ] operator+ / operator[]（含 const 版本）/ at（含 const 版本）
//   [ ] size / capacity
//   [ ] insert / push_back
//   [ ] c_str / operator const char*
//   [ ] swap
//   [ ] friend operator<< / operator>>
//
// 完成后按 docs/build-and-test.md 的步骤构建、运行测试并做 ASan/UBSan 检查。
// ============================================================================

#include "../include/my_string.h"
#include <stdexcept>
#include <utility>
 namespace {
    constexpr std::size_t kMinCapacity = 16;//无符号整数，专门用来表述大小，长度，或者数组下标的整数
    //const常量  expr（expression.表达式），作用是：告诉编译器，这东西在代码还没运行时，你就提前把结果算出来，当成一个死的数写在程序里。
    //和const的区别：const只保证运行时不修改，而constexpr强制要求在编译阶段就把结果算出来，必须在编译过程就有确定的结果。
    //用它的好处：1，提升性能，直接在代码里替换成了数字，运行时不需要去读内存查这个变量，速度更快。2，安全，如果编译器算不出来，他会直接报错防止你写出用未知变量去开数组大小的危险代码
    //3，在数组中中括号内必须要用编译期常量

    std::size_t c_str_length(const char* s){
        std::size_t len = 0 ;
        while (s && s[len] != '\0')//在C++中，指针用于布尔语境时会发生隐式转换。
        //如果s是空指针（nullptr），他就会被转化为false；
        // 如果s指向有效内存，不是空指针，他就会被转化为true
        //空指针是地址不存在，没有这个地方，指向内存为空的内存的地址的指针不算空指针
        //这样的写法避免了空指针和空内存。
        //while的括号内是条件判断，属于布尔语境+++
        {
            ++len;
        }
        return len ;
        }

        std::size_t growth_target(std::size_t current, std::size_t needed){
            std::size_t target = current < kMinCapacity ? kMinCapacity : current;//三目运算符：如果问号前面的条件成立，就选前者，否则后者。
            while (target < needed) target *= 2;
            return target;
    }
 } // 匿名命名空间只能够在本文件中使用，避免与其他文件的同名函数冲突
 //匿名空间只写在cpp文件中，不能写在头文件中。
 //为什么要使用匿名命名空间：在开发大项目时，会有成百上千个cpp文件，经常会发生撞名字的情况，只要把自己写的辅助函数用匿名命名空间包起来，就可以避免。
 //不适用static：因为static在c++中兼职太多，可以修饰局部变量（静态变量）也可以修饰类成员（静态成员）
 //怎么使用：直接调用即可，因为没有名字，所以没法像MySpace::add()这样用

 // 1. 默认构造
String::String()
//这是极其专业的初始化列表写法：如果你在函数体里面写，系统会提前给你一套默认家具，等你初始化的时候就是把旧家具丢了换新家具，无用功；而冒号开头的打法可以在
//装修开始之前联系装修公司提前确定所需家具，直接就把你要的家具装进去，就方便多了。
//：（冒号）是初始化列表开始的符号。
//new 是在系统内存里找一块地方，让后把这块地方的首地址（第一个房间的房卡）告诉你。new操作符的作用是“向内存要一块地”。而它返回给你的，不是地上的东西。二十这块地的
//门牌号（地址）。在C++中，专门用来存“门牌号”的变量类型，就是指针
//new char[...+1]申请了一块连续的字符内存，并且它返回了这块内存的首地址，data_变量接受了这个地址，所以data_的数据类型一定是char* 
//在底层，new只做了两件事：1，分配内存。2，返回第一个元素的地址（指针）

//初始化列表：如果你在函数体里面写初始化（赋值），在进入大括号之前，data_这个指针已经产生了，里面最开始存着一个随机垃圾地址，进入大括号后，再用new申请一块内存
//，把地址赋值给它，就会造成那个随即垃圾地址指向的内存你永远找不到也无法释放，造成内存泄漏。
    : data_(new char[kMinCapacity + 1]), size_(0), capacity_(kMinCapacity) {
    data_[0] = '\0';
}

// 2. 析构
String::~String() {
    delete[] data_; // delete[] nullptr 是合法的
}

// 3. C 字符串构造
String::String(const char* str)
    : data_(nullptr), size_(0), capacity_(0) {
    const std::size_t len = (str == nullptr) ? 0 : c_str_length(str);
    const std::size_t cap = len > kMinCapacity ? len : kMinCapacity;

    char* fresh = new char[cap + 1]; // 先分配成功
    for (std::size_t i = 0; i < len; ++i) fresh[i] = str[i];
    fresh[len] = '\0';

    data_ = fresh;
    size_ = len;
    capacity_ = cap;
}

// 4. 基础接口
std::size_t String::size() const noexcept { return size_; }
std::size_t String::capacity() const noexcept { return capacity_; }
char& String::operator[](std::size_t index) noexcept { return data_[index]; }
const char& String::operator[](std::size_t index) const noexcept { return data_[index]; }
const char* String::c_str() const noexcept { return data_ != nullptr ? data_ : ""; }
String::operator const char*() const noexcept { return c_str(); }

// 5. 带边界检查的 at
char& String::at(std::size_t index) {
    if (index >= size_) throw std::out_of_range("String::at: index out of range");
    return data_[index];
}
const char& String::at(std::size_t index) const {
    if (index >= size_) throw std::out_of_range("String::at: index out of range");
    return data_[index];
}

// 6. 扩容和追加
void String::ensure_capacity(std::size_t needed) {
    if (needed <= capacity_) return;
    const std::size_t target = growth_target(capacity_, needed);
    char* fresh = new char[target + 1]; // 1. 先分配
    for (std::size_t i = 0; i < size_; ++i) fresh[i] = data_[i];
    fresh[size_] = '\0';
    delete[] data_; // 3. 成功后才释放旧的
    data_ = fresh;
    capacity_ = target;
}

void String::push_back(char ch) {
    ensure_capacity(size_ + 1);
    data_[size_] = ch;
    size_++;
    data_[size_] = '\0';
}



//第二部分。。
String::String(const String& other) {
    size_=other.size_;
    capacity_ = other.capacity_ ;//先看看别人的车厢有多大

    data_= new char[capacity_+1];//申请了一块全新的“字符房间”，大小为capacity_+1.
    for(std::size_t i = 0 ; i < size_;i++){
        data_[i]=other.data_[i]  ;//把别人车厢里的字符一个个搬过来
    }
    data_[size_] = '\0'  ;//加上了结尾符号
}
void String::swap(String& other)noexcept//保证不会出差错
{
    std::swap(data_,other.data_);
    std::swap(size_,other.size_);
    std::swap(capacity_,other.capacity_);//swap函数交换了两个String对象的data_、size_和capacity_成员变量的值，从而实现了两个String对象的内容交换。
}
String& String::operator = (const String& other){
    if (this == &other)//字符值检查，不能自己给自己倒货
        return *this;
  String temp(other);
  swap(temp);
  return *this ;  

// TODO: 在此实现 include/my_string.h 中声明的所有成员函数与运算符。
}
String String::operator+(const String& other) const{
    String result ;//默认空车
    result.ensure_capacity(size_ + other.size_);//确保result车厢够大
    for (std::size_t i = 0 ;i < size_ ;++i){
        result.data_[i] = data_[i] ;//把自己的货装进去
    }
    for (std::size_t i = 0 ;i < other.size_ ;++i){
        result.data_[size_ + i] = other.data_[i] ;//把别人的货也装进去
    }
    result.size_=size_+other.size_;
    result.data_[result.size_] = '\0' ;//加上结尾符号
    return result ;
}
void String::insert(std::size_t pos, const String& str) {
    // 1. 边界检查（越界抛异常）
    if (pos > size_) {
        throw std::out_of_range("String::insert: pos out of range");
    }

    // 2. 插入空串，直接返回
    if (str.size_ == 0) {
        return;
    }

    // 3. 处理自插入（如果不处理，直接覆盖会导致源数据被破坏）
    if (this == &str) {
        String temp(str); // 先拷贝一份自己
        insert(pos, temp); // 用拷贝的版本来插入
        return;
    }

    const std::size_t added = str.size_;
    const std::size_t new_size = size_ + added;

    // 4. 判断容量是否足够
    if (new_size <= capacity_) {
        // --- 容量够：原地移动数据 ---
        // 从后往前移，给新数据腾位置（防止覆盖）
        for (std::size_t i = size_ + 1; i-- > pos; ) {
            data_[i + added] = data_[i];
        }
        // 把 str 的内容复制到 pos 处
        for (std::size_t i = 0; i < added; ++i) {
            data_[pos + i] = str.data_[i];
        }
        size_ = new_size;
        data_[size_] = '\0';
    } else {
        // --- 容量不够：先分配新内存 ---
        const std::size_t target = growth_target(capacity_, new_size);
        char* fresh = new char[target + 1];

        // 复制 pos 之前的部分
        for (std::size_t i = 0; i < pos; ++i) fresh[i] = data_[i];
        
        // 插入 str 的内容
        for (std::size_t i = 0; i < added; ++i) fresh[pos + i] = str.data_[i];
        
        // 复制 pos 之后的部分
        for (std::size_t i = pos; i < size_; ++i) fresh[added + i] = data_[i];
        
        fresh[new_size] = '\0';

        // 释放旧车厢，接管新车厢
        delete[] data_;
        data_ = fresh;
        size_ = new_size;
        capacity_ = target;
    }
}