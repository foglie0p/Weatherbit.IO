#ifdef ESP8266
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif

#include <JSON_Listener.h>
#include <JSON_Decoder.h>
#include "WeatherbitIO.h"

/***************************************************************************************
** Function name:           getCurrent
** Description:             Setup the weather forecast request from api.weatherbit.io
** The structures etc are created by the sketch and passed to this function.
** Pass a nullptr for current, hourly or forecast pointers to exclude in response.
***************************************************************************************/
bool WeatherbitIO::getCurrent(WB_current *current, String city, String country, String apiKey, String language, String units)
{
  data_set = "current";

  // Local copies of structure pointers, the structures are filled during parsing
  this->current  = current;

  // Fetch the current and the forecast
  String url = "http://api.weatherbit.io/v2.0/current?city=" + city;
  if (country != ""){ url += "&country="  + country;}
  url += "&key=" + apiKey;
  if (language != ""){ url += "&lang="  + language;}
  if (units != ""){ url += "&units=" + units;}

  Serial.println(url);

  // Send GET request and feed the parser
  bool result = parseRequest(url);

  // Null out pointers to prevent crashes
  this->current  = nullptr;

  return result;
}

/***************************************************************************************
** Function name:           getForecast
** Description:             Setup the weather forecast request from api.weatherbit.io
** The structures etc are created by the sketch and passed to this function.
** Pass a nullptr for current, hourly or forecast pointers to exclude in response.
***************************************************************************************/
bool WeatherbitIO::getForecast(WB_forecast *forecast, String city, String country, String apiKey, String language, String units, String max_days)
{
  data_set = "forecast";
  forecast_index = 0;
  
  // Local copies of structure pointers, the structures are filled during parsing
  this->forecast  = forecast;

  // Fetch the current and the forecast
  String url = "http://api.weatherbit.io/v2.0/forecast/daily?city=" + city;
  if (country != ""){ url += "&country="  + country;}
  url += "&key=" + apiKey;
  if (language != "en"){ url += "&lang="  + language;}
  if (units != "M"){ url += "&units=" + units;}
  url += "&days=" + max_days;

  Serial.println(url);

  // Send GET request and feed the parser
  bool result = parseRequest(url);

  // Null out pointers to prevent crashes
  this->forecast  = nullptr;

  return result;
}


#ifdef ESP32 // The ESP32 and ESP8266 have different and evolving client library
             // behaviours so there are two versions of parseRequest

/***************************************************************************************
** Function name:           parseRequest (for ESP32)
** Description:             Fetches the JSON message and feeds to the parser
***************************************************************************************/
bool WeatherbitIO::parseRequest(String url) {

  uint32_t dt = millis();

  WiFiClient client;

  JSON_Decoder parser;
  parser.setListener(this);

  const char*  host = "api.weatherbit.io";

  if (!client.connect(host, 80))
  {
    Serial.println("Connection failed.");
    return false;
  }

  uint32_t timeout = millis();
  char c = 0;
  int ccount = 0;
  uint32_t readCount = 0;
  parseOK = false;

  // Send GET request
  Serial.println("Sending GET request to api.weatherbit.io...");
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  Serial.println("Parsing JSON");

  // Wait for a response to arrive
  while ( client.available() == 0 )
  {
    if ((millis() - timeout) > 4000UL)
    {
      parser.reset();
      client.stop();
      return false;
    }
    yield();
  }

  // Parse the JSON data, the JSON_Decoder drops header characters until first '{'
  // Using client.connected() here can make the ESP32 WiFiClient library hang
  while(client.available() > 0)
  {
    c = client.read();
    parser.parse(c);
#ifdef SHOW_JSON
    if (c == '{' || c == '[' || c == '}' || c == ']') Serial.println();
    Serial.print(c); if (ccount++ > 1000 && c == ']') {ccount = 0; Serial.println("ccount max reached");}
#endif

    if ((millis() - timeout) > 8000UL)
    {
      Serial.println ("JSON parse loop timeout");
      parser.reset();
      client.stop();
      return false;
    }
    yield();
  }

  Serial.print("Done in "); Serial.print(millis()-dt); Serial.println(" ms");

  parser.reset();

  client.stop();

  // A message has been parsed but the datapoint correctness is unknown
  return parseOK;
}

#else // ESP8266 version

/***************************************************************************************
** Function name:           parseRequest (for ESP8266)
** Description:             Fetches the JSON message and feeds to the parser
***************************************************************************************/
bool WeatherbitIO::parseRequest(String url) {

  uint32_t dt = millis();

  WiFiClient client;

  JSON_Decoder parser;
  parser.setListener(this);

  const char*  host = "api.weatherbit.io";

  if (!client.connect(host, 80))
  {
    Serial.println("Connection failed.");
    return false;
  }


  uint32_t timeout = millis();
  char c = 0;
  int ccount = 0;

  parseOK = false;

  // Send GET request
  Serial.println("Sending GET request to api.apixu.com...");
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  Serial.println("Parsing JSON");
  
  // Parse the JSON data, the JSON_Decoder drops header characters until first '{'
  // OR used because ESP8266 WiFiClientlibrary can disconnect when buffer is not empty
  while ( (client.available() > 0 ) || client.connected())
  {
    while ( client.available() )
    {
      c = client.read();
      parser.parse(c);
  #ifdef SHOW_JSON
      if (c == '{' || c == '[' /* || c == '}' || c == ']'*/) Serial.println();
      Serial.print(c); if (ccount++ > 300 && c == ',') {ccount = 0; Serial.println();}
  #endif
      yield();
    }

    if ((millis() - timeout) > 8000UL)
    {
      Serial.println ("JSON client timeout");
      parser.reset();
      client.stop();
      return false;
    }
    yield();
  }

  Serial.println("");
  Serial.print("Done in "); Serial.print(millis()-dt); Serial.println(" ms\n");

  parser.reset();

  client.stop();
  
  // A message has been parsed without error but the datapoint correctness is unknown
  return parseOK;
}

#endif // ESP32 or ESP8266 parseRequest


/***************************************************************************************
** Function name:           key etc
** Description:             These functions are called while parsing the JSON message
***************************************************************************************/
void WeatherbitIO::key(const char *key) {

  currentKey = key;

#ifdef SHOW_CALLBACK
  Serial.print("<<< Key <<<\n");
#endif
}

void WeatherbitIO::startDocument() {

  currentObject = currentKey = "";
  objectLevel = 0;
  arrayIndex = 0;
  parseOK = true;

#ifdef SHOW_CALLBACK
  Serial.print("\n>>> Start document >>>");
#endif
}

void WeatherbitIO::endDocument() {

  currentObject = currentKey = "";
  objectLevel = 0;
  arrayIndex = 0;

#ifdef SHOW_CALLBACK
  Serial.print("\n<<< End document <<<");
#endif
}

void WeatherbitIO::startObject() {

  if (currentKey == "location") {
    data_set = "location";
  }

  if (currentKey == "current") {
    data_set = "current";
  }

  if (currentKey == "forecast") {
    data_set = "forecast";
  }

  objectLevel++;

#ifdef SHOW_CALLBACK
  Serial.print("\n>>> Start object level:" + (String) objectLevel + " index:" + (String) arrayIndex +" >>>");
#endif
}

void WeatherbitIO::endObject() {

  currentObject = "";
  objectLevel--;

#ifdef SHOW_CALLBACK
  Serial.print("\n<<< End object <<<");
#endif
}

void WeatherbitIO::startArray() {

  arrayIndex  = 0;

#ifdef SHOW_CALLBACK
  Serial.print("\n>>> Array index " + (String) arrayIndex +" >>>");
#endif
}

void WeatherbitIO::endArray() {

  arrayIndex  = 0;

#ifdef SHOW_CALLBACK
  Serial.print("\n<<< End array <<<");
#endif
}

void WeatherbitIO::whitespace(char c) {
}

void WeatherbitIO::error( const char *message ) {
  Serial.print("\nParse error message: ");
  Serial.print(message);
  parseOK = false;
}

/***************************************************************************************
** Function name:           iconIndex
** Description:             Convert the weather condition code to an icon array index
***************************************************************************************/
uint8_t WeatherbitIO::iconIndex(uint16_t code)
{
  // 48 weather condition codes are listed on Apixu website
  if ( code == 200 ) return 1;//"t01";
  if ( code == 201 ) return 2;//"t02";
  if ( code == 202 ) return 3;//"t03";
  if ( code == 230 || code == 231 || code == 232) return 4;//"t04";
  if ( code == 300 ) return 5;//"d01";
  if ( code == 301 ) return 6;//"d02";
  if ( code == 302 ) return 7;//"d03";
  if ( code == 500 ) return 8;//"r01";
  if ( code == 501 ) return 9;//"r02";
  if ( code == 502 ) return 10;//"r03";
  if ( code == 511 ) return 11;//"f01";
  if ( code == 520 ) return 12;//"r04";
  if ( code == 521 ) return 13;//"r05";
  if ( code == 522 ) return 14;//"r06";
  if ( code == 600 || code == 621) return 15;//"s01";
  if ( code == 601 || code == 622) return 16;//"s02";
  if ( code == 602 ) return 17;//"s03";
  if ( code == 610 ) return 18;//"s04";
  if ( code == 611 || code == 612) return 19;//"s05";
  if ( code == 623 ) return 20;//"s06";
  if ( code == 700 ) return 21;//"a01";
  if ( code == 711 ) return 22;//"a02";
  if ( code == 721 ) return 23;//"a03";
  if ( code == 731 ) return 24;//"a04";
  if ( code == 741 ) return 25;//"a05";
  if ( code == 751 ) return 26;//"a06";
  if ( code == 800 ) return 27;//"c01";
  if ( code == 801 || code == 802) return 28;//"c02";
  if ( code == 803 ) return 29;//"c03";
  if ( code == 804 ) return 30;//"c04";
  if ( code == 900 ) return 31;//"u00";

  return NO_VALUE;
}

