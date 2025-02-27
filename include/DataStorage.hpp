#pragma once

#include "standard/vector.hpp"

template <typename T, size_t Size>
class DataStorage {
public:
  DataStorage& add( const T& data ) {
    if ( !full() ) { m_data.emplace_back( data ); }
    return *this;
  }

  size_t       size() const { return m_data.size(); }
  bool         full() const { return m_data.full(); }
  bool         empty() const { return m_data.empty(); }
  DataStorage& clear() {
    m_data.clear();
    return *this;
  }

  typename vector<T, Size>::const_iterator begin() const { return m_data.begin(); }
  typename vector<T, Size>::const_iterator end() const { return m_data.end(); }

private:
  vector<T, Size> m_data{};
};