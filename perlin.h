#ifndef PERLIN_H
#define PERLIN_H

#include "headers.h"

class perlin {
  public:
    perlin() {
        randfloat = new double[point_count];//array sized to the number of points in the noise texture
        for (int i = 0; i < point_count; i++) {//assigns a random value [0, 1] to every point
            randfloat[i] = random_double();
        }

        perm_x = perlin_generate_perm();//generates permutations of sequence 0-255 for each axis
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    ~perlin() {
        delete[] randfloat;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    double noise(const point3& p) const {
        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());//double difference from floored value
        
        //hermite cube rounds off the Mach bands
        u = u*u*(3-2*u);
        v = v*v*(3-2*v);
        w = w*w*(3-2*w);

        auto i = int(floor(p.x()));
        auto j = int(floor(p.y()));
        auto k = int(floor(p.z()));
        double c[2][2][2];

        for (int di=0; di < 2; di++)
            for (int dj=0; dj < 2; dj++)
                for (int dk=0; dk < 2; dk++)
                    c[di][dj][dk] = randfloat[
                        perm_x[(i+di) & 255] ^
                        perm_y[(j+dj) & 255] ^
                        perm_z[(k+dk) & 255]
                    ];//calculate the noise at each corner of the cube

        return trilinear_interp(c, u, v, w);//interpolate the values with the floored difference
    }

  private:
    static const int point_count = 256;
    double* randfloat;
    int* perm_x;
    int* perm_y;
    int* perm_z;

    static int* perlin_generate_perm() {//perlin noise algorithm
        auto p = new int[point_count];

        for (int i = 0; i < point_count; i++)//initalise array to index as values
            p[i] = i;

        permute(p, point_count);

        return p;
    }//returns array size 256, of a random permutation of sequence 0-255

    static void permute(int* p, int n) {
        for (int i = n-1; i > 0; i--) {//walk backwards swapping the current with points up to the current
            int target = random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    static double trilinear_interp(double c[2][2][2], double u, double v, double w) {//c is the cube in which the interpolation is preformed
        auto accum = 0.0;
        for (int i=0; i < 2; i++)
            for (int j=0; j < 2; j++)
                for (int k=0; k < 2; k++)//all 8 corners of the cube
                    accum += (i*u + (1-i)*(1-u))
                           * (j*v + (1-j)*(1-v))
                           * (k*w + (1-k)*(1-w))
                           * c[i][j][k];//calculates the weights based on the distances u,v,w from the corner

        return accum;//returns the sum of the weights of each corner
    }
};

#endif