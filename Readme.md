# Zeit project
 Qt frontend to `crontab` and `at`

### Features: ###
* Add, edit and delete `crontab` tasks
* Add, edit and delete environment variables for `crontab`
* Add and delete `at` commands
* Alarms and Timers
* Optional root actions, PolKit support (KF5Auth and KF5CoreAddons needed)

### Build and run ###
```bash
mkdir build && cd build
cmake ..
make -j2
./src/zeit
```
#### Maybe you have to install this dependencies
```bash
sudo apt install extra-cmake-modules libkf5auth-dev qttools5-dev
```

### Screenshot ###
![Screenshot of the Zeit app](https://raw.githubusercontent.com/loimu/zeit/master/assets/screenshot.png)

### Installation on Ubuntu ###
```bash
# stable releases
sudo add-apt-repository ppa:blaze/main
sudo apt update
sudo apt install zeit

# development snapshots
sudo add-apt-repository ppa:blaze/dev
sudo apt update
sudo apt install zeit
```
