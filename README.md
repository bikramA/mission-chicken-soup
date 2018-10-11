(TOC)

### Mission Chicken Soup

  mission_chicken_soup is a multi agent system in which one agent converts the soup ingredients 
  into soup particles. The second agent takes the soup particles and checks for inconsistent 
  ingredients that need to be filtered before final soup delivery.
  
  The first agent is a publisher whereas the second agent is a subscriber.

  ZMQ TCP protocol example usage are given below.


### Getting Started:
  - Getting started can be daunting given the time it takes to build of GAMS, MADARA and ACE.
    We assume that you have GAMS and MADARA environment variables set appropriately.
    In context of the author's environment, this is how the `.bashrc` file looks like

    ```
    export MPC_ROOT=/home/bikram/MPC
    export EIGEN_ROOT=/home/bikram/eigen
    export CAPNP_ROOT=/home/bikram/capnproto
    export MADARA_ROOT=/home/bikram/madara
    export GAMS_ROOT=/home/bikram/gams
    export VREP_ROOT=
    export ZMQ_ROOT=/usr/local
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MADARA_ROOT/lib:$GAMS_ROOT/lib:$VREP_ROOT:$CAPNP_ROOT/c++/.libs
    export PATH=$PATH:$MPC_ROOT:$VREP_ROOT:$CAPNP_ROOT/c++
    
    export CORES=4
    export MY_INSTALL_DIR=/home/bikram/ace
    export ACE_ROOT=$MY_INSTALL_DIR/ACE_wrappers
    export TAO_ROOT=$ACE_ROOT/TAO
    export LD_LIBRARY_PATH=$ACE_ROOT/lib:$TAO_ROOT/lib:$LD_LIBRARY_PATH
    export PATH=$ACE_ROOT/bin:$TAO_ROOT/bin:$PATH

    ```

  - Disclaimer: In the beginning I found it confusing regarding installation and pre-requisites between MADARA, GAMS and ACE. Also ended up looking into deprecated documentation and video tutorials that are largely not relevant any more. I later learnt that ACE is not required as long as MADARA is installed as part of GAMS and we use `action.sh` script to build our projects. In short, install GAMS and use `action.sh`, the rest of the magic follows.


### Create Project:

  We assume that you have installed GAMS and MADARA and their corresponding pre requisites already.

  ```
  mkdir -p $HOME/gams_ws
  cd $HOME/gams_ws
  $GAMS_ROOT/scripts/projects/gpc.pl --path mission_chicken_soup  --container MissionChickenSoup mission_chicken_soup/action.sh compile
  ```

### Update Controller.cpp:

  Major code implementation of this mission is in `src/controller.cpp`.
  The following key steps were taken to create this controller.

  - Reviewed MADARA documentation to find sample code for zmq type transport.
  - An example was found in `/madara/tests/transports/network_profiler.cpp`
  - This code was used as reference to create the mission_chicken_soup controller.cpp

  The controller generates random numbers pertaining to the size of soup ingredients from 0 to 1000.
  The publisher agent takes the soup ingredients and converts a fraction of them into soup particles of size less than 100. The subscriber agent takes all soup particles and filters fine soup particles from large granules
  The subscribe agent aggregates the soup particles and publishes soup conversion ratio in fraction. 


### How to Compile

  - It was difficult to comprehend errors when using `mwc.pl` with `gnuace` option as suggested in the default documentation. GAMS documentation to create and build project came in handy.

  ```
  cd $HOME/gams_ws/mission_chicken_soup
  ./action.sh compile
  ```
  
  - And that's it. Assuming you didn't run into compile time errors.

### How to Run
  
  - Run the publisher in one terminal

  ```
  cd $HOME/gams_ws/mission_chicken_soup/bin
  ./custom_controller -i 0 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001
  ```

  - Run the subscriber in another terminal.

  ```
  cd $HOME/gams_ws/mission_chicken_soup/bin
  ./custom_controller -i 2 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000
  ```


### What Results to Expect
  
  - After sometime expect to see the following results on the publisher terminal

  ```
  Publishing soup packets in 100 variables @5 hz for 30 s on 0MQ transport
  Publisher is done. Check results on subscriber.
  
  Knowledge in Knowledge Base:
  ingredient_size0=90
  ingredient_size1=939
  ingredient_size10=90
  ingredient_size11=90
  ingredient_size12=788
  ingredient_size13=90
  ...
  ...
  ingredient_size99=90
  num_ingredients=100
  ```

  - The knowledge base is also saved as a context file in `bin/recent_context.kkb`.

  - After sometime expect to see the following results on the subscriber terminal
  ```
  Receiving for 30 s on 0MQ transport
  Test: SUCCESS
  Settings:
    Transport type: 0MQ
    Hosts: 
      tcp://127.0.0.1:30001
      tcp://127.0.0.1:30000
    Soup Contents recieved: 121 
    Soup Conversion Ratio: 0.752066 
  ```

### Factory How to Instruction that I could not get to work:

#### COMPILE ON LINUX:
   ```  
    mwc.pl -type gnuace workspace.mwc
    make vrep=1
   ``` 
#### COMPILE ON WINDOWS:
   ```
    mwc.pl -type vc12 workspace.mwc 
   ``` 
   
    <Open Visual Studio and compile project>. Note that you should compile the
    solution in the same settings you used for ACE, MADARA, and GAMS. For most
    users, this is probably Release mode for x64. However, whatever you use
    make sure that you have compiled ACE, MADARA, and GAMS in those settings.
    
#### RUN THE SIMULATION:
    open VREP simulator
    perl sim/run.pl
    
    
