// helpers.c
// Implements: grayscale, reflect, blur, edges (Sobel)

#include "helpers.h"
#include <math.h>
#include <stdlib.h>

// Helper: cap a value at 255
static int cap255(int value)
{
    return (value > 255) ? 255 : value;
}

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int r = image[i][j].rgbtRed;
            int g = image[i][j].rgbtGreen;
            int b = image[i][j].rgbtBlue;
            int gray = (int) round((r + g + b) / 3.0);
            image[i][j].rgbtRed = gray;
            image[i][j].rgbtGreen = gray;
            image[i][j].rgbtBlue = gray;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            RGBTRIPLE tmp = image[i][j];
            image[i][j] = image[i][width - j - 1];
            image[i][width - j - 1] = tmp;
        }
    }
}

// Blur image (box blur, using a copy so calculations don't affect each other)
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Make a copy of the original image
    RGBTRIPLE copy[height][width];
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            copy[i][j] = image[i][j];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int redSum = 0;
            int greenSum = 0;
            int blueSum = 0;
            int count = 0;

            // Visit neighbors in a 3x3 box
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < height && nj >= 0 && nj < width)
                    {
                        redSum += copy[ni][nj].rgbtRed;
                        greenSum += copy[ni][nj].rgbtGreen;
                        blueSum += copy[ni][nj].rgbtBlue;
                        count++;
                    }
                }
            }

            image[i][j].rgbtRed = (int) round((float) redSum / count);
            image[i][j].rgbtGreen = (int) round((float) greenSum / count);
            image[i][j].rgbtBlue = (int) round((float) blueSum / count);
        }
    }
}

// Detect edges using the Sobel operator
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Copy the original so we read original pixels only
    RGBTRIPLE copy[height][width];
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            copy[i][j] = image[i][j];

    // Sobel kernels
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int gxRed = 0, gxGreen = 0, gxBlue = 0;
            int gyRed = 0, gyGreen = 0, gyBlue = 0;

            // Apply kernels to 3x3 neighborhood
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;
                    int ki = di + 1; // kernel index 0..2
                    int kj = dj + 1;

                    if (ni >= 0 && ni < height && nj >= 0 && nj < width)
                    {
                        RGBTRIPLE p = copy[ni][nj];
                        int kx = Gx[ki][kj];
                        int ky = Gy[ki][kj];

                        gxRed += p.rgbtRed * kx;
                        gxGreen += p.rgbtGreen * kx;
                        gxBlue += p.rgbtBlue * kx;

                        gyRed += p.rgbtRed * ky;
                        gyGreen += p.rgbtGreen * ky;
                        gyBlue += p.rgbtBlue * ky;
                    }
                    // else: outside image treated as black (0), so add nothing
                }
            }

            // Combine Gx and Gy: sqrt(Gx^2 + Gy^2), round, cap at 255
            int red = (int) round(sqrt((double) gxRed * gxRed + (double) gyRed * gyRed));
            int green = (int) round(sqrt((double) gxGreen * gxGreen + (double) gyGreen * gyGreen));
            int blue = (int) round(sqrt((double) gxBlue * gxBlue + (double) gyBlue * gyBlue));

            image[i][j].rgbtRed = cap255(red);
            image[i][j].rgbtGreen = cap255(green);
            image[i][j].rgbtBlue = cap255(blue);
        }
    }
}
