#include <iostream>
#include <stdexcept> //std::out_of_range
#include <typeinfo> //typeid, this #include seems unnecessary, at least on Linux


template <class T>
class pT_wrapper {
   int size_;
   T* pT_;
   public:
   pT_wrapper(T* pT, int size): size_(size), pT_(pT) {}
   const T& operator[](int i) const{
     if(i<0 || i > size_) throw std::out_of_range("Second dimension index out of range");
     return pT_[i];
   }
   T& operator[](int i){ //REFERENCE TO T NECESSARY HERE FOR FUTURE ASSIGNMENT TO WORK
      if(i<0 || i > size_) throw std::out_of_range("Second dimension index out of range");
      return pT_[i];
   }
};
template <class T>
class table{
   int size_[2];
   T** ppT_;
   public:
   table(int a, int b): ppT_(new T*[a]){
     size_[0] = a;
     size_[1] = b;
     for(int i{};i<a;i++){
       ppT_[i] = new T[b];
     }
   }
   int Size() const{return size_[0]*size_[1];}
   int Size(int i) const{
     if(i==0 || i==1){return size_[i];}
     else{throw std::out_of_range("Dimension index out of range");} //JUST IN CASE...
   }
   //int (&Dims())[2] const{return dims_;} //NICE TRY...https://stackoverflow.com/questions/3473438/return-array-in-a-function
   const pT_wrapper<T> operator[](int i) const{
     if(i<0 || i > size_[0]) throw std::out_of_range("First dimension index out of range");
     //return ppT_[i];
     return pT_wrapper<T>(ppT_[i], size_[1]);
   }
   pT_wrapper<T> operator[](int i){
     if(i<0 || i > size_[0]) throw std::out_of_range("First dimension index out of range");
     //return ppT_[i];
     return pT_wrapper<T>(ppT_[i], size_[1]);
   }
   table(const table& table_first): ppT_(new T*[table_first.size_[0]]) {
      size_[0] = table_first.size_[0];
      size_[1] = table_first.size_[1];
      for (int i = 0; i < size_[0]; ++i) {
        ppT_[i] = new T[size_[1]];
        for(int j = 0; j < size_[1]; ++j) {
          ppT_[i][j] = table_first.ppT_[i][j];
        }
      }
   }
   table& operator=(const table &table_first) { // deep copy assignment overloading
      // self-assignment is a special case: don't need to do anything
      if (this != &table_first) {
        for(int i{};i<size_[0];i++){
           delete [] ppT_[i];
         }
         delete [] ppT_;
        size_[0] = table_first.size_[0];
        size_[1] = table_first.size_[1];
        ppT_ = new T*[size_[0]];
        for (int i = 0; i < size_[0]; ++i) {
          ppT_[i] = new T[size_[1]];
          for(int j = 0; j < size_[1]; ++j) {
            ppT_[i][j] = static_cast<T>(table_first.ppT_[i][j]);
          }
        }
      }
      return *this;
    }
    template<class U>
    //table<T>& operator=(const table<U> &table_first) =delete;
    table& operator=(const table<U> &table_first) { // deep copy assignment overloading
       // self-assignment is a special case: don't need to do anything
       //if (this != &table_first) {
         for(int i{};i<size_[0];i++){
            ppT_[i] = nullptr;
            delete [] ppT_[i];
          }
         ppT_ = nullptr;
         delete [] ppT_;
         size_[0] = table_first.Size(0);
         size_[1] = table_first.Size(1);
         ppT_ = new T*[size_[0]];
         for (int i = 0; i < size_[0]; ++i) {
           ppT_[i] = new T[size_[1]];
           for(int j = 0; j < size_[1]; ++j) {
             ppT_[i][j] = static_cast<T>(table_first[i][j]);
           }
         }
       //}
       return *this;
     }
   ~table(){
     for(int i{};i<size_[0];i++){
       delete [] ppT_[i];
     }
     delete [] ppT_;
   }
};

int main() {
  auto test = table<int>(2, 3);
  test[0][0] = 4;
  std::cout << test[0][0]; // выводит 4
  std::cout << std::endl;

  std::cout<<"Testing copy constructor:\n";
  auto test2(test);
  std::cout << "test2(test)" <<std::endl;
  std::cout << "test2[0][0] = " << test2[0][0] << std::endl; // выводит 4
  std::cout << std::endl;

  //TESTING OVERLOADED ASSIGNMENT OPERATOR
  //BOTH TABLES INTEGER TYPE
  std::cout<<"Testing overloaded assignment operator, both tables are of integer type:\n";
  auto test3 = table<int>(3, 4);
  std::cout << "auto test3 = table<int>(3, 4);" <<std::endl;
  test3 = test;
  std::cout << "test3 = test;" <<std::endl;
  std::cout << "test3[0][0] = "<< test3[0][0] << std::endl; // выводит 4
  std::cout << std::endl;

  //DIFFERENT TYPES
  std::cout<<"Now, the table to be overwitten is originally of type double, size 3x4:\n";
  auto test4 = table<double>(3, 4);
  std::cout << "auto test4 = table<double>(3, 4);" <<std::endl;
  test4[0][0] = 3.5;
  std::cout << "test4[0][0] = "<< test4[0][0] << std::endl; // выводит 3.5
  std::cout << "Dim 1 of test4 is "<< test4.Size(0) << std::endl; // выводит 3
  std::cout << "Dim 2 of test4 is "<< test4.Size(1) << std::endl; // выводит 4
  std::cout << "Type of test4 is " << typeid(test4[0][0]).name() << std::endl; // выводит d
  std::cout << std::endl;
  std::cout<<"... after being reassigned to test, changes size to 2x3, but stays of type double, all values are inherited from test but cast to double:\n";
  test4 = test;
  std::cout << "test4 = test;" <<std::endl;
  std::cout << "test4[0][0] = "<< test4[0][0] << std::endl; // выводит 4
  std::cout << "Dim 1 of test4 is "<< test4.Size(0) << std::endl; // выводит 2
  std::cout << "Dim 2 of test4 is "<< test4.Size(1) << std::endl; // выводит 3
  std::cout << "Type of test4 is " << typeid(test4[0][0]).name() << std::endl; // выводит d
  std::cout << "Type of test is "<< typeid(test[0][0]).name() << std::endl; // выводит i
}
