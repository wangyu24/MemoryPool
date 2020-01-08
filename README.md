# MemoryPool
A memory pool manager which you can preallocate shared_ptr objects of multiple different types in advance, and then get it from the pool in need without ad-hoc memory allocation.

Header-only
C++17, GCC 7.4

Quick Start:

*******************************
  auto sItem = memorypool::MemoryPoolManager<int, double, std::string>::Instance().alloc<std::string>();
  
  *sItem = "HELLO";
  
  std::cout << *sItem << std::endl;

  auto iItem = memorypool::MemoryPoolManager<int, double, std::string>::Instance().alloc<int>();
  
  *iItem = 100;
 
  std::cout << *iItem << std::endl;

  auto dItem = memorypool::MemoryPoolManager<int, double, std::string>::Instance().alloc<double>();
  
  *dItem = 11.2;
  
  std::cout << *dItem << std::endl;
******************************
