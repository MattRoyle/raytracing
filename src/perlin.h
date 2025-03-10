#ifndef PERLIN_H
#define PERLIN_H

#include "headers.h"

class perlin {
  public:
    perlin() {
        randvec = new vec3[point_count];//array sized to the number of points in the noise texture
        for (int i = 0; i < point_count; i++) {//assigns a random value [0, 1] to every point
            randvec[i] = unit_vector(vec3::random(-1,1));
        }

        perm_x = perlin_generate_perm();//generates permutations of sequence 0-255 for each axis
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    ~perlin() {
        delete[] randvec;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    double noise(const point3& p) const {
        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());//double difference from floored value
    
        auto i = int(floor(p.x()));
        auto j = int(floor(p.y()));
        auto k = int(floor(p.z()));
        vec3 c[2][2][2];

        for (int di=0; di < 2; di++)
            for (int dj=0; dj < 2; dj++)
                for (int dk=0; dk < 2; dk++)
                    c[di][dj][dk] = randvec[
                        perm_x[(i+di) & 255] ^
                        perm_y[(j+dj) & 255] ^
                        perm_z[(k+dk) & 255]
                    ];//calculate the noise at each corner of the cube

        return perlin_interp(c, u, v, w);//interpolate the values with the floored difference
    }
    double octave(const point3& p, int depth) const {
        auto accum = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        for (int i = 0; i < depth; i++) {//apply decreasing octaves of noise
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return fabs(accum);
    }

  private:
    static const int point_count = 256;
    vec3* randvec;
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

    static double perlin_interp(const vec3 c[2][2][2], double u, double v, double w) {//returns -1 to 1
        auto uu = u*u*(3-2*u);//Hermite cubic, smooths Mach bands
        auto vv = v*v*(3-2*v);
        auto ww = w*w*(3-2*w);
        auto accum = 0.0;

        for (int i=0; i < 2; i++)
            for (int j=0; j < 2; j++)
                for (int k=0; k < 2; k++) {
                    vec3 weight_v(u-i, v-j, w-k);
                    accum += (i*uu + (1-i)*(1-uu))
                           * (j*vv + (1-j)*(1-vv))
                           * (k*ww + (1-k)*(1-ww))
                           * dot(c[i][j][k], weight_v);//dots the weight with the current corner
                }

        return accum;
    }
};

#endif