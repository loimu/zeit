# Zeit project
 A Qt frontend for `crontab` and `at`

### What it does ###
 Provides possibility of quickly adding and editing `crontab` and `at` tasks
 via user-friendly interface.

### Build and run ###
```
#!Bash
mkdir build && cd build
cmake ..
make -j2
./src/zeit
```

### Features: ###
* Add, edit and delete `crontab` tasks
* Add, edit and delete environment variables for `crontab`
* Add and delete `at` commands
* Alarms and Timers

### Screenshot ###
![Screenshot of the Zeit app](https://lh3.googleusercontent.com/-Ju_EuEcBNcY/V63bFFcrWJI/AAAAAAAAAiQ/Uz0SAuiMpTgs0ncY5B988chnFboTqRgpQCL0B/s0/zeit.png)

### Installation on Ubuntu: ###
```
#!Bash
sudo add-apt-repository ppa:blaze/main
sudo apt-get update
sudo apt-get install zeit
```
