# Regado

The firmware is developed for ARM Cortex-A9 processor on the Zynq board with customized embedded Linux operating system. It adopts a multi-task architecture to coordinate the real-time tasks such as communication with FPGA, vision algorithms execution, data logging and etc.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

Install and Xilinx SDK and set the toolchain path

```
export XILINX_TOOLCHAIN_DIR="~/Xilinx/SDK/2016.4/gnu/arm/lin/bin"

```

### Building

```
make firmware
```

## Authors

* **Xiangxu Dong** - tsldngx@nus.edu.sg