// Client library for the Weatherbit.IO weather datapoint server
// https://api.weatherbit.IO

// The API server uses http

// Modified by Pascal Foglietta from the original APIXU library from Bodmer

// See license.txt in root folder of library
// iconList[] ndex default

#define MAX_ICON_INDEX 16 // Maximum for weather icon index

#define NO_VALUE 15       // for precipType default (none)

#ifndef WeatherbitIO_h
#define WeatherbitIO_h

#include "Settings.h"

#if (MAX_DAYS < 1)
  #undef MAXDAYS
  #define MAX_DAYS 1
#endif

#if (MAX_DAYS > 7)
  #undef MAXDAYS
  #define MAX_DAYS 7
#endif

#include "Data_Set.h"


/***************************************************************************************
** Description:   JSON interface class
***************************************************************************************/
class WeatherbitIO: public JsonListener {

  public:
    // Sketch calls this forecast request, it returns true if no parse errors encountered
    bool getCurrent(WB_current *current, String city, String country, String apiKey, String language, String units);
	bool getForecast(WB_forecast *forecast, String city, String country, String apiKey, String language, String units, String max_days);
					 
    // Called by library (or user sketch), sends a GET request to a http url
    bool parseRequest(String url); // and parses response, returns true if no parse errors

    // Set values to be metric (true) or imperial (false)
    void setMetric(bool true_or_false);

  private:

    // Streaming parser callback functions, allow tracking and decisions
    void startDocument(); // JSON document has started, typically starts once
                          // Initialises varaibles used, e.g. sets objectLayer = 0
                          // and arrayIndex =0
    void endDocument();   // JSON document has ended, typically ends once

    void startObject();   // Called every time an Object start is detected
                          // may be called multiple times as object layers entered
                          // Used to increment objectLayer
    void endObject();     // Called every time an object ends
                          // Used to decrement objectLayer and zero arrayIndex


    void startArray();    // An array of name:value pairs entered
    void endArray();      // Array member ended

    void key(const char *key);         // The current "object" or "name" for a name:value pair
    void value(const char *value);     // String value from name:value pair e.g. "1.23" or "rain"

    void whitespace(char c);           // Whitespace character in JSON - not used

    void error( const char *message ); // Error message is sent to serial port

    // Convert the weather condition number to an icon image index
    uint8_t iconIndex(uint16_t index); 

  private: // Variables used internal to library

    uint16_t forecast_index; // index into the APW_daily structure's data arrays

    // The value storage structures are created and deleted by the sketch and
    // a pointer passed via the library getForecast() call the value() function
    // is then used to populate the structs with values
    WB_current  *current;  // pointer provided by sketch to the APW_current struct
    WB_forecast *forecast; // pointer provided by sketch to the APW_daily struct


    bool     parseOK;       // true if the parse been completed
                            // (does not mean data values gathered are good!)

    bool     metric;        // Metric units if true

    String   currentObject; // Current object e.g. "daily"
    String   data_set;      // A copy of the last object name at the head of an array
                            // short equivalent to path.
    uint16_t objectLevel;   // Object level, increments for new object, decrements at end
    String   currentKey;    // Name key of the name:value pair e.g "temperature"
    uint16_t arrayIndex;    // Array index e.g. 5 for day 5 forecast

    // Lookup table to convert  an array index to a weather icon bmp filename e.g. rain.bmp

};

/***************************************************************************************
***************************************************************************************/
#endif
