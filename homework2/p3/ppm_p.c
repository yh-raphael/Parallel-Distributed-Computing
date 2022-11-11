/* Author: Raphael
   Creation Date: 2022-11-11 
   Topic: PPM image file in P6 format processing
   Go for it to the masterpiece! */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define MAX_NAME_LEN    64

struct timeval t;
double start, finish, elapsed;

typedef struct {
	char	        M, N;		// magic number???
	int             width;
	int             height;
	int             max;
	unsigned char** pixels;
} PPMfile;


int read_image (FILE* fp, PPMfile *pImg);
int write_image (char* file_name, PPMfile *pImg);
int close_image (PPMfile *pImg);

int create_flip (const PPMfile *pOri, PPMfile *pImg);
int create_grayscale (const PPMfile *pOri, PPMfile *pImg);
int create_smooth (const PPMfile *pOri, PPMfile *pImg);


int main (int argc, char *argv[])
{
    char input_file[MAX_NAME_LEN];
    FILE *ifp;
    PPMfile original, flip, grayscale, smooth;
    int my_rank, comm_size;
    
    if (argc != 2) {
        fprintf (stderr, "usage: a.out <input_file>\n");
        return -1;
    }

    strcpy (input_file, argv[1]);
    if ((ifp = fopen (input_file, "r")) == NULL) {
        fprintf (stderr, "input file open error\n");
        return -1;
    }


    /* MPI init */
    MPI_Status status;
	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size (MPI_COMM_WORLD, &comm_size);


    /* Start time measurement */
    gettimeofday (&t, NULL);
    start = t.tv_sec + t.tv_usec / 1000000.0;


    /* Convert! */
    read_image (ifp, &original);

    /* (1) flip */
    create_flip (&original, &flip);
    write_image ("output_flip.ppm", &flip);

    /* (2) grayscale */
    create_grayscale (&original, &grayscale);
    write_image ("output_grayscale.ppm", &grayscale);

    /* (3) smooth */
    create_smooth (&original, &smooth);
    write_image ("output_smooth.ppm", &smooth);


    /* MPI exit */
    MPI_Finalize ();

    /* evaluate the latency */
    gettimeofday (&t, NULL);
    finish = t.tv_sec + t.tv_usec / 1000000.0;
    elapsed = finish - start;
    printf ("The code to be timed took %e seconds\n", elapsed);

    close_image (&original);
    close_image (&flip); close_image (&grayscale); close_image (&smooth);

    fclose (ifp);
	return 0;
}   /* main */


/* Read the image file */
int read_image (FILE* fp, PPMfile *pImg)
{
    /* gain the metadata of the file */
    fscanf (fp, "%c%c ", &pImg->M, &pImg->N);
    fscanf (fp, "%d %d ", &pImg->width, &pImg->height);
    fscanf (fp, "%d ", &pImg->max);

    /* allocate memory space for pixel values */
    pImg->pixels = (unsigned char **) malloc (sizeof (unsigned char *) * pImg->height);
    for (int i = 0; i < pImg->height; i++)
        pImg->pixels[i] = (unsigned char *) malloc (sizeof (unsigned char) * pImg->width * 3);   // 1 pixel = R,G,B

    /* read and assigned the value for each pixel */
    for (int i = 0; i < pImg->height; i++)
    {
        for (int j = 0; j < (pImg->width) * 3; j++)     // 1 pixel = R,G,B
        {
            fread(&pImg->pixels[i][j], sizeof (unsigned char), 1, fp);
        }
    }

    /* Debug */
    // printf("count: %d \n", count);
    // printf("%c%c \n", pImg->M, pImg->N);
    // printf("%d %d\n", pImg->width, pImg-> height);
    // printf("%d\n", pImg->max);

    return 0;
}

/* Write the created image file */
int write_image (char* file_name, PPMfile *pImg)
{
    FILE *ofp;
    if ((ofp = fopen (file_name, "w")) == NULL) {
        fprintf (stderr, "Output file open error \n");
        return -1;
    }

    fprintf (ofp, "%c%c\n", pImg->M, pImg->N);
    fprintf (ofp, "%d %d\n", pImg->width, pImg->height);
    fprintf (ofp, "%d\n", pImg->max);

    for (int i = 0; i < pImg->height; i++)
    {
        for (int j = 0; j < (pImg->width) * 3; j += 3)
        {
            fwrite (&pImg->pixels[i][j], sizeof (unsigned char), 3, ofp);   // R,G,B
        }
    }

    fclose (ofp);
    return 0;
}

/* Free the memory space dynamically allocated */
int close_image (PPMfile *pImg)
{
    for (int i = 0; i < pImg->height; i++)
        free (pImg->pixels[i]);

    return 0;
}


