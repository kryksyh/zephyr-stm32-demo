.. _temp_reader:

Temperature reader
###########

Overview
********

A homework with the goal to build a sample zephyr-app that simulates batched temperature readings
and provides basic serial intererface.

The application uses nanopb for data transmission, if it is not added to your workspace install it with:

```
west config manifest.project-filter -- +nanopb
west update
```

Building and Running
********************

This application can be built and executed on any supported board

.. zephyr-app-commands::
   :zephyr-app: temp_reader
   :board: nucleo_f207zg
   :goals: build flash
   :compact:

To build for another board, change "nucleo_f207zg" above to that board's name.

Sample Output
=============


TODO: UPDATE USAGE
.. code-block:: console

    Hello World! x86

Exit QEMU by pressing :kbd:`CTRL+A` :kbd:`x`.
