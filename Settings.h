// Configuration settings for Weatherbit.IO library

// These parameters set the data point count stored in program memory (not the datapoint
// count sent by the Weatherbit.IO server). So they determine the memory used during collection
// of the data points.

#define MAX_DAYS 3    	// Maximum day count for the forecast, use a value in range 1 - 7

//#define SHOW_JSON     // Debug only - simple serial output formatting of whole JSON message
//#define SHOW_CALLBACK // Debug only - to show when the callbacks occur