/* Create the flipped (mirrored) image */
int create_flip (const PPMfile *pOri, PPMfile *pImg)
{
    int new_pos;

    /* assign the metadat from the original */
    pImg->M = pOri->M;          pImg->N = pOri->N;
    pImg->width = pOri->width;  pImg->height = pOri->height;
    pImg->max = pOri->max;

    /* allocate memory space for pixel values */
    pImg->pixels = (unsigned char **) malloc (sizeof (unsigned char *) * pImg->height);
    for (int i = 0; i < pImg->height; i++)
        pImg->pixels[i] = (unsigned char *) malloc (sizeof (unsigned char) * pImg->width * 3);   // 1 pixel = R,G,B

    /* flip the original */
    for (int i = 0; i < pImg->height; i++)
    {
        for (int j = 0; j < (pImg->width) * 3; j += 3)
        {
            new_pos = 3 * ((pImg->width) - 1) - j;          // So tricky!!! Done :)

            pImg->pixels[i][new_pos]        = pOri->pixels[i][j];
            pImg->pixels[i][new_pos + 1]    = pOri->pixels[i][j + 1];
            pImg->pixels[i][new_pos + 2]    = pOri->pixels[i][j + 2];
        }
    }

    return 0;
}

/* Create the grayscaled image */
int create_grayscale (const PPMfile *pOri, PPMfile *pImg)
{
    int avg;

    /* assign the metadat from the original */
    pImg->M = pOri->M;          pImg->N = pOri->N;
    pImg->width = pOri->width;  pImg->height = pOri->height;
    pImg->max = pOri->max;

    /* allocate memory space for pixel values */
    pImg->pixels = (unsigned char **) malloc (sizeof (unsigned char *) * pImg->height);
    for (int i = 0; i < pImg->height; i++)
        pImg->pixels[i] = (unsigned char *) malloc (sizeof (unsigned char) * pImg->width * 3);   // 1 pixel = R,G,B

    /* grayscale the original */
    for (int i = 0; i < pImg->height; i++)
    {
        for (int j = 0; j < (pImg->width) * 3; j += 3)
        {
            avg = (pOri->pixels[i][j] + pOri->pixels[i][j + 1] + pOri->pixels[i][j + 2]) / 3;
            pImg->pixels[i][j]      = avg;
            pImg->pixels[i][j + 1]  = avg;
            pImg->pixels[i][j + 2]  = avg;
        }
    }

    return 0;
}

/* Create the smoothed image */
int create_smooth (const PPMfile *pOri, PPMfile *pImg)
{
    int sum_1, sum_2, sum_3;
    int cnt;

    /* assign the metadat from the original */
    pImg->M = pOri->M;          pImg->N = pOri->N;
    pImg->width = pOri->width;  pImg->height = pOri->height;
    pImg->max = pOri->max;

    /* allocate memory space for pixel values */
    pImg->pixels = (unsigned char **) malloc (sizeof (unsigned char *) * pImg->height);
    for (int i = 0; i < pImg->height; i++)
        pImg->pixels[i] = (unsigned char *) malloc (sizeof (unsigned char) * pImg->width * 3);   // 1 pixel = R,G,B

    /* grayscale the original */
    for (int i = 0; i < pImg->height; i++)
    {
        for (int j = 0; j < (pImg->width) * 3; j += 3)
        {
            sum_1 = 0; sum_2 = 0; sum_3 = 0;
            cnt = 0;
            
            if (i != 0 && j != 0)
                                    { sum_1 += pOri->pixels[i - 1][j - 3]; cnt++;
                                      sum_2 += pOri->pixels[i - 1][j - 2];
                                      sum_3 += pOri->pixels[i - 1][j - 1];      }
            if (i != 0)
                                    { sum_1 += pOri->pixels[i - 1][j]; cnt++;
                                      sum_2 += pOri->pixels[i - 1][j + 1];
                                      sum_3 += pOri->pixels[i - 1][j + 2];      }
            if (i != 0 && j != (pImg->width - 1) * 3)
                                    { sum_1 += pOri->pixels[i - 1][j + 3]; cnt++;
                                      sum_2 += pOri->pixels[i - 1][j + 4];
                                      sum_3 += pOri->pixels[i - 1][j + 5];      }
            if (j != (pImg->width - 1) * 3)
                                    { sum_1 += pOri->pixels[i][j + 3]; cnt++;
                                      sum_2 += pOri->pixels[i][j + 4];
                                      sum_3 += pOri->pixels[i][j + 5];          }
            if (i != (pImg->height - 1) && j != (pImg->width - 1) * 3)
                                    { sum_1 += pOri->pixels[i + 1][j + 3]; cnt++;
                                      sum_2 += pOri->pixels[i + 1][j + 4];
                                      sum_3 += pOri->pixels[i + 1][j + 5];      }
            if (i != (pImg->height - 1))
                                    { sum_1 += pOri->pixels[i + 1][j]; cnt++;
                                      sum_2 += pOri->pixels[i + 1][j + 1];
                                      sum_3 += pOri->pixels[i + 1][j + 2];      }
            if (i != (pImg->height - 1) && j != 0)
                                    { sum_1 += pOri->pixels[i + 1][j - 3]; cnt++;
                                      sum_2 += pOri->pixels[i + 1][j - 2];
                                      sum_3 += pOri->pixels[i + 1][j - 1];      }
            if (j != 0)
                                    { sum_1 += pOri->pixels[i][j - 3]; cnt++;
                                      sum_2 += pOri->pixels[i][j - 2];
                                      sum_3 += pOri->pixels[i][j - 1];          }

            pImg->pixels[i][j]      = sum_1 / cnt;
            pImg->pixels[i][j + 1]  = sum_2 / cnt;
            pImg->pixels[i][j + 2]  = sum_3 / cnt;          // Wow... Brilliant! At one trial, Done!
        }
    }

    return 0;
}
