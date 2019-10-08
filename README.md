# Safety-Critical Task Allocation

Safety-Critical Task Allocation software project sponsored by [SAFRAN](https://www.safran-group.com/).

For more information on theoretical aspect, refer to this [paper](https://ieeexplore.ieee.org/document/8569348).

This repository includes both centralized and decentralized versions.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

What things you need to install the software and how to install them

#### To run on Ubuntu

* Install [git](https://git-scm.com/), [gcc](https://gcc.gnu.org/) and [CMake](https://cmake.org/)
```
sudo apt-get install -y git
sudo apt-get install -y build-essential
sudo apt-get install -y cmake
```

> If CMake version is too old

```
wget https://github.com/Kitware/CMake/releases/download/v3.15.0/cmake-3.15.0.tar.gz
tar -xvf cmake-3.15.0.tar.gz cmake-3.15.0/
cd cmake-3.15.0/
cmake .
make -j4
sudo make install
echo "export CMAKE_ROOT=/usr/local/share/cmake-3.15" >> ~/.bashrc
source ~/.bashrc
```

* Install [Eigen3](http://eigen.tuxfamily.org/index.php?title=Main_Page)
```
wget http://bitbucket.org/eigen/eigen/get/3.3.7.tar.gz
tar -xvf 3.3.7.tar.gz
cd eigen-eigen-323c052e1731
mkdir build
cd build
cmake ..
sudo make install
```

* Install [GLPK](https://www.gnu.org/software/glpk/)
```
wget http://ftp.gnu.org/gnu/glpk/glpk-4.65.tar.gz
tar -xvf glpk-4.65.tar.gz
cd glpk-4.65
./configure
make -j4
sudo make install
```

* Install [MPI](https://www.mpich.org/)
```
wget http://www.mpich.org/static/downloads/3.3.1/mpich-3.3.1.tar.gz
tar -xvf mpich-3.3.1.tar.gz
cd mpich-3.3.1
./configure --disable-fortran
make -j4
sudo make install
```

* Install [xterm](https://invisible-island.net/xterm/)
```
sudo apt-get update -y
sudo apt-get install -y xterm
```

* Add Libraries to Path

```
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib" >> ~/.bashrc
source ~/.bashrc
```

#### To run on Raspberry Pi

##### Install Raspberry Pi operating system (Raspbain) on sd card

* Follow this official [guide](https://www.raspberrypi.org/documentation/installation/installing-images/)

##### Enable ssh and change the hostname

* Open the terminal
```
sudo raspi-config
```
* Choose Interfacing Options

> Enalble ssh and save

* Choose Network Options

> Change the hostname to pi00

* Save and restart

##### Change the ip address

* In thedesktop mode, open the network preference from the top right corner

* At the drop down button next to configure, choose eth0

>  Set the ip addresss to 192.168.0.2

* Save and restart

##### Add ssh authorized key

* Open termimal

```
sudo -i
ssh-keygen
```

* Hit enter until it finishes

```
cd .ssh/
cp id_rsa.pub rpi_id_rsa.pub
cat rpi_id_rsa.pub >> authorized_keys
```
##### Connecting to [GATECH](https://www.gatech.edu/) wifi

* Connect to GTother

* The WPA2-PSK key is 

> GeorgeP@1927

* Log in on the web browser using GT account

##### Installing libraries

* Install [git](https://git-scm.com/), [gcc](https://gcc.gnu.org/), [CMake](https://cmake.org/), [Eigen3](http://eigen.tuxfamily.org/index.php?title=Main_Page), [GLPK](https://www.gnu.org/software/glpk/), and [MPI](https://www.mpich.org/) as above.

* Install [wiringPI](http://wiringpi.com/)
```
sudo apt-get install wiringpi
gpio -v
gpio readall
```

* Install [Phidget](https://www.phidgets.com/docs/Main_Page)
```
sudo -i
wget -qO- http://www.phidgets.com/gpgkey/pubring.gpg | apt-key add -
echo 'deb http://www.phidgets.com/debian stretch main' > /etc/apt/sources.list.d/phidgets.list
apt-get update
apt-get install libphidget22 libphidget22-dev
exit
```

##### Clone the sd card

* Insert an sd card to the PC

* Create an image file

```
cd
sudo dd if=/dev/sdb of=~/SDCardBackup.img
```

* Use [Etcher](https://www.balena.io/etcher/) to flash an image to a new sd card (dd acts weird sometimes on writing)

##### Change the hostname and ip address of the new sd card

* Follow the same instruction as above

### Building the Software on the Local PC

To download and compile the project

```
cd
git clone https://bitbucket.org/tkhamvilai/dist-milp/src/<name of the branch you want>/ my-dist-milp
cd my-dist-milp
mkdir build
cd build
cmake ..
make -j4
```

The execution file should be created in dist-milp/bin folder

## Running the software simulation

To run the project on Ubuntu after successfully compiling

### Execute with MPI

The default setting is already set to use MPI

```
cd
cd my-dist-milp/bin
mpiexec -np 17 xterm -e ./DIST_MILP
```

### Execute without MPI

Comment out `#define USE_MPI` line in `dist-milp/include/MY_MACROS.hpp`, then

```
cd
cd my-dist-milp/build
make -j4
cd ../bin
./DIST_MILP
```

## Deployment on the Immobotarium

To deploy this on the immobotarium (Raspberry Pi cluster)

* Instlall [rsync](https://rsync.samba.org/) on both local PC and raspberry pi

```
sudo apt-get install rsync
```

* Install [cssh](https://github.com/duncs/clusterssh) on the local PC
```
sudo apt install clusterssh
```

To be modified ...

* Register ssh-key of the local PC on the Raspberry pi
```
cat ~/.ssh/id_rsa.pub | ssh username@server.address.com 'cat >> ~/.ssh/authorized_keys'
```

* Centralized

```
sudo mpiexec -np 18 -hosts 192.168.0.2,192.168.0.3,192.168.0.4,192.168.0.5,192.168.0.6,192.168.0.7,192.168.0.8,192.168.0.9,192.168.0.10,192.168.0.11,192.168.0.12,192.168.0.13,192.168.0.14,192.168.0.15,192.168.0.16,192.168.0.17,192.168.0.18,192.168.0.19 ./DIST_MILP
```

* Decentralized

```
sudo mpiexec -np 17 -hosts 192.168.0.19,192.168.0.3,192.168.0.4,192.168.0.5,192.168.0.6,192.168.0.7,192.168.0.8,192.168.0.9,192.168.0.10,192.168.0.11,192.168.0.12,192.168.0.13,192.168.0.14,192.168.0.15,192.168.0.16,192.168.0.17,192.168.0.18 ./DIST_MILP
```

## Troubleshoot

* make sure the local pc and the raspberry pi that will execute mpiexec used to ssh to all other Pis at least once


## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Thanakorn Khamvilai** - *Initial work* - [tkhamvilai](https://tkhamvilai.github.io/)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

This project is licensed under the [MIT license](http://opensource.org/licenses/mit-license.php) - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone whose code was used
* Inspiration
* etc
