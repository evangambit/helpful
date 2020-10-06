#ifndef mmap_hpp
#define mmap_hpp

#include <memory>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

template<class T>
class MMappedFile {
 public:
  MMappedFile(const std::string& filename) {
    {  // Find length of file.
      struct stat st;
      stat(filename.c_str(), &st);
      length_ = st.st_size / sizeof(T);
    }
    
    int fd = open(filename.c_str(), O_RDWR);
    if(fd == -1) {
      throw std::runtime_error(std::string("Failed to open \"" + filename + "\""));
    }
    
    data_ = (T*) mmap(NULL, length_ * sizeof(T), PROT_WRITE, MAP_PRIVATE, fd, 0);
    if(data_ == MAP_FAILED) {
      close(fd);
      throw std::runtime_error(std::string("Failed to mmap \"" + filename + "\""));
    }
  }

  ~MMappedFile(){
    munmap(data_, length_ * sizeof(T));
  }

  inline T const& operator[](size_t idx) const {
    return data_[idx];
  }
  
  inline T& operator()(size_t idx){
    return data_[idx];
  }
  
  inline size_t size() {
    return length_;
  }
  
private:
  T* data_;
  size_t length_;
};

#endif /* mmap_hpp */