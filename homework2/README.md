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
And then, three execution files will be created: psum_b, psum_nb and psum_scan.  
  
## Execution: blocking calls
usage: $ mpiexec -np <# of nodes> -mca btl ^openib -hostfile <host_file> ./psum_b  
e.g. : $ mpiexec -np 4 -mca btl ^openib -hostfile hosts ./psum_b  
  
## Execution: non-blocking calls
usage: $ mpiexec -np <# of nodes> -mca btl ^openib -hostfile <host_file> ./psum_nb  
e.g. : $ mpiexec -np 4 -mca btl ^openib -hostfile hosts ./psum_nb  
  
  
# Problem 3 - [MPI] image processing of a PPM file in P6 format
3 types of input files are provided: sample_1.ppm (small), sample_2.ppm (medium) and sample_3.ppm (big).  
Small, medium and big mean the problems size.  
  
## Compilation
Please, go to the directory ./p3 and input the command "$ make".  
And then, two execution files will be created: ppm_s, ppm_p.  
  
## Execution: sequential program
usage: $ ./ppm_s <input_file>  
e.g. : $ ./ppm_s sample_1.ppm  
  
-> And then, three output files would be generated: output_flip.ppm, output_grayscale.ppm and output_smooth.ppm.  
  
## Execution: parallel program
usage: $ ./ppm_p <input_file>  
e.g. : $ ./ppm_p sample_1.ppm  
  
-> And then, three output files would be generated: output_flip.ppm, output_grayscale.ppm and output_smooth.ppm.  
  