A load balancer which sends requests from clients to workers taking into consideration the workload of the workers

#### CONFIG FILES:
1. build_config - bash script for exporting build environment variables
2. local_debug_settings - bash script for exporting variables needed for local testing (not using the test_bed); not recommended

#### DIRs:
1. src - source code
2. test_bed - vagrant environment for testing

#### BUILD:
Run the following commands in the **project** directory:

    `source build_config`
    `make`

#### TEST BED:
The **Test Bed** is a vagrant environment which copies and runs the apps in different virtual machines. It is controlled by **settings.yml**. 

#### RUN TEST BED:
1. Build the project
2. In **test_bed** directory run the following command:

    `vagrant up`
