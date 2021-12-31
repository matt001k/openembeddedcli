#include "system.h"
#include "uart.h"
#include "watchdog.h"
#include "cli.h"
#include <string.h>

#define CLI_BUFFER_SIZE 128
#define CLI_COMMAND_COUNT 2

#define CLI_ENTRY(command) { command ## Callback, (const CLI_BUF_VALUE_T *) #command } 

static void cliRxCallback(uint8_t data);
static void cliTxCallback(CLI_BUF_VALUE_T *buf, CLI_TX_BUF_COUNT_VALUE_T bufc);

static CLIRet_t cliCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t helloworldCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);


CLIConfig_t l_cli_cnf;
CLIInst_t l_cli_inst;
CLI_BUF_VALUE_T l_cli_buf[CLI_BUFFER_SIZE];
CLICommand_t l_cli_commands[CLI_COMMAND_COUNT] =
{
    CLI_ENTRY(cli),
    CLI_ENTRY(helloworld),
};


int main(void)
{
    l_cli_cnf.buf = l_cli_buf;
    l_cli_cnf.bufc = CLI_BUFFER_SIZE;
    l_cli_cnf.commands = l_cli_commands;
    l_cli_cnf.commandc = CLI_COMMAND_COUNT;
    l_cli_cnf.tx = cliTxCallback;

    SystemInit();
    UARTInit();
    WDTInit();
    
    UARTInterruptSetCallback(cliRxCallback);

    CLIInit(&l_cli_inst, l_cli_cnf);

    while (1)
    {
        CLIHandle(&l_cli_inst);
        WDTKick();
    }
    


    return 0;
}

static void cliRxCallback(uint8_t data)
{
    CLIInsert(&l_cli_inst, data);
}

static void cliTxCallback(CLI_BUF_VALUE_T *buf, CLI_TX_BUF_COUNT_VALUE_T bufc)
{
    UARTTransmitBuffer(buf, bufc);
}

static CLIRet_t cliCallback(void *args, CLI_ARG_COUNT_VALUE_T argc)
{
    uint8_t argIdx = 0U;
    char *buf = args; 

    while (argc--)
    {
        cliTxCallback((CLI_BUF_VALUE_T *) &buf[argIdx], strlen(&buf[argIdx]));
        argIdx+=strlen(&buf[argIdx]) + 1U;
    }
    return CLI_OK;
}

static CLIRet_t helloworldCallback(void *args, CLI_ARG_COUNT_VALUE_T argc)
{
    uint8_t argIdx = 0U;
    char *buf = args; 
    const char msg[] = "Hello World: ";

    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));

    while (argc--)
    {
        cliTxCallback((CLI_BUF_VALUE_T *) &buf[argIdx], strlen(&buf[argIdx]));
        argIdx+=strlen(&buf[argIdx]) + 1U;
    }
    return CLI_OK;
}