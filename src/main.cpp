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
  // average over 12h
  constexpr auto averaging_time = 12ul * 60ul * 60ul * 1000ul; // ms
  // store measurement every 5 minutes (RAM is limited!)
  constexpr auto interval = 5ul * 60ul * 1000ul; // ms
  constexpr auto N        = averaging_time / interval;
  void display( float temp, float temp_extreme, float hum, float hum_extreme, const char* extreme,
                int digits = 1 ) {
    lcd.clear();
    lcd.setCursor( 0, 0 );
    lcd.print( temp, digits );
    lcd.print( "\337C " );
    lcd.print( extreme );
    lcd.print( temp_extreme, digits );

    lcd.setCursor( 0, 1 );
    lcd.print( hum, digits );
    lcd.print( "%  " );
    lcd.print( extreme );
    lcd.print( hum_extreme, digits );
  }

} // namespace Measurements

auto   temperatures = MeasurementSeries<float, Measurements::N>{};
auto   humidities   = MeasurementSeries<float, Measurements::N>{};
size_t loop_count   = 0;
bool   min          = true;
void   setup() {
  lcd.init();
  lcd.backlight();
  sensor.begin();
  // wait unitl measurement yields something useful
  while ( isnan( sensor.readTemperature() ) || isnan( sensor.readHumidity() ) ) {}
  const auto temp = sensor.readTemperature();
  const auto hum  = sensor.readHumidity();
  temperatures.add( temp );
  humidities.add( hum );
  Measurements::display( temp, temp, hum, hum, "Max:" );
}

void loop() {
  delay( DHTSpecs::measurement_interval );
  const auto temp = sensor.readTemperature();
  const auto hum  = sensor.readHumidity();
  if ( loop_count == Measurements::interval / DHTSpecs::measurement_interval ) {
    temperatures.clearIfFull().add( temp );
    humidities.clearIfFull().add( hum );
    loop_count = 0;
  }
  if ( loop_count % 4 == 0 ) { min = !min; }
  if ( min ) {
    Measurements::display( temp, temperatures.min(), hum, humidities.min(), "Min:" );
  } else {
    Measurements::display( temp, temperatures.max(), hum, humidities.max(), "Max:" );
  }
  ++loop_count;
}
