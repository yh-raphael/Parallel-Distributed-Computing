# Problem 1 - [OpenMP] palindrome

## Compilation
Please, go to the directory ./p1 and input the command "$ make".
And then, one execution file will be created: palindrome.

## Execution
usage: $ ./palindrome <# of threads> <input_file> <output_file>
e.g. : $ ./palindrome 4 words.txt output.txt


# Problem 2 - [MPI] prefix sum

## Compilation
Please, go to the directory ./p2 and input the command "$ make".
And then, three execution files will be created: psum_b, psum_nb, psum_scan.

## Execution: blocking calls
usage: $ mpiexec -np <# of nodes> -mca btl ^openib -hostfile <host_file> ./psum_b
e.g. : $ mpiexec -np 4 -mca btl ^openib -hostfile hosts ./psum_b

## Execution: non-blocking calls
usage: $ mpiexec -np <# of nodes> -mca btl ^openib -hostfile <host_file> ./psum_nb
e.g. : $ mpiexec -np 4 -mca btl ^openib -hostfile hosts ./psum_nb


# Problem 3