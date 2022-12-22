#ifndef __OE_CLI_H
#define __OE_CLI_H

#include "cli/config.h"

#define CLI_CB(command) \
    CLIRet_t command ## Callback(void **args, CLI_ARG_COUNT_VALUE_T argc)
#define CLI_ENTRY(command) \
    { command ## Callback, (const CLI_BUF_VALUE_T *) #command } 

typedef enum
{
    CLI_COMMAND_MATCH = -5,
    CLI_COMMAND_NOT_MATCH = -4,
    CLI_INVALID_ARGUMENT = -3,
    CLI_BUF_FULL = -2,
    CLI_ERROR_GENERAL = -1,
    CLI_OK = 0U,
    CLI_END_ARGS = 1U,
    CLI_SUPPRESS_LINE_BEGIN = 2U,
} CLIRet_t;

typedef CLIRet_t (*CLICommandCallback_t)(void **args, CLI_ARG_COUNT_VALUE_T argc);
typedef void (*CLITXCallback_t)(CLI_BUF_VALUE_T *buf, CLI_TX_BUF_COUNT_VALUE_T bufc);

typedef struct
{
    CLICommandCallback_t    callback;
    const CLI_BUF_VALUE_T   *command;   // This must be terminated with CLI_ARG_TERMINATION_VALUE to work properly
} CLICommand_t;

typedef struct
{
    CLICommand_t                *commands;
    CLI_COMMAND_COUNT_VALUE_T   commandc;
    CLITXCallback_t             tx;
    CLI_BUF_VALUE_T             *buf;
    CLI_BUF_COUNT_VALUE_T       bufc;
} CLIConfig_t;

typedef struct
{
    CLIConfig_t                 config;
    volatile CLI_BUF_VALUE_T    *bufp;
    volatile CLI_BUF_VALUE_T    *bufd;
    union
    {
        struct
        {
            volatile CLI_FLAGS_VALUE_T  ready: 1;
            volatile CLI_FLAGS_VALUE_T  tab: 1;
            volatile CLI_FLAGS_VALUE_T  cdone: 1; 
            volatile CLI_FLAGS_VALUE_T  delete: 1;
            CLI_FLAGS_VALUE_T : 4;
        };
        volatile CLI_FLAGS_VALUE_T       flags;
    };
} CLIInst_t;

CLIRet_t CLIInit(CLIInst_t *cli, CLIConfig_t cnf);
CLIRet_t CLIDeinit(CLIInst_t *cli);
CLIRet_t CLIHandle(CLIInst_t *cli);
CLIRet_t CLIInsert(CLIInst_t *cli, CLI_BUF_VALUE_T value);

#endif // __OE_CLI_H
