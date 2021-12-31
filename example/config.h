#ifndef __CLI_CONFIG_T
#define __CLI_CONFIG_T

#include <stdint.h>
#include <stdbool.h>

///////////////////////////////////////////
//          Type Definitions             //
//    These values must be byte sized    //
///////////////////////////////////////////
#define CLI_ARG_COUNT_VALUE_T       uint8_t
#define CLI_COMMAND_COUNT_VALUE_T   uint8_t
#define CLI_TX_BUF_COUNT_VALUE_T    uint8_t
#define CLI_BUF_VALUE_T             uint8_t
#define CLI_BUF_COUNT_VALUE_T       uint8_t
#define CLI_FLAGS_VALUE_T           uint8_t

///////////////////////////////////////////
//           CLI depedendant             //
///////////////////////////////////////////
#define CLI_TAB_COMPLETE_ENABLE        true
#define CLI_INCLUDE_CARRIAGE_RETURN    true
#define CLI_DELETE_IN_INSERT           true

///////////////////////////////////////////
//    These must be single values of     //
//            CLI_BUF_VALUE_T            //
///////////////////////////////////////////
#define CLI_ENDING_COMMAND_VALUE       '\r'
#define CLI_TAB_COMPLETE_VALUE         '\t'
#define CLI_ARG_DELIMETER_VALUE         ' '
#define CLI_ARG_TERMINATION_VALUE      '\0'
#define CLI_NEW_LINE_VALUE             '\n'
#define CLI_DELETE_VALUE                127
#define CLI_CARRIAGE_RETURN_VALUE      '\r'

///////////////////////////////////////////
//  These must be an array of values of  //
//            CLI_BUF_VALUE_T            //
///////////////////////////////////////////
#define CLI_DELETE_LINE         "\033[1K\r"
#define CLI_DELETE_CHAR             "\b \b"
#define CLI_LINE_BEGINNING             ">>"

#endif // __CLI_CONFIG_T