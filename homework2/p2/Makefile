all:
	mpicc psum_b.c -lm -o psum_b
	mpicc psum_nb.c -lm -o psum_nb
	mpicc psum_scan.c -lm -o psum_scan

clean :
	rm -rf psum_b
	rm -rf psum_nb
	rm -rf psum_scan