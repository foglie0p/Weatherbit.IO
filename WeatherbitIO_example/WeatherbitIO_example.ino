// Sketch for ESP32 or ESP8266 to fetch the Weather Forecast from Weatherbit.IO

// Sign up for a key and read API configuration info here:
// http://weatherbit.IO

// Choose the WiFi library to load depending on the selected processor
#ifdef ESP8266
  #include <ESP8266WiFi.h>
#else // ESP32
  #include <WiFi.h>
#endif

#include <JSON_Decoder.h> // Load library from: https://github.com/Bodmer/JSON_Decoder

#include <WeatherbitIO2.h> // Load library from: https://github.com/Foglie0p/Weatherbit.IO

#include <Time.h> // Standard Arduino Time library: https://github.com/PaulStoffregen/Time

// =====================================================
// ========= User configured stuff starts here =========
// Further configuration settings can be found in the

#define TIME_OFFSET 0UL * 3600UL          // UTC +0 hour

// Change to suit your WiFi router
#define SSID "YOUR_WIFI"
#define SSID_PASSWORD "YOUR_WIFI_PASS"

// Weatherbit.IO Details, replace  with your API key
String apiKey = "YOUR_API_KEY";           // Obtain this from your Weatherbit.IO account

// Set location to nearest city, zip code, post code, latitude & longitude etc
String city =  "YOUR_CITY";               // No spaces allowed, use %20 for spaces
String country =  "YOUR_COUNTRY_CODE";    // "US", "AU", "CH", etc etc
String language =  "en";                  // Language code
String units =  "M";                      // M for Metric, S for Scientific, I for Imperial 
String max_days = (String)MAX_DAYS;       // MAX_DAYS is set in the Settings.h file of the library for max forecast days requested to the API


// =========  User configured stuff ends here  =========
// =====================================================

WeatherbitIO WB; // Weather forecast library instance

/***************************************************************************************
**                          setup
***************************************************************************************/
void setup() { 
  Serial.begin(115200);

  Serial.printf("Connecting to %s\n", SSID);

  WiFi.begin(SSID, SSID_PASSWORD);
   
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected\n");
}

/***************************************************************************************
**                          loop
***************************************************************************************/
void loop() {

  printCurrentWeather();
  printForecastWeather();

  delay(10UL * 60UL * 1000UL); // Every 10 minutes

}

/***************************************************************************************
**                          Send weather info to serial port
***************************************************************************************/
void printCurrentWeather()
{
  // Create the structures that hold the retrieved weather
  WB_current  *current  = new WB_current;

  time_t time;

  Serial.print("\nRequesting weather information from Weatherbit.io... ");

  WB.getCurrent(current, city, country, apiKey, language, units);

  Serial.println("Weather from Weatherbit.io\n");


  Serial.println("############### Current weather ###############\n");

  Serial.print("Latitude------------------------------------: "); Serial.println(current->lat);
  Serial.print("Longitude-----------------------------------: "); Serial.println(current->lon);
  Serial.print("Sunrise-------------------------------------: "); Serial.print(current->sunrise);Serial.println(" UTC + 0h");
  Serial.print("Sunset--------------------------------------: "); Serial.print(current->sunset);Serial.println(" UTC + 0h");
  Serial.print("Timezone------------------------------------: "); Serial.println(current->timezone);
  Serial.print("Station-------------------------------------: "); Serial.println(current->station);
  Serial.print("Observation time YYYY-MM-DD HH:MM-----------: "); Serial.println(current->last_observation_time);
  Serial.print("Current cycle hour YYYY-MM-DD:HH------------: "); Serial.println(current->current_cycle_hour);
  Serial.print("Last observation time Unix------------------: "); Serial.print(strTime(current->last_observation_unix));
  Serial.print("City name-----------------------------------: "); Serial.println(current->city_name);
  Serial.print("Country code--------------------------------: "); Serial.println(current->country_code);
  Serial.print("State code----------------------------------: "); Serial.println(current->state_code);
  Serial.print("Pressure in Mb------------------------------: "); Serial.print(current->pressure_mb);Serial.println(" Mb");
  Serial.print("Sea level pressure in Mb--------------------: "); Serial.print(current->sea_level_pressure_mb);Serial.println(" Mb");
  Serial.print("Wind speed m/s------------------------------: "); Serial.print(current->wind_spd);Serial.println(" m/s");
  Serial.print("Wind direction degrees----------------------: "); Serial.print(current->wind_direction_degrees);Serial.println(" deg");
  Serial.print("Wind direction short------------------------: "); Serial.println(current->wind_direction_short);
  Serial.print("Wind direction full-------------------------: "); Serial.println(current->wind_direction);
  Serial.print("Actual temperature--------------------------: "); Serial.print(current->actual_temp);Serial.println(" C");
  Serial.print("Apparent temperature------------------------: "); Serial.print(current->feels_like_temp);Serial.println(" C");
  Serial.print("Relative humidity---------------------------: "); Serial.print(current->actual_humidity);Serial.println(" %");
  Serial.print("Dew point-----------------------------------: "); Serial.print(current->dew_point);Serial.println(" C");
  Serial.print("Clouds coverage in %------------------------: "); Serial.print(current->cloud_coverage);Serial.println(" %");
  Serial.print("Part of the day-----------------------------: "); Serial.println(current->part_of_the_day);
  Serial.print("Weather icon code---------------------------: "); Serial.println(current->weather_icon);
  Serial.print("Weather code--------------------------------: "); Serial.println(current->weather_code);
  Serial.print("Weather description-------------------------: "); Serial.println(current->weather_description);
  Serial.print("Visibility in Kms---------------------------: "); Serial.println(current->visibility);
  Serial.print("Rainfall in mm/h----------------------------: "); Serial.print(current->rain_mm_per_hr);Serial.println(" mm/h");
  Serial.print("Snowfall in mm/h----------------------------: "); Serial.print(current->snow_mm_per_hr);Serial.println(" mm/h"); 
  Serial.print("UV index------------------------------------: "); Serial.println(current->uv_index);
  Serial.print("Air quality index---------------------------: "); Serial.println(current->air_quality);
  Serial.print("Diffuse horizontal solar irradiance in W/m--: "); Serial.print(current->diffuse_horizontal_solar_irradiance);Serial.println(" W/m2");
  Serial.print("Direct normal solar irradiance in W/m2------: "); Serial.print(current->direct_normal_solar_irradiance);Serial.println(" W/m2");
  Serial.print("Global horizontal solar irradiance in W/m2--: "); Serial.print(current->global_horizontal_solar_irradiance);Serial.println(" W/m2");
  Serial.print("Estimated solar Radiation in W/m2-----------: "); Serial.print(current->estimated_solar_radiation);Serial.println(" W/m2");
  Serial.print("Solar elevation angle in degrees------------: "); Serial.print(current->solar_elevation_angle);Serial.println(" deg");
  Serial.print("Solar hour angle in degrees-----------------: "); Serial.print(current->solar_hour_angle);Serial.println(" deg");


  delete current;
}

void printForecastWeather()
{
  WB_forecast *forecast = new WB_forecast;

  time_t time;
  
  Serial.println(); Serial.println("############## Forecast weather ###############\n");

  WB.getForecast(forecast, city, country, apiKey, language, units, max_days);
  // MAX_DAYS is defined in User_Setup.h within the WetherbitIO library
  for (int i = 1; i < max_days.toInt(); i++)
  {
    Serial.print("\nForecast summary   "); Serial.print(i); Serial.println();

    //Serial.print("Latitude------------------------------------: ");Serial.println(forecast->lat[i]);
    //Serial.print("Longitude-----------------------------------: ");Serial.println(forecast->lon[i]);
    //Serial.print("Timezone------------------------------------: "); Serial.println(forecast->timezone[i]);
    //Serial.print("City name-----------------------------------: "); Serial.println(forecast->city_name[i]);
    //Serial.print("Country code--------------------------------: "); Serial.println(forecast->country_code[i]);
    //Serial.print("State code----------------------------------: "); Serial.println(forecast->state_code[i]);
    Serial.print("Valid date YYYY-MM-DD-----------------------: "); Serial.println(forecast->valid_date[i]);
    Serial.print("Start of forecast period Unix---------------: "); Serial.print(strTime(forecast->forecast_start_period_utc[i]));
    Serial.print("[DEPRECATED] Forecast valid date YYYY-MM-DD-: "); Serial.println(forecast->forecast_valid_date[i]);
    Serial.print("Wind_gust_speed in m/s----------------------: "); Serial.print(forecast->wind_gust_speed[i]);Serial.println(" m/s");
    Serial.print("Wind_speed in m/s---------------------------: "); Serial.print(forecast->wind_speed[i]);Serial.println(" m/s");
    Serial.print("Wind direction in degrees-------------------: "); Serial.print(forecast->wind_direction_degrees[i]);Serial.println(" deg");
    Serial.print("Abreviated wind direction-------------------: "); Serial.println(forecast->wind_direction_short[i]);
    Serial.print("Wind direction full-------------------------: "); Serial.println(forecast->wind_direction[i]);
    Serial.print("Average temperature-------------------------: "); Serial.print(forecast->average_temp[i]);Serial.println(" C");
    Serial.print("Maximum temperature-------------------------: "); Serial.print(forecast->max_temp[i]);Serial.println(" C");
    Serial.print("Minimum temperature-------------------------: "); Serial.print(forecast->min_temp[i]);Serial.println(" C");
    Serial.print("Highest temperature of the day--------------: "); Serial.print(forecast->high_temp_day[i]);Serial.println(" C");
    Serial.print("Lowest temperature of the day---------------: "); Serial.print(forecast->low_temp_day[i]);Serial.println(" C");
    Serial.print("'Feels like' maximum temperture-------------: "); Serial.print(forecast->app_max_temp[i]);Serial.println(" C");
    Serial.print("'Feels like' minimum temperture-------------: "); Serial.print(forecast->app_min_temp[i]);Serial.println(" C");
    Serial.print("Probability of rain in %--------------------: "); Serial.print(forecast->rain_probability[i]);Serial.println(" %");
    Serial.print("Accumulated rain in mm----------------------: "); Serial.print(forecast->accumulated_rain_mm[i]);Serial.println(" mm");
    Serial.print("Accumulated snow in mm----------------------: "); Serial.print(forecast->accumulated_snow_mm[i]);Serial.println(" mm");
    Serial.print("Depth of snow in mm-------------------------: "); Serial.print(forecast->snow_depth_mm[i]);Serial.println(" mm");
    Serial.print("Average pressure in Mb----------------------: "); Serial.print(forecast->average_pressure_mb[i]);Serial.println(" Mb");
    Serial.print("Average sea level pressure in Mb------------: "); Serial.print(forecast->average_sea_level_pressure_mb[i]);Serial.println(" Mb");
    Serial.print("Average dew point---------------------------: "); Serial.print(forecast->average_dew_point[i]);Serial.println(" C");
    Serial.print("Average humidity in %-----------------------: "); Serial.print(forecast->average_humidity[i]);Serial.println(" %");
    Serial.print("Weather icon code---------------------------: "); Serial.println(forecast->weather_icon[i]);
    Serial.print("Weather code--------------------------------: "); Serial.println(forecast->weather_code[i]);
    Serial.print("Weather description-------------------------: "); Serial.println(forecast->weather_description[i]);
    Serial.print("Low altitude clouds coverage in %-----------: "); Serial.print(forecast->low_clouds_coverage[i]);Serial.println(" %");
    Serial.print("Mid altitude clouds coverage in %-----------: "); Serial.print(forecast->mid_clouds_coverage[i]);Serial.println(" %");
    Serial.print("High altitude clouds coverage in %----------: "); Serial.print(forecast->high_clouds_coverage[i]);Serial.println(" %");
    Serial.print("Average clouds coverage in %----------------: "); Serial.print(forecast->average_clouds_coverage[i]);Serial.println(" %");
    Serial.print("Visibility in Kms---------------------------: "); Serial.print(forecast->visibility[i]);Serial.println(" kms");
    Serial.print("[DEPRECATED] Maximum direct solar radiation : "); Serial.print(forecast->max_solar_radiation[i]);Serial.println(" W/m2");
    Serial.print("UV index------------------------------------: "); Serial.println(forecast->uv_index[i]);
    Serial.print("Average ozone level (Dobson units)----------: "); Serial.println(forecast->average_ozone[i]);
    Serial.print("Moon phase fraction-------------------------: "); Serial.println(forecast->moon_phase_fraction[i]);
    Serial.print("Moonrise Unix-------------------------------: "); Serial.print(strTime(forecast->moonrise_unix[i]));
    Serial.print("Moonset Unix--------------------------------: "); Serial.print(strTime(forecast->moonset_unix[i]));
    Serial.print("Sunrise Unix--------------------------------: "); Serial.print(strTime(forecast->sunrise_unix[i]));
    Serial.print("Sunset Unix---------------------------------: "); Serial.print(strTime(forecast->sunset_unix[i]));

    Serial.println();
  }

  // Delete to free up space and prevent fragmentation as strings change in length
  delete forecast;
}

/***************************************************************************************
**                          Convert unix time to a time string
***************************************************************************************/
String strTime(time_t unixTime)
{
  unixTime += TIME_OFFSET;
  return ctime(&unixTime);
}
