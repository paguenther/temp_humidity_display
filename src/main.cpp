/******************************************************
 * Copyright (c) 2025, Andre Guenther
 * All rights reserved.

 * This source code is licensed under the GPL-style license found in the
 * LICENSE file in the root directory of this source tree.
**/
#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#include "math.h"

#include "MeasurementSeries.hpp"

namespace LCDSpecs {
  constexpr auto address = 0x27;
  constexpr auto n_chars = 16;
  constexpr auto n_lines = 2;
} // namespace LCDSpecs
namespace DHTSpecs {
  constexpr auto data_pin = 13;
  constexpr auto model    = DHT22;
  // DHT22 can make a measurement each 2 seconds
  constexpr auto measurement_interval = 2000; // ms
} // namespace DHTSpecs

auto lcd    = LiquidCrystal_I2C{ LCDSpecs::address, LCDSpecs::n_chars, LCDSpecs::n_lines };
auto sensor = DHT{ DHTSpecs::data_pin, DHTSpecs::model };

namespace Measurements {
  // average over 1 min
  constexpr auto averaging_time = 60ul * 1000ul; // ms
  constexpr auto N              = averaging_time / DHTSpecs::measurement_interval;

  void display( const char* msg1, float temp, const char* unit1, const char* msg2, float hum,
                const char* unit2 ) {
    lcd.clear();
    lcd.setCursor( 0, 0 );
    lcd.print( msg1 );
    lcd.print( temp );
    lcd.print( unit1 );

    lcd.setCursor( 0, 1 );
    lcd.print( msg2 );
    lcd.print( hum );
    lcd.print( unit2 );
  }

} // namespace Measurements

auto temperatures      = MeasurementSeries<float, Measurements::N>{};
auto min_temp_last_12h = __FLT_MAX__;
auto max_temp_last_12h = __FLT_MIN__;
auto humidities        = MeasurementSeries<float, Measurements::N>{};
auto min_hum_last_12h  = __FLT_MAX__;
auto max_hum_last_12h  = __FLT_MIN__;

void setup() {
  lcd.init();
  lcd.backlight();
  sensor.begin();
  // wait unitl measurement yields something useful
  while ( isnan( sensor.readTemperature() ) || isnan( sensor.readHumidity() ) ) {}
  const auto temp = sensor.readTemperature();
  const auto hum  = sensor.readHumidity();
  Measurements::display( "Temp: ", temp, "\337C", "Hum:  ", hum, "%" );
  min_temp_last_12h = temp;
  max_temp_last_12h = temp;
  min_hum_last_12h  = hum;
  max_hum_last_12h  = hum;
}

void loop() {
  int display_toggle = 0;
  while ( !temperatures.dataStorage().full() && !humidities.dataStorage().full() ) {
    temperatures.add( sensor.readTemperature() );
    humidities.add( sensor.readHumidity() );
    if ( display_toggle % 2 == 0 ) {
      Measurements::display( "Min:  ", min_temp_last_12h, "\337C", "Max:  ", max_temp_last_12h,
                             "\337C" );
    }
    delay( DHTSpecs::measurement_interval );
    temperatures.add( sensor.readTemperature() );
    humidities.add( sensor.readHumidity() );
    if ( display_toggle % 2 == 0 ) {
      Measurements::display( "Min:  ", min_hum_last_12h, "%", "Max:  ", max_hum_last_12h, "%" );
    }
    delay( DHTSpecs::measurement_interval );
    temperatures.add( sensor.readTemperature() );
    humidities.add( sensor.readHumidity() );
    if ( display_toggle % 2 == 0 ) {
      Measurements::display( "Temp: ", temperatures.average(), "\337C",
                             "Hum:  ", humidities.average(), "%" );
    }
    delay( DHTSpecs::measurement_interval );
    ++display_toggle;
  }
  min_temp_last_12h =
      temperatures.min() < min_temp_last_12h ? temperatures.min() : min_temp_last_12h;
  max_temp_last_12h =
      temperatures.max() > max_temp_last_12h ? temperatures.max() : max_temp_last_12h;
  min_hum_last_12h = humidities.min() < min_hum_last_12h ? humidities.min() : min_hum_last_12h;
  max_hum_last_12h = humidities.max() > max_hum_last_12h ? humidities.max() : min_hum_last_12h;
  Measurements::display( "Min:  ", min_temp_last_12h, "\337C", "Max:  ", max_temp_last_12h,
                         "\337C" );
  delay( DHTSpecs::measurement_interval );
  Measurements::display( "Min:  ", min_hum_last_12h, "%", "Max:  ", max_hum_last_12h, "%" );
  delay( DHTSpecs::measurement_interval );
  Measurements::display( "Temp: ", temperatures.averageAndClear(), "\337C",
                         "Hum:  ", humidities.averageAndClear(), "%" );
}
