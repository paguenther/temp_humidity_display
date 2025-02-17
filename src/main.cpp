/******************************************************
 * Copyright (c) 2025, Andre Guenther
 * All rights reserved.

 * This source code is licensed under the GPL-style license found in the
 * LICENSE file in the root directory of this source tree.
**/
#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

constexpr auto lcd_address = 0x27;
constexpr auto n_chars_lcd = 16;
constexpr auto n_lines_lcd = 2;
auto lcd = LiquidCrystal_I2C{lcd_address, n_chars_lcd, n_lines_lcd};

constexpr auto dht_data_pin = 13;
constexpr auto dht_type = DHT22;
auto dht_sensor = DHT{dht_data_pin, dht_type};

constexpr auto messurement_interval = 5000; // ms

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  dht_sensor.begin();
}

void loop() {

  const auto temperature = dht_sensor.readTemperature();
  const auto humidity = dht_sensor.readHumidity();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("\337C"); // \337 = °

  lcd.setCursor(0, 1);
  lcd.print("Hum:  ");
  lcd.print(humidity);
  lcd.print("%");

  delay(messurement_interval);
}
