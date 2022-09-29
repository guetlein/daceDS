# Usage

## Operating System
- Linux Mint 20 was used for development and evaluations. 
- On the development system, we used the confluent packages in version 6.0.2 for Kafka and the schema registry. In addition, we used npm 6.14.4. 
- For Windows wrappers, we used the Java Wrappers on windows 10.



## How to start the simulation service
- Start the Kafka broker
- Start the schema registry
- Run the SimService.jar

## How to trigger a new simulation from the command line
- Create a SCE-file that contains all information related to the scenario
- Run ‚SendScenarioObject.jar [SCE path]‘
- For additional parameters such as a custom output run SendScenarioObject.jar without any args to show possible command line switches

## How to start the frontend
- Start the simulation service
- Go to /Frontend/backend and run ‚npm run start‘. This is the connection between the Kafka  broker and the JS web app.
- Go to /Frontend/frontend and run ‚npm run serve‘. The frontend web app is now available under http://localhost:8080
- If you also wish to browse the raw topics (catalog tab), we suggest to run https://github.com/provectus/kafka-ui in the background, as the frontend links are compliant to their structure.





## Building the Java Wrappers
- All Java projects are Maven projects
- First, run maven install for the JavaBaseWrapper projects
- Afterwards, you can run maven install for the component wrappers, translators, and projectors

## Building the Cpp Wrappers
- Adjust the paths in the Makefile and install missing libraries (mainly libserdes++, libavrocpp, librdkafka, and libcppkafka)
- Afterwards, adjust the paths in the component wrappers‘ Makefiles and run them

## Building Docker images
- In a similar fashion, the wrappers can also be built as docker images. Have a look at the provided Docker files.

## Running Instances
- The simulation controller tries to instantiate the involved instances by calling ‚/[componentType]/run.sh [scenarioID] [instanceID]‘
- The run script can than, for instance, call the compiled native binaries, trigger a docker container, or do more sophistated stuff in order to instantiate a requested component.
- A file called config.properties next to the binaries holds self-explanatory parameters such as the broker IP.
