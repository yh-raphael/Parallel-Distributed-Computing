### Written by Raphael Cho on Dec 12, 2022.
### Last updated on Dec 13, 2022.

# RPC calculating server

This calculator supports Addition(+), Subtraction(-), Multiplication(*), Division(/) and Power(**).  
The operand would be integer values, but it has been developed with considering the case of double type operands.  

Please note that you should NOT input the parenthesis. (-> due to the Homework problem definition)  
That's why I didn't converted an infix expression into a postfix expression.  

## How to compile sources?

Please go the the directory "calc/" and enter the make command "make -f Makefile.calc".  

Once the compilation is over, there should be newly generated object files and executalbe files.  


## How to run it?

### 1st. Server side

Please run the "calc_server" executable file in the server side.  

e.g. You may go to cspro2.sogang.ac.kr and enter the below command.  
-> $ ./calc_server  

If it runs correctly, it will wait for the client requests.  

### 2nd. Client side

Please run the "calc_client" executable file in the client side.  

e.g. You may go to cspro1.sogang.ac.kr and enter the below command.  
Usage: ./calc_client host  (enter)  
2+3*5+2**3  (enter)  

-> $ ./calc_client cspro2.sogang.ac.kr  
-> 2+3*5+2**3  
=> The answer is 25  
=> The answer is 25.000000 in floating number  

### cf. Server side

There would be printed messages on the server side terminal (in this case: cspro2.sogang.ac.kr).  

e.g.  
=> hello server  
=> The answer is 25  
=> The answer is 25.000000 in floating number  



# Kafka: A Text-Based Chatting Program

## How to compile sources?

Please go the the directory "SogangTalk/" and enter the make command "mvn package" or possibly "mvn clean package".  

Once the compilation is over, there should be newly generated .jar files in the "SogangTalk/target/" directory.  


## How to run it?

### 1st. Run the Zookeeper and the Broker

There should be kafka engine on your server.  
If so, you can run the zookeeper using the command below as an example.  

-> $ bin/zookeeper-server-start.sh config/zookeeper.properties  
-> $ bin/kafka-server-start.sh config/server.properties  

### 2nd. Run the .jar file compiled before

If you are in the "SogangTalk/" directory, then you can enter the command below to start the chatting application!  

-> $ java -jar target/SogangTalk_raphael-1.0.jar  



Thanks for your reading :)  