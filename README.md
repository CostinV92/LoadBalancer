A load balancer which sends requests from clients to workers taking into consideration the workload of the workers

#### CONFIG FILES:
1. local_debug_settings - bash script for exporting variables needed for local testing (not using the test_bed); not recommended

#### DIRs:
1. src - source code
2. libs - libraries source code
3. test_bed - vagrant environment for testing

#### BUILD:
1. Create a build directory
2. In the build dir, run the `cmake` command giving the project's main `CMakeLists.txt` as argument
3. In the build dir, run `make`

Example commands to build the project (commands executed in the **project** directory):

    `mkdir build`
    `cd build`
    `cmake ..`
    `make`

#### TEST BED:
The **Test Bed** is a vagrant environment which copies and runs the apps in different virtual machines. It is controlled by **settings.yml**. Vagrant, VirtualBox and Ansible needed.

#### RUN TEST BED:
1. Build the project
2. Rename **test_bed/example.settings.yml** to **settings.yml** 
3. In **test_bed/vms** directory run the following command:

    `vagrant up`
