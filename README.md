# QNX IPC Channel Reader

A lightweight utility for monitoring IPC channels from the "io-ipc" resource manager in QNX on Harman OMAP+J5 platforms.

## Overview

This tool allows developers to monitor and debug IPC channels in QNX systems, displaying message content in a readable hex format. It supports both regular numbered IPC channels and raw channels.

## Features

- Monitor any QNX IPC channel in real-time
- Display messages in formatted hex output
- Support for local and remote (networked) channels
- Special handling for raw IPC channels (rx/tx)
- Non-blocking operation with minimal CPU usage

## Usage

```
./ipcreader <channel_number|raw_rx|raw_tx> [j]
```

### Parameters:

- `channel_number`: The IPC channel number to monitor (e.g., 25)
- `raw_rx`: Monitor the raw receive channel at `/dev/ipc/raw/rx_0`
- `raw_tx`: Monitor the raw transmit channel at `/dev/ipc/raw/tx_0`
- `j`: Optional flag to monitor channels on a remote "jacinto" system (uses `/net/hu-jacinto/...` paths)

### Examples:

```bash
# Monitor local IPC channel 25
./ipcreader 25

# Monitor local raw receive channel
./ipcreader raw_rx

# Monitor raw transmit channel on jacinto system
./ipcreader raw_tx j
```

## Compilation

Use the QNX Momentics 4.7 IDE included in SDP6.5.0SP1.

## Important Notes

- **Message Consumption**: Reading from an IPC channel consumes messages, preventing other applications from receiving them. Use this tool for debugging only, not in production environments. Other applications will compete for messages and may need to be killed for accurate tracing.
- **Performance Impact**: When running this utility, some system functionality may be affected as messages are intercepted before reaching their intended recipients.
- **Raw chanels**: To read from raw objects (rx_0/tx_0), io-ipc must be started with the "-raw" argument. Edit your starter.cfg to add this parameter.

## Implementation Details

The utility opens the specified IPC channel in non-blocking mode, continually polling for new messages. When data is received, it's displayed in a formatted hex output. 

## Paths

The utility uses the following path patterns:

- Regular channels: `/dev/ipc/ipc<number>`
- Raw receive channel: `/dev/ipc/raw/rx_0`
- Raw transmit channel: `/dev/ipc/raw/tx_0`

With the jacinto flag (`j`), these become:

- Regular channels: `/net/hu-jacinto/dev/ipc/ipc<number>`
- Raw receive channel: `/net/hu-jacinto/dev/ipc/raw/rx_0`
- Raw transmit channel: `/net/hu-jacinto/dev/ipc/raw/tx_0`

## Channel mapping

```
[OMAP]:

0, 1                                Not exposed by default
25                                  CDSYSMOSTServerDSIIPCBridgeStub. Consumed by: opt/sys/bin/most.
26                                  Watchdog
27                                  CDSYSNBTEVOTouchComDSIIPCBridge. Consumed by: opt/car/bin/touchComDevice.
28
rx_0                                (io-ipc -raw) combination of all messages received. The channel id is byte2 (hex).
tx_0                                (io-ipc -raw) combination of all messages sent The channel id is byte2 (hex).


[Jacinto]:

0, 1                                Not exposed by default
2                                   V850 IOC OnOff channel
4                                   Watchdog
5
6
7                                   Raw GPS sensor data consumed by: OMAP /usr/bin/dev-navsens.
8                                   CDSYSMOSTServerDSIIPCBridgeStub
9                                   CDSYSMOSTServerDSIIPCBridgeStub
10                                  /bin/sysetsrvproxyevo_s
11                                  CDSYSAmpControlDSIIPCBridge
12
13                                  CDSYSFanControlDSIIPCBridge 
14                                  CDSYSErrorMemoryAppClientDSIIPCBridge
15                                  CDSYSErrorMemoryAppDSIIPCBridgeStub
16                                  CDSYSNBTGatewayDiagnosisDSIIPCBridge
17                                  CDSYSProtocolGateway_ControlDSIIPCBridge
18
19                                  CAN?
rx_0
tx_0
```

## License

[MIT License](LICENSE)

## Contributing

Contributions welcome! Please feel free to submit a Pull Request.