/***************************************************************************************
** Function name:           metric
** Description:             Set the metric or imperial units
***************************************************************************************/
void WeatherbitIO::setMetric(bool m)
{
  metric = m;
}


/***************************************************************************************
** Function name:           value (full data set)
** Description:             Stores the parsed data in the structures for sketch access
***************************************************************************************/

void WeatherbitIO::value(const char *val) {

  String value = val;

  if (data_set == "current") {
    // Using the APW_current struct rather than create one for location
	if (currentKey == "lat") current->lat = value.toFloat();
    else
	if (currentKey == "lon") current->lon = value.toFloat();
    else
	if (currentKey == "sunrise") current->sunrise = value;
    else		
	if (currentKey == "sunset") current->sunset = value;
    else
	if (currentKey == "timezone") current->timezone = value;
    else
	if (currentKey == "station") current->station = value;
    else
	if (currentKey == "ob_time") current->last_observation_time = value;
    else
	if (currentKey == "datetime") current->current_cycle_hour = value;
    else
	if (currentKey == "ts") current->last_observation_unix  = (uint32_t)value.toInt();
    else
	if (currentKey == "city_name") current->city_name = value;
    else
	if (currentKey == "country_code") current->country_code = value;
    else
	if (currentKey == "state_code") current->state_code = value;
    else
	if (currentKey == "pres") current->pressure_mb  = value.toFloat();
    else
	if (currentKey == "slp") current->sea_level_pressure_mb = value.toFloat();
    else
	if (currentKey == "wind_spd") current->wind_spd = value.toFloat();
    else
	if (currentKey == "wind_dir") current->wind_direction_degrees = value.toFloat();
    else
	if (currentKey == "wind_cdir") current->wind_direction_short = value;
    else
	if (currentKey == "wind_cdir_full") current->wind_direction = value;
    else
	if (currentKey == "temp") current->actual_temp = value.toFloat();
    else
	if (currentKey == "app_temp") current->feels_like_temp = value.toFloat();
    else
	if (currentKey == "rh") current->actual_humidity = value.toFloat();
    else
	if (currentKey == "dewpt") current->dew_point = value.toFloat();
    else
	if (currentKey == "clouds") current->cloud_coverage = value.toFloat();
    else
	if (currentKey == "pod") current->part_of_the_day = value;
    else
	if (currentKey == "icon") current->weather_icon = value;
    else
	if (currentKey == "code") current->weather_code  =  iconIndex( (uint16_t)value.toInt() );
    else
	if (currentKey == "description") current->weather_description = value;
    else
	if (currentKey == "vis") current->visibility = value.toFloat();
    else
	if (currentKey == "precip") current->rain_mm_per_hr = value.toFloat();
    else
	if (currentKey == "snow") current->snow_mm_per_hr = value.toFloat();
    else
	if (currentKey == "uv") current->uv_index  = value.toFloat();
    else
	if (currentKey == "aqi") current->air_quality = value.toFloat();
    else
	if (currentKey == "dhi") current->diffuse_horizontal_solar_irradiance = value.toFloat();
    else
	if (currentKey == "dni") current->direct_normal_solar_irradiance = value.toFloat();
    else
	if (currentKey == "ghi") current->global_horizontal_solar_irradiance = value.toFloat();
    else
	if (currentKey == "solar_rad") current->estimated_solar_radiation = value.toFloat();
    else
	if (currentKey == "elev_angle") current->solar_elevation_angle = value.toFloat();
    else
	if (currentKey == "h_angle") current->solar_hour_angle = value.toFloat();
 
    return;
  }

  if (data_set == "forecast") {
	
	if (currentKey == "moonrise_ts") forecast->moonrise_unix[arrayIndex] = (uint32_t)value.toInt();
    else
    if (currentKey == "wind_cdir") forecast->wind_direction_short[arrayIndex] = value;
    else
    if (currentKey == "rh") forecast->average_humidity[arrayIndex] = value.toFloat();
    else
    if (currentKey == "pres") forecast->average_pressure_mb[arrayIndex] = value.toFloat();
    else
    if (currentKey == "high_temp") forecast->high_temp_day[arrayIndex] = value.toFloat();
    else
    if (currentKey == "sunset_ts") forecast->sunrise_unix[arrayIndex] = (uint32_t)value.toInt() - 7620;		// Mixed up by Weatherbit & off by 2h07m
	else
    if (currentKey == "ozone") forecast->average_ozone[arrayIndex] = value.toFloat();
    else
    if (currentKey == "moon_phase") forecast->moon_phase_fraction[arrayIndex] = value.toFloat();
	else
    if (currentKey == "wind_gust_speed") forecast->wind_gust_speed[arrayIndex] = value.toFloat();
    else
    if (currentKey == "snow_depth") forecast->snow_depth_mm[arrayIndex] = value.toFloat();
    else
    if (currentKey == "clouds") forecast->average_clouds_coverage[arrayIndex] = value.toFloat();
    else
    if (currentKey == "ts") forecast->forecast_start_period_utc[arrayIndex] = (uint32_t)value.toInt();
    else
    if (currentKey == "sunrise_ts") forecast->sunset_unix[arrayIndex] = (uint32_t)value.toInt();		// Mixed up by Weatherbit between sunrise and sunset
	else
    if (currentKey == "app_min_temp") forecast->app_min_temp[arrayIndex] = value.toFloat();
    else
    if (currentKey == "wind_spd") forecast->wind_speed[arrayIndex] = value.toFloat();
    else
    if (currentKey == "pop") forecast->rain_probability[arrayIndex] = value.toFloat();
    else
    if (currentKey == "wind_cdir_full") forecast->wind_direction[arrayIndex] = value;
    else
    if (currentKey == "slp") forecast->average_sea_level_pressure_mb[arrayIndex] = value.toFloat();
    else
    if (currentKey == "valid_date") forecast->valid_date[arrayIndex] = value;
    else
    if (currentKey == "app_max_temp") forecast->app_max_temp[arrayIndex] = value.toFloat();
    else
    if (currentKey == "vis") forecast->visibility[arrayIndex] = value.toFloat();
    else
    if (currentKey == "dewpt") forecast->average_dew_point[arrayIndex] = value.toFloat();
    else
    if (currentKey == "snow") forecast->accumulated_snow_mm[arrayIndex] = value.toFloat();
    else
    if (currentKey == "uv") forecast->uv_index[arrayIndex] = value.toFloat();
    else
    if (currentKey == "icon") forecast->weather_icon[arrayIndex] = value;
    else
    if (currentKey == "code") forecast->weather_code[arrayIndex] = iconIndex( (uint16_t)value.toInt() );
    else
    if (currentKey == "description") forecast->weather_description[arrayIndex] = value;
    else
    if (currentKey == "wind_dir") forecast->wind_direction_degrees[arrayIndex] = value.toFloat();
    else
    if (currentKey == "max_dhi") forecast->max_solar_radiation[arrayIndex] = value;
    else
    if (currentKey == "clouds_hi") forecast->high_clouds_coverage[arrayIndex] = value.toFloat();
    else
    if (currentKey == "precip") forecast->accumulated_rain_mm[arrayIndex] = value.toFloat();
    else
    if (currentKey == "low_temp") forecast->low_temp_day[arrayIndex] = value.toFloat();
    else
    if (currentKey == "max_temp") forecast->max_temp[arrayIndex] = value.toFloat();
    else
    if (currentKey == "moonset_ts") forecast->moonset_unix[arrayIndex] = (uint32_t)value.toInt();
    else
    if (currentKey == "datetime") forecast->forecast_valid_date[arrayIndex] = value;
    else
    if (currentKey == "temp") forecast->average_temp[arrayIndex] = value.toFloat();
    else
    if (currentKey == "min_temp") forecast->min_temp[arrayIndex] = value.toFloat();
    else
    if (currentKey == "clouds_mid") forecast->mid_clouds_coverage[arrayIndex] = value.toFloat();
    else
    if (currentKey == "clouds_low") {forecast->low_clouds_coverage[arrayIndex] = value.toFloat(); arrayIndex++;} //clouds_low is last in the forecast

/*
    if (currentKey == "lat") forecast->lat[arrayIndex] = value.toFloat();
    else
    if (currentKey == "lon") forecast->lon[arrayIndex] = value.toFloat();
    else		
    if (currentKey == "timezone") forecast->timezone[arrayIndex] = value;
    else
    if (currentKey == "city_name") forecast->city_name[arrayIndex] = value;
    else
    if (currentKey == "country_code") forecast->country_code[arrayIndex] = value;
    else
    if (currentKey == "state_code") forecast->state_code[arrayIndex] = value;
*/		
   return;
  }
}

