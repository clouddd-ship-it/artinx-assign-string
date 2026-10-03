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
 namespace{
    constexpr std::size_t kMinCapacity = 16;

    std::size_t c_str_length(const char* s){
        std::size_t len = 0 ;
        while (s && s[len] != '\0') {
            ++len;
        }
        return len ;
        }

        std::size_t growth_target(std::size_t current, std::size_t needed){
            std::size_t target = current < kMinCapacity ? kMinCapacity : current;
            while (target < needed) target *= 2;
            return target;
    }
 }

 // 1. 默认构造
String::String()
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
    ++size_;
    data_[size_] = '\0';
}

// TODO: 在此实现 include/my_string.h 中声明的所有成员函数与运算符。
