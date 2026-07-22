#ifndef PERLIN_NOISE_2D_H
#define PERLIN_NOISE_2D_H

#include <vector>
#include <numeric>
#include <random>
#include <cmath>

class PerlinNoise {
private:
    std::vector<int> p;

    // Smooths the transition between grid boundaries using a quintic polynomial
    double fade(double t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    // Linearly interpolates between two values
    double lerp(double t, double a, double b) {
        return a + t * (b - a);
    }

    // Calculates the dot product of a pseudorandom gradient vector and a distance vector
    double grad(int hash, double x, double y, double z) {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

public:
    // Initialize with the standard reference permutation vector
    PerlinNoise() {
        std::vector<int> reference_p = {
            151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
            190, 6,148,247,120,234,75,0,26,56,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,
            125,136,171,168, 68,175,74,165,71,134,139,48,27,166,74,246,245,126,68,131,47,248,81,202,15,220,139,
            190,221,137,92,236,53,67,102,65,54, 11,248,236,1,241,16,137,225,83,232,166,0,249,244,111,102,143,15,
            42,29,108,24,234,43,74,187,70,68,11,115,233,14,3,234,18,4,125,43,148,150,150,26,189,170,130,56,122,
            143,109,137,214,145,24,75,197,27,45,108,204,150,143,218,141,63,127,1,25,120,74,43,244,121,126,95,141,
            244,162,143,39,134,44,128,189,181,97,132,106,44,192,213,59,163,252,216,229,242,172,47,20,140,242,110,
            177,14,245,214,57,235,244,151,80,73,128,85,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,
            198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,
            182,189,28,42,223,183,170,213,119,248,152, 2,44,165,104,123,222,252,30,138,27,35,15,2,103,172,243,254,
            135,22,235,191,139,162,114,151,242,50,134,192,209,53,41,202,154,191,23,52,43,45,243,189,21,171,94,204,
            239,161,251,46,62,74,213,228,135,206,9,144,251,115,222,208,124,129,214,187,175
        };
        p.insert(p.end(), reference_p.begin(), reference_p.end());
        p.insert(p.end(), reference_p.begin(), reference_p.end());
    }

    // Initialize with a custom seed for procedural variations
    PerlinNoise(unsigned int seed) {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::default_random_engine engine(seed);
        std::shuffle(p.begin(), p.end(), engine);
        p.insert(p.end(), p.begin(), p.end());
    }

    // Generates a noise value between -1.0 and 1.0
    double noise(double x, double y, double z = 0.0) {
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;
        int Z = static_cast<int>(std::floor(z)) & 255;

        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);

        double u = fade(x);
        double v = fade(y);
        double w = fade(z);

        int A = p[X] + Y;
        int AA = p[A] + Z;
        int AB = p[A + 1] + Z;
        int B = p[X + 1] + Y;
        int BA = p[B] + Z;
        int BB = p[B + 1] + Z;

        return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
            grad(p[BA], x - 1, y, z)),
            lerp(u, grad(p[AB], x, y - 1, z),
                grad(p[BB], x - 1, y - 1, z))),
            lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
                grad(p[BA + 1], x - 1, y, z - 1)),
                lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
                    grad(p[BB + 1], x - 1, y - 1, z - 1))));
    }
};


#endif

