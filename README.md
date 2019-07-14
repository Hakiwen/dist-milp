# Safety-Critical Task Allocation

Safety-Critical Task Allocation software project sponsored by [SAFRAN](https://www.safran-group.com/).

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
make-j4
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

> Open .bashrc file
```
cd
nano .bashrc
```
> Add the following line to the end of the file
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
```
> Reload .bashrc by
```
source ~/.bashrc
```

#### To run on Raspberry Pi

To be added ...

### Installation

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

To be added ...

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
