# Streamix Application `<app_name>`

This application ...

## Configuration

For each net the following properties can be added to the root node of the net:
 - `profiler` if set to `on` or `1` profiling for this net is enabled.
   If set to `off` or `0` profiling is disabled (default).

An example configuration can be found in `/opt/smx/conf/<app_name>.xml`.

## Network

The application network is described in `<app_name>.smx` with the coordination language Streamix.
Refer to the configuration file `/opt/smx/conf/<app_name>.xml` for a list of all boxes used in the network.

## Usage

To display the command line options of the application type:
```
<app_name> - h
```

## Compiling and Installing

A Makefile is provided with the following commands (among others):
 - `make`: compile the app and all local box implementations to an executable `<app_name>`.
   Note that all local box implementations are linked statically while external ones (marked with `extern` in Streamix) are linked dynamically.
 - `make dpkg`: create a `deb` package including the binary, this document, and the default configuration file.
 - `sudo apt install`: manually install to the target folders
 - `sudo apt uninstall`: manually remove the app files from to the target folders
