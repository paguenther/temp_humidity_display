#pragma once

#include "DataStorage.hpp"
#include "math.h"
#undef min
#undef max

template <typename T, size_t Size>
class MeasurementSeries {
public:
  MeasurementSeries& add( const T& data ) {
    if ( !isnan( data ) ) {
      m_data.add( data );
      m_evaluate = true;
    }
    return *this;
  }

  MeasurementSeries& clear() {
    m_evaluate = true;
    m_avg      = NAN;
    m_min      = NAN;
    m_max      = NAN;
    m_data.clear();
    return *this;
  }

  MeasurementSeries& eval() {
    if ( m_evaluate && !m_data.empty() ) {
      m_avg = T{};
      m_min = __FLT_MAX__;
      m_max = __FLT_MIN__;
      for ( const auto& d : m_data ) {
        m_avg += d;
        m_min = d < m_min ? d : m_min;
        m_max = d > m_max ? d : m_max;
      }
      m_avg /= m_data.size();
      m_evaluate = false;
    }
    return *this;
  }

  T average() {
    eval();
    return m_avg;
  }

  T averageAndClear() {
    const auto avg = average();
    clear();
    return avg;
  }

  T min() {
    eval();
    return m_min;
  }

  T max() {
    eval();
    return m_max;
  }

  const DataStorage<T, Size>& dataStorage() { return m_data; }

private:
  bool                 m_evaluate{ true };
  T                    m_avg{ NAN };
  T                    m_min{ NAN };
  T                    m_max{ NAN };
  DataStorage<T, Size> m_data{};
};