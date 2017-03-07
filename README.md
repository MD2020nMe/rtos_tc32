# README #

12/07/2016
added LED and debug output for verifying tasks.  porting still going on but basically it will run.  malloc and other memory
features still ongoing.

11/29/2016
added context swith code.  more coming in the next few days.
MD

This is a draft skeleton of the TC32 RTOS code and sample drivers

You will need the full toolchain and SDK.  Perhaps OpenOCD. 

Some limitation at this stage
- newlib support is limited.  
- TC32 MCU does not support floating point math by default.  All floating points are 
  emulated.  If you insist that you need the floating point, chat with Telink staff to see if there is a math library that you can use
- MCU and guidelines are available with the source code under doc directory
  it will need to be updated but it will work for most of your tasks

### What is this repository for? ###

* FreeRTOS over 8269
* Version  0.1


### How do I get set up? ###

* Summary of set up
* Configuration
* Dependencies
* Deployment instructions

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact
