# MemoryPool
A memory pool manager which can support multiple different types.

Header-only
C++17, GCC7.4

How to use it:
  std::shared_ptr<std::string> sItem = memorypool::MemoryPoolManager<int, double, std::string>::Instance().alloc<std::string>();
  *sItem = "HELLO";
  std::cout << *sItem << std::endl;

  std::shared_ptr<int> iItem = memorypool::MemoryPoolManager<int, double, std::string>::Instance().alloc<int>();
  *iItem = 100;
  std::cout << *iItem << std::endl;

  std::shared_ptr<double> dItem = memorypool::MemoryPoolManager<int, double, std::string>::Instance().alloc<double>();
  *dItem = 11.2;
  std::cout << *dItem << std::endl;
