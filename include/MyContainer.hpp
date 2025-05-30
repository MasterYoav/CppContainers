// Author: realyoavperetz@gmail.com

#pragma once

#include <vector>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <stdexcept>
#include <ostream>
#include <numeric>
#include <cstddef>

namespace ariel {

//-------------------------------------------------------------------------
// helper trait – C++17 check for comparability ( < and == )
//-------------------------------------------------------------------------
namespace detail {
  template<typename, typename = void>
  struct is_comparable : std::false_type {};

  template<typename T>
  struct is_comparable<T, std::void_t<decltype(std::declval<T>() <  std::declval<T>()),
                                      decltype(std::declval<T>() == std::declval<T>())>> : std::true_type {};
}

template<typename T>
constexpr bool is_comparable_v = detail::is_comparable<T>::value;

//-------------------------------------------------------------------------
// MyContainer template
//-------------------------------------------------------------------------

template<typename T = int>
class MyContainer {
  static_assert(is_comparable_v<T>, "MyContainer requires < and == operators");

  std::vector<T> data_;

public:
  using value_type = T;
  using size_type  = std::size_t;

  // ------------------------ basic API -------------------------------
  void addElement(const T& value) { data_.push_back(value); }

  void removeElement(const T& value) {
      auto new_end = std::remove(data_.begin(), data_.end(), value);
      if (new_end == data_.end()) {
          throw std::out_of_range("Element not found in MyContainer");
      }
      data_.erase(new_end, data_.end());
  }

  size_type size() const noexcept { return data_.size(); }

  // --------------------- printing -----------------------------------
  friend std::ostream& operator<<(std::ostream& os, const MyContainer& c) {
      os << "[";
      for (size_type i = 0; i < c.data_.size(); ++i) {
          os << c.data_[i];
          if (i + 1 < c.data_.size()) os << ", ";
      }
      return os << "]";
  }

  //======================== Iterator base ============================
private:
  template<typename OrderPolicy>
  class Iterator { // forward iterator
      const MyContainer* container_ {nullptr};
      std::vector<size_type> order_; // index order to traverse
      size_type pos_ {0};

  public:
      using iterator_category = std::forward_iterator_tag;
      using value_type        = T;
      using difference_type   = std::ptrdiff_t;
      using pointer           = const T*;
      using reference         = const T&;

      Iterator(const MyContainer* c, bool at_end = false) : container_(c) {
          if (!c) return;
          OrderPolicy::build(order_, c->data_);
          pos_ = at_end ? order_.size() : 0;
      }

      reference operator*()  const { return container_->data_[order_[pos_]]; }
      pointer   operator->() const { return &(**this); }

      Iterator& operator++() { ++pos_; return *this; }
      Iterator  operator++(int) { Iterator tmp=*this; ++*this; return tmp; }

      bool operator==(const Iterator& other) const { return pos_==other.pos_ && container_==other.container_; }
      bool operator!=(const Iterator& other) const { return !(*this==other); }
  };

  //======================== Order Policies ===========================
  struct PolicyOrder {
      static void build(std::vector<size_type>& order, const std::vector<T>& data) {
          order.resize(data.size());
          std::iota(order.begin(), order.end(), 0);
      }
  };

  struct PolicyReverse {
      static void build(std::vector<size_type>& order, const std::vector<T>& data) {
          order.resize(data.size());
          std::iota(order.begin(), order.end(), 0);
          std::reverse(order.begin(), order.end());
      }
  };

  struct PolicyAscending {
      static void build(std::vector<size_type>& order, const std::vector<T>& data) {
          order.resize(data.size());
          std::iota(order.begin(), order.end(), 0);
          std::sort(order.begin(), order.end(), [&data](size_type a, size_type b){ return data[a] < data[b]; });
      }
  };

  struct PolicyDescending {
      static void build(std::vector<size_type>& order, const std::vector<T>& data) {
          order.resize(data.size());
          std::iota(order.begin(), order.end(), 0);
          std::sort(order.begin(), order.end(), [&data](size_type a, size_type b){ return data[a] > data[b]; });
      }
  };

  struct PolicySideCross {
      static void build(std::vector<size_type>& order, const std::vector<T>& data) {
          std::vector<size_type> sorted_idx(data.size());
          std::iota(sorted_idx.begin(), sorted_idx.end(), 0);
          std::sort(sorted_idx.begin(), sorted_idx.end(), [&data](size_type a, size_type b){ return data[a] < data[b]; });
          order.clear();
          size_type left = 0, right = sorted_idx.size();
          while (left < right) {
              order.push_back(sorted_idx[left++]);
              if (left < --right) order.push_back(sorted_idx[right]);
          }
      }
  };

  struct PolicyMiddleOut {
      static void build(std::vector<size_type>& order, const std::vector<T>& data) {
          size_type n = data.size();
          order.reserve(n);
          if(n==0) return;
          size_type mid = n/2; // רצפה אם זוגי
          order.push_back(mid);
          for(size_type offset=1; offset<=mid; ++offset){
              if(mid>=offset) order.push_back(mid-offset);
              if(mid+offset<n) order.push_back(mid+offset);
          }
      }
  };

public: //================ Iterator type aliases =======================
  using OrderIterator        = Iterator<PolicyOrder>;
  using ReverseIterator      = Iterator<PolicyReverse>;
  using AscendingIterator    = Iterator<PolicyAscending>;
  using DescendingIterator   = Iterator<PolicyDescending>;
  using SideCrossIterator    = Iterator<PolicySideCross>;
  using MiddleOutIterator    = Iterator<PolicyMiddleOut>;

  //================ begin_/end_ wrappers ============================
  OrderIterator       begin_order()              const { return  OrderIterator(this,false);}  
  OrderIterator       end_order()                const { return  OrderIterator(this,true);}   

  ReverseIterator     begin_reverse_order()      const { return  ReverseIterator(this,false);}  
  ReverseIterator     end_reverse_order()        const { return  ReverseIterator(this,true);}   

  AscendingIterator   begin_ascending_order()    const { return  AscendingIterator(this,false);}  
  AscendingIterator   end_ascending_order()      const { return  AscendingIterator(this,true);}   

  DescendingIterator  begin_descending_order()   const { return  DescendingIterator(this,false);}  
  DescendingIterator  end_descending_order()     const { return  DescendingIterator(this,true);}   

  SideCrossIterator   begin_side_cross_order()   const { return  SideCrossIterator(this,false);}  
  SideCrossIterator   end_side_cross_order()     const { return  SideCrossIterator(this,true);}   

  MiddleOutIterator   begin_middle_out_order()   const { return  MiddleOutIterator(this,false);}  
  MiddleOutIterator   end_middle_out_order()     const { return  MiddleOutIterator(this,true);}   
};

} // namespace cppcont
