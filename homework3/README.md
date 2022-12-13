## Written by Raphael Cho on Dec 12, 2022.

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


Thanks for your reading :)  