#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct markov_chain {
    int size;
    int current_class;
    float** matrix;
    void (*hop)(struct markov_chain* self);
};

static void populate_with_rand(float** matrix, const int size) {
    // populate
    for (int currentClass = 0; currentClass < size; currentClass++) {
        float rowSum = 0;
        for (int transitionClass = 0; transitionClass < size; transitionClass++) {
            matrix[currentClass][transitionClass] = (float)rand() / (float)RAND_MAX;
            rowSum += matrix[currentClass][transitionClass];
        }
        // normalize
        for (int transitionClass = 0; transitionClass < size; transitionClass++) {
            matrix[currentClass][transitionClass] /= rowSum;
        }
    }
}

static int sample_discrete_distribution(const float* probabilities, const int numClasses) {
    if (numClasses <= 1) return 0;
    float r = (float)rand() / (float)RAND_MAX;

    float cumSum = 0.0;
    for (int class = 0; class < numClasses; class++) {
        cumSum += probabilities[class];
        if (r < cumSum) return class;
    }

    return numClasses;
}

static void hop(struct markov_chain* self) {
    self->current_class = sample_discrete_distribution(self->matrix[self->current_class], self->size);
}

static struct markov_chain createRandomChain(const int size) {
    struct markov_chain result;

    result.size = size;
    result.current_class = 0;
    result.matrix = (float**)malloc(size * sizeof(float*));
    for (int i = 0; i < size; i++) { result.matrix[i] = (float*)malloc(size * sizeof(float)); }
    populate_with_rand(result.matrix, size);
    result.hop = hop;

    return result;
}

static void print_2d_array(float** array, const int size) {
    for (int i = 0; i < size; i++) {
        printf("row %d: ", i);
        for (int j = 0; j < size; j++) {
            printf("%f ", array[i][j]);
        }
        printf("\n");
    }
}

int main() {
    srand((unsigned int)time(nullptr));
    struct markov_chain chain = createRandomChain(3);

    printf("Generated following transition matrix:\n");
    print_2d_array(chain.matrix, chain.size);


    for (int i = 0; i < 50; i++) {
        printf("%d\n", chain.current_class);
        hop(&chain);
    }
}