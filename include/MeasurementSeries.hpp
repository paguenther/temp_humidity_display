#pragma once

#include "DataStorage.hpp"
#include "math.h"
#include "standard/limits.hpp"
#undef min
#undef max

template <typename T, size_t Size>
class MeasurementSeries {
public:
  MeasurementSeries& add( const T& data ) {
    if ( isnan( data ) ) return *this;
    m_data.add( data );
    if ( data < m_min ) { m_min = data; }
    if ( data > m_max ) { m_max = data; }
    return *this;
  }

  MeasurementSeries& clear() {
    m_avg = NAN;
    m_min = numeric_limits<T>::max();
    m_max = numeric_limits<T>::lowest();
    m_data.clear();
    return *this;
  }

  MeasurementSeries& clearIfFull() {
    if ( dataStorage().full() ) { clear(); }
    return *this;
  }

  T average() const { return m_avg; }

  T min() const { return m_min; }

  T max() const { return m_max; }

  const DataStorage<T, Size>& dataStorage() const { return m_data; }

private:
  T                    m_avg{ NAN };
  T                    m_min{ numeric_limits<T>::max() };
  T                    m_max{ numeric_limits<T>::lowest() };
  DataStorage<T, Size> m_data{};
};