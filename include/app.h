//#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(X) Serial.print(X)
#define DEBUG_PRINTLN(X) Serial.println(X)
#define DEBUG_DUMPPARAMS(X) dumpParams(X)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(X)
#define DEBUG_PRINTLN(X)
#define DEBUG_DUMPPARAMS(X)
#define DEBUG_PRINTF(...)
#endif



