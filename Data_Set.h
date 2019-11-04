// The structures below are the repository for the data values extracted from the
// JSON message. The structures are popolated with the extracted data by the "value()"
// member function in the main ApixuWeather.cpp file.

// The content is zero or "" when first created.

/***************************************************************************************
** Description:   Structure for current weather
***************************************************************************************/
typedef struct WB_current {
	float		lat = 0;
	float		lon = 0;
	String		sunrise;
	String		sunset;
	String		timezone;
	String		station;
	String		last_observation_time;
	String		current_cycle_hour;
	uint32_t	last_observation_unix = 0;
	String		city_name;
	String   	country_code;
	String   	state_code;
	float    	pressure_mb = 0;
	float		sea_level_pressure_mb = 0;
	float    	wind_spd = 0;	
	float 		wind_direction_degrees = 0;	
	String		wind_direction_short;
	String		wind_direction;
	float    	actual_temp = 0;
	float		feels_like_temp = 0;
	float		actual_humidity = 0;
	float		dew_point = 0;
	float		cloud_coverage = 0;
	String		part_of_the_day;
	String		weather_icon;
	uint8_t		weather_code = 0;
	String		weather_description;
	float		visibility = 0;
	float		rain_mm_per_hr = 0;
	float		snow_mm_per_hr = 0;
	float		uv_index = 0;
	float		air_quality = 0;
	float		diffuse_horizontal_solar_irradiance = 0;
	float		direct_normal_solar_irradiance = 0;
	float		global_horizontal_solar_irradiance = 0;
	float		estimated_solar_radiation = 0;
	float		solar_elevation_angle = 0;
	float		solar_hour_angle = 0;

} WB_current;

/***************************************************************************************
** Description:   Structure for daily weather
***************************************************************************************/
typedef struct WB_forecast {
	
	float		lat[MAX_DAYS] = { 0 };
	float		lon[MAX_DAYS] = { 0 };
	String		timezone[MAX_DAYS];
	String		city_name[MAX_DAYS];
	String   	country_code[MAX_DAYS];
	String   	state_code[MAX_DAYS];
	String		valid_date[MAX_DAYS];
	uint32_t	forecast_start_period_utc[MAX_DAYS] = { 0 };
	String		forecast_valid_date[MAX_DAYS];
	float		wind_gust_speed[MAX_DAYS] = { 0 };
	float		wind_speed[MAX_DAYS] = { 0 };
	float		wind_direction_degrees[MAX_DAYS] = { 0 };
	String		wind_direction_short[MAX_DAYS];
	String		wind_direction[MAX_DAYS];
	float		average_temp[MAX_DAYS] = { 0 };
	float		max_temp[MAX_DAYS] = { 0 };
	float		min_temp[MAX_DAYS] = { 0 };
	float		high_temp_day[MAX_DAYS] = { 0 };
	float		low_temp_day[MAX_DAYS] = { 0 };
	float		app_max_temp[MAX_DAYS] = { 0 };
	float		app_min_temp[MAX_DAYS] = { 0 };
	float		rain_probability[MAX_DAYS] = { 0 };
	float		accumulated_rain_mm[MAX_DAYS] = { 0 };
	float		accumulated_snow_mm[MAX_DAYS] = { 0 };
	float		snow_depth_mm[MAX_DAYS] = { 0 };
	float		average_pressure_mb[MAX_DAYS] = { 0 };
	float		average_sea_level_pressure_mb[MAX_DAYS] = { 0 };
	float		average_dew_point[MAX_DAYS] = { 0 };
	float		average_humidity[MAX_DAYS] = { 0 };
	String		weather_icon[MAX_DAYS];
	uint8_t		weather_code[MAX_DAYS] = { 0 };
	String		weather_description[MAX_DAYS];
	float		low_clouds_coverage[MAX_DAYS] = { 0 };
	float		mid_clouds_coverage[MAX_DAYS] = { 0 };
	float		high_clouds_coverage[MAX_DAYS] = { 0 };
	float		average_clouds_coverage[MAX_DAYS] = { 0 };
	float		visibility[MAX_DAYS] = { 0 };
	String		max_solar_radiation[MAX_DAYS];
	float		uv_index[MAX_DAYS] = { 0 };
	float		average_ozone[MAX_DAYS] = { 0 };
	float		moon_phase_fraction[MAX_DAYS] = { 0 };
	uint32_t	moonrise_unix[MAX_DAYS] = { 0 };
	uint32_t	moonset_unix[MAX_DAYS] = { 0 };
	uint32_t	sunrise_unix[MAX_DAYS] = { 0 };
	uint32_t	sunset_unix[MAX_DAYS] = { 0 };

} WB_forecast;
