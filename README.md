# Open Embedded CLI #

Open Embedded CLI is a simple CLI written in C that can be used with lightweight embedded systems.

It includes the following features:
* Easy configurability
* Command tab auto-complete
* Easy implementation
* Does not rely on standard library
* No limitation on the number of arguements besides the allowable character buffer size during configuration

## Integration Guide

### Configuration

Overall project configuration is done through the `config.h` file.
An example of this file can be seen in `example/config.h`.
This provides a customization for input/output command characters,
as well as data types and complete strings.

Instance configuration is done through the `CLIConfig_t` structure.
Multiple instances can be ran within a code base.

### Inputs

The `CLIInsert` API must be utilized with incoming data,
this can be data polled or available within an interrupt.

ex:

```
static void uartRXISR(uint8_t data)
{
    CLIInsert(&l_cli_inst, data);
}
```

### Outputs

`CLITXCallback_t` must point to the transmission portion of your instance.

ex:

```
static void cliTxCallback(CLI_BUF_VALUE_T *buf, CLI_TX_BUF_COUNT_VALUE_T bufc)
{
    UARTTransmitBuffer(buf, bufc);
}


l_cli_cnf.tx = cliTxCallback;
```

### Handling Events

The `CLIHandle` API must be ran within a set task/main loop to handle new events that occur from inputs.

### Command Calls

`CLICommandCallback_t` is used to define commands.
`args` is an array of passed in arguments to the CLI,
each argument is terminated by `CLI_ARG_TERMINATION_VALUE`.
`argc` is a count of the number of array elements.

## Example

A basic example project is located in the `example` directory.

