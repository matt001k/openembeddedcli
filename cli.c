#include "cli.h"

#ifndef NULL
#define NULL ((void *) 0U)
#endif

#define FLAG_READY (1U)
#define FLAG_NOT_READY (0U)

#define SPECIAL_VALUE_LENGTH (1U)

static CLIRet_t commandCmp(unsigned char *s1, unsigned char *s2, CLI_SIZE_T n);
static CLI_SIZE_T commandLen(CLIInst_t *cli, CLI_BUF_VALUE_T *command);
static inline CLIRet_t argParse(CLIInst_t *cli, CLI_BUF_VALUE_T **arg, CLI_BUF_VALUE_T *command, CLI_SIZE_T *offset);
static CLIRet_t commandClr(CLIInst_t cli, CLI_BUF_VALUE_T *command);
static inline void *commandCopy(unsigned char *dest, unsigned char *src, CLI_SIZE_T n);
static inline CLIRet_t flagHandler(CLIInst_t *cli);

CLIRet_t CLIInit(CLIInst_t *cli, CLIConfig_t cnf)
{
    CLIRet_t ret = CLI_INVALID_ARGUMENT;

    if (cli && cnf.tx && cnf.buf && cnf.commands)
    {
        ret = CLI_OK;
        cli->config = cnf;
        cli->bufp = cli->config.buf;
        commandClr(*cli, cli->config.buf);
        cli->config.tx((CLI_BUF_VALUE_T *) CLI_DELETE_LINE, sizeof(CLI_DELETE_LINE));
        cli->config.tx((CLI_BUF_VALUE_T *) CLI_LINE_BEGINNING, sizeof(CLI_LINE_BEGINNING));
    }

    return ret;
}

CLIRet_t CLIDeinit(CLIInst_t *cli)
{
    CLIRet_t ret = CLI_INVALID_ARGUMENT;

    if (cli)
    {
        cli->config.buf = NULL;
        cli->config.bufc = 0;
        cli->config.commands = NULL;
        cli->config.commandc = 0;
        cli->config.tx = NULL;
        cli->bufp = NULL;
        ret = CLI_OK;
    }

    return ret;
}

CLIRet_t CLIHandle(CLIInst_t *cli)
{
    CLIRet_t ret = CLI_OK;

    if (cli->ready || cli->tab || cli->delete)
    {
        ret = flagHandler(cli);
    }

    return ret;
}

CLIRet_t CLIInsert(CLIInst_t *cli, CLI_BUF_VALUE_T value)
{
    CLIRet_t ret = CLI_OK;

    if (cli)
    {
        if (!cli->tab && !cli->ready && !cli->delete)
        {
            switch (value)
            {
                case CLI_ENDING_COMMAND_VALUE:
                    if (cli->bufp != cli->config.buf)
                    {
                        cli->ready = FLAG_READY;
                        cli->bufp = cli->config.buf;
                        value = CLI_NEW_LINE_VALUE;
                        cli->config.tx(&value, SPECIAL_VALUE_LENGTH);
#if CLI_INCLUDE_CARRIAGE_RETURN
                        value = CLI_CARRIAGE_RETURN_VALUE;
                        cli->config.tx(&value, SPECIAL_VALUE_LENGTH);
#endif // CLI_INCLUDE_CARRIAGE_RETURN
                    }
                    break;
                case CLI_TAB_COMPLETE_VALUE:
#if CLI_TAB_COMPLETE_ENABLE
                    cli->tab = FLAG_READY;
#endif // CLI_TAB_COMPLETE_ENABLE
                    break;
                case CLI_DELETE_VALUE:
                    if (cli->bufp > cli->config.buf)
                    {
#if CLI_DELETE_IN_INSERT
                        cli->config.tx((CLI_BUF_VALUE_T *) CLI_DELETE_CHAR, sizeof(CLI_DELETE_CHAR));
#else
                        cli->delete = FLAG_READY;
#endif //CLI_DELETE_IN_INSERT

                        if (--cli->bufp == cli->bufd && cli->cdone)
                        {
                            cli->cdone = FLAG_NOT_READY;
                            cli->bufd = NULL;
                        }
                        *(cli->bufp) = CLI_ARG_TERMINATION_VALUE;
                    }
                    break;
                default:
                    if (cli->bufp - cli->config.buf < cli->config.bufc)
                    {
                        if (value == CLI_ARG_DELIMETER_VALUE && !cli->cdone)
                        {
                            cli->cdone = FLAG_READY;
                            cli->bufd = cli->bufp;
                        }
                        *cli->bufp = value;
                        cli->config.tx((CLI_BUF_VALUE_T *) cli->bufp, 1U);
                        cli->bufp++;
                    }
                    else
                    {
                        ret = CLI_BUF_FULL;
                    }
                    break;
            }
        }
    }

    return ret;
}

void *CLIArgParse(CLIInst_t *cli, CLIArg_t *arg, void *args, CLI_BUF_COUNT_VALUE_T argc)
{
    CLI_BUF_VALUE_T *buf = NULL;

    if (arg && cli)
    {
        if (!arg->bufp && argc > 0)
        {
            buf = arg->bufp = args;
            arg->counter = argc;
            arg->counter--;
        }
        else if (arg->counter > 0)
        {
            buf = arg->bufp;
            arg->counter--;
        }

        if (arg->counter > 0)
        {
            arg->bufp += commandLen(cli, arg->bufp) + 1U;
        }

    }

    return buf;
}

static CLIRet_t commandCmp(unsigned char *s1, unsigned char *s2, CLI_SIZE_T n)
{
    CLIRet_t ret = CLI_OK;

    if (s1 && s2 && n != 0U)
    {
        while (n-- != 0U)
        {
            if (*s1++ != *s2++)
            {
                n = 0U;
                ret = CLI_COMMAND_NOT_MATCH;
            }
        }
    }
    else
    {
        ret = CLI_INVALID_ARGUMENT;
    }

    return ret;
}

static CLI_SIZE_T commandLen(CLIInst_t *cli, CLI_BUF_VALUE_T *command)
{
    CLI_SIZE_T n = 0U;

    while (command[n] != CLI_ARG_TERMINATION_VALUE 
        && command[n] != CLI_ARG_DELIMETER_VALUE 
        && n < cli->config.bufc)
    {
        n++;
    }

    return n;
}

static inline CLIRet_t argParse(CLIInst_t *cli, CLI_BUF_VALUE_T **arg, CLI_BUF_VALUE_T *command, CLI_SIZE_T *offset)
{
    CLIRet_t ret = CLI_OK;
    CLI_SIZE_T begin = *offset;

    *arg = &command[begin];
    
    *offset += commandLen(cli, &command[begin]);

    if (command[*offset] == CLI_ARG_TERMINATION_VALUE || *offset > cli->config.bufc)
    {
        ret = CLI_END_ARGS;
    }
    else
    {
        command[*offset] = CLI_ARG_TERMINATION_VALUE;
        *offset = *offset + 1U;
    }

    return ret;
}

static inline CLIRet_t commandClr(CLIInst_t cli, CLI_BUF_VALUE_T *command)
{
    CLIRet_t ret = CLI_INVALID_ARGUMENT;

    if (command)
    {
        ret = CLI_OK;
        while (cli.config.bufc-- > 0U)
        {
            *command++ = CLI_ARG_TERMINATION_VALUE;
        }
    }

    return ret;
}

static inline void *commandCopy(unsigned char *dest, unsigned char *src, CLI_SIZE_T n)
{
    while (n-- > 0U)
    {
        *dest++ = *src++;
    }
    return dest;
}

static inline CLIRet_t flagHandler(CLIInst_t *cli)
{
    CLIRet_t ret = CLI_INVALID_ARGUMENT;

    if (cli)
    {
        if (cli->delete)
        {
#if !CLI_DELETE_IN_INSERT
            cli->config.tx((CLI_BUF_VALUE_T *) CLI_DELETE_CHAR, sizeof(CLI_DELETE_CHAR));
            ret = CLI_OK;
#endif //CLI_DELETE_IN_INSERT
        }
        else
        {
            CLI_SIZE_T n = commandLen(cli, cli->config.buf);

            if (n != 0)
            {
                for (CLI_COMMAND_COUNT_VALUE_T commandIdx = 0U; commandIdx < cli->config.commandc; commandIdx++)
                {
                    if (cli->ready)
                    {
                        n = commandLen(cli,(unsigned char *) cli->config.commands[commandIdx].command);
                        if(commandCmp((unsigned char *) cli->config.commands[commandIdx].command, (unsigned char *) cli->config.buf, n) == CLI_OK)
                        {
                            CLI_BUF_VALUE_T *args[] = {0U};
                            CLI_ARG_COUNT_VALUE_T argc = 0U;
                            unsigned char argt = (unsigned char) CLI_ARG_TERMINATION_VALUE;
                            CLI_BUF_VALUE_T endl = CLI_NEW_LINE_VALUE;

                            if (commandCmp((unsigned char *) &cli->config.buf[n], &argt, SPECIAL_VALUE_LENGTH) == CLI_COMMAND_NOT_MATCH)
                            {

                                CLI_SIZE_T offset = n + 1U;

                                while (argParse(cli, &args[argc++], cli->config.buf, &offset) != CLI_END_ARGS);
                            }

                            ret = cli->config.commands[commandIdx].callback((void *) &args[0U][0U], argc);

                            cli->config.tx(&endl, SPECIAL_VALUE_LENGTH);
#if CLI_INCLUDE_CARRIAGE_RETURN
                            endl = CLI_CARRIAGE_RETURN_VALUE;
                            cli->config.tx(&endl, SPECIAL_VALUE_LENGTH);
#endif // CLI_INCLUDE_CARRIAGE_RETURN
                            break;
                        }
                    }
#if CLI_TAB_COMPLETE_ENABLE
                    else if (cli->tab && !cli->cdone)
                    {
                        if(commandCmp((unsigned char *) cli->config.commands[commandIdx].command, (unsigned char *) cli->config.buf, n) == CLI_OK)
                        {
                            n = commandLen(cli, (CLI_BUF_VALUE_T *) cli->config.commands[commandIdx].command);
                            commandCopy((unsigned char *) cli->config.buf, (unsigned char *) cli->config.commands[commandIdx].command, n);
                            cli->bufp = cli->config.buf + n;
                            cli->config.tx((CLI_BUF_VALUE_T *) CLI_DELETE_LINE, sizeof(CLI_DELETE_LINE));
                            cli->config.tx((CLI_BUF_VALUE_T *) CLI_LINE_BEGINNING, sizeof(CLI_LINE_BEGINNING));
                            cli->config.tx(cli->config.buf, n);
                            break;
                        }
                    }
#endif // CLI_TAB_COMPLETE_ENABLE
                }
            }

            if (cli->ready)
            {
                commandClr(*cli, cli->config.buf);
                cli->cdone = FLAG_NOT_READY;
                cli->config.tx((CLI_BUF_VALUE_T *) CLI_LINE_BEGINNING, sizeof(CLI_LINE_BEGINNING));
            }
        }

        cli->tab = cli->ready = cli->delete = FLAG_NOT_READY;
    }

    return ret;
}