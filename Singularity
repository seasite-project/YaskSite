Bootstrap: docker
From: ubuntu:latest

%post
    apt-get clean && apt-get update
    apt-get install -y locales
    localedef -i en_US -f UTF-8 en_US.UTF-8
    apt-get install -y build-essential
    export DEBIAN_FRONTEND=noninteractive
    apt-get install -y cmake
    apt-get install -y cmake-curses-gui
    apt-get install -y git
    apt install -y software-properties-common
    add-apt-repository -y  ppa:deadsnakes/ppa
    apt install -y python3.8
    apt install -y python
    apt-get install -y bc
    apt-get install -y unzip
    apt-get install -y wget
    wget https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS-2023.PUB
    apt-get install -y gnupg
    apt-key add GPG-PUB-KEY-INTEL-SW-PRODUCTS-2023.PUB
    rm GPG-PUB-KEY-INTEL-SW-PRODUCTS-2023.PUB
    echo "deb https://apt.repos.intel.com/oneapi all main" | tee /etc/apt/sources.list.d/oneAPI.list
    apt-get update
    apt-get install -y intel-hpckit
    git clone https://github.com/RRZE-HPC/likwid.git
    cd likwid
    cp config.mk config_old.mk
    sed -e "s@ACCESSMODE = accessdaemon@ACCESSMODE = perf_event@g" config_old.mk > config.mk
    make  && make install
    cd -

%environment
   export SINGULARITY_BASE_PATH=${PWD}

###### Build YaskSite, it has to be done on the running machine therefore as runscript #######
%apprun build
    git clone https://github.com/seasite-project/YaskSite.git
    cd YaskSite
    mkdir build && cd build
    bash -c "source /opt/intel/oneapi/setvars.sh && CC=icc CXX=icpc cmake .. -DI_AGREE_ALL_TERMS_AND_CONDITIONS=true -DCMAKE_INSTALL_PREFIX=${SINGULARITY_BASE_PATH}/installkit -DTEMP_DIR=${SINGULARITY_BASE_PATH}/tmp_YaskSite -DLIKWID_LIBRARIES=/usr/local/lib/liblikwid.so -DLIKWID_INCLUDE_DIR=/usr/local/include && make && make install"
    cd ../example
    mkdir build && cd build
    bash -c "source /opt/intel/oneapi/setvars.sh && CC=icc CXX=icpc cmake .. -DyaskSite_DIR=${SINGULARITY_BASE_PATH}/installkit && make"
    cd ${SINGULARITY_BASE_PATH}
    echo "Building YaskSite success"

##### App for running yasksite ######
%apprun YaskSite
    cd $SINGULARITY_BASE_PATH
    echo "Running YaskSite with arguments $*"
    threads=$(echo "$*" | grep -o -P '(?<=\-c).*?(?=\-)')
    echo "executing export PATH=$PATH:YaskSite/example/build && source /opt/intel/oneapi/setvars.sh && likwid-pin -c S0:0-$((threads-1)) perf_wo_likwid $@"
    bash -c "export PATH=$PATH:YaskSite/example/build && source /opt/intel/oneapi/setvars.sh && likwid-pin -c S0:0-$((threads-1)) perf_wo_likwid $@"

%apprun YaskSite
    This is the complete YaskSite app. To see help options type 'singularity run --app YaskSite <container_name> "-h"'.
    Remember: Please pass arguments as a string, i.e., for example to run Wave3D, radius 1 with 20 cores and inner-dimensions in range 500:20:800 on Cascade Lake Gold 6248
    use 'singularity run --app YaskSite YS_CGO.sif "-k Wave3D:3 -m YaskSite/example/mc_files/CascadelakeSP_Gold-6248.yml -R 500:20:800 -c 20 -t 1 -f auto -r 1 -O spatial -o <out file>"'

###### App for reproducing Fig. 3 plots #######
%apprun Fig3
    cd $SINGULARITY_BASE_PATH
    echo "Running Fig3 with arguments $*"
    threads=$(echo "$*" | grep -o -P '(?<=\-c).*?(?=\-)')
    echo "executing export PATH=$PATH:YaskSite/example/build && source /opt/intel/oneapi/setvars.sh && likwid-pin -c S0:0-$((threads-1)) perf_wo_likwid -k Wave3D:3-t 1 -R 20:20:1000 -f auto $@"
    bash -c "export PATH=$PATH:YaskSite/example/build && source /opt/intel/oneapi/setvars.sh && likwid-pin -c S0:0-$((threads-1)) perf_wo_likwid -k Wave3D:3-t 1 -R 20:20:1000 -f auto $@"

%apphelp Fig3
    Reproduces result in Figure 3 of the paper. machine file, threads, radius, blocking and output file are the parameters. For reproducing the
    results in paper set threads to number of cores on 1 socket (20 on Intel Cascade Lake which we tested). 
    The machine file corresponding to our architecture is the  YaskSite/example/mc_files/CascadelakeSP_Gold-6248.yml
    For example for radius 1 and spatial blocking run : 'singularity run --app Fig3 <container_name> -m <machine_file> -c <ncores> -r 1 -O spatial -o <out_file>'


%help
    This is a singularity container that reproduces result from CGO2021 paper
    "YaskSite – Stencil Optimization Techniques Appliedto Explicit ODE Methods
    on Modern Architectures"
