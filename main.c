#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lib/cJSON.h"

static const char* TEXT_SOURCE = "./probabilities.mrkv";

struct markov_chain {
    int size;
    int current_class;
    float** matrix;
    char** labels;
    void (*hop)(struct markov_chain* self);
};

static void normalize_matrix(float** matrix, const int size) {
    for (int i = 0; i < size; i++) {
        float rowSum = 0.0f;
        for (int j = 0; j < size; j++) {
            rowSum += matrix[i][j];
        }

        for (int j = 0; j < size; j++) {
            matrix[i][j] /= rowSum;
        }
    }
}

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
    result.labels = 0;
    result.matrix = (float**)malloc(size * sizeof(float*));
    for (int i = 0; i < size; i++) { result.matrix[i] = (float*)malloc(size * sizeof(float)); }
    populate_with_rand(result.matrix, size);
    result.hop = hop;

    return result;
}

static struct markov_chain createMarkovChain(const int size, float** transitionMatrix) {
    struct markov_chain result;

    result.size = size;
    result.current_class = 0;
    result.labels = 0;
    result.matrix = transitionMatrix;
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

static int string_to_class(const char* string, char** classes, const int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(string, classes[i]) == 0) {return i;}
    }
    return -1;
}

static char* class_to_string(const int class, char** classes, const int size) {
    if (class >= size) return "";
    return classes[class];
}

const cJSON* parse_mrkv_file() {
    FILE *textSamples = fopen(TEXT_SOURCE, "rb");
    if (!textSamples) {
        fprintf(stderr, "Text file not found");
        return NULL;
    }

    fseek(textSamples, 0, SEEK_END);
    long length = ftell(textSamples);
    fseek(textSamples, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Couldn't allocate memory for the given text file.");
        fclose(textSamples);
        return NULL;
    }

    const size_t read_bytes = fread(buffer, 1, length, textSamples);
    buffer[read_bytes] = '\0';
    fclose(textSamples);
    cJSON* sourceText = cJSON_Parse(buffer);
    free(buffer);
    return sourceText;
}

static void populate_classes(const cJSON* sourceText, char** classes) {
    const cJSON* keyNode = NULL;
    int i = 0;
    cJSON_ArrayForEach(keyNode, sourceText) {
        classes[i] = strdup(keyNode->string);
        //printf("%s\n", classes[i]);
        i++;
    }
}

static void populate_transition_matrix(const cJSON* sourceText, char** classes, const int entries, float** transitionMatrix) {
    const cJSON* keyNode = NULL;
    cJSON_ArrayForEach(keyNode, sourceText) {
        const cJSON *item = NULL;
        const int currentClass = string_to_class(keyNode->string, classes, entries);
        cJSON_ArrayForEach(item, keyNode) {
            char pair[128];
            if (sscanf(keyNode->string, "%*s %49s", pair) != 1) continue;

            strcat(pair, " ");
            strcat(pair, item->valuestring);

            //printf("%s ", pair);

            const int transitionClass = string_to_class(pair, classes, entries);
            if (transitionClass != -1) {
                transitionMatrix[currentClass][transitionClass] += 1;
                //printf("increment");
            }

            //printf("\n");
        }
    }
}

static struct markov_chain* create_text_markov_chain() {
    const cJSON* sourceText = parse_mrkv_file();
    int entries = cJSON_GetArraySize(sourceText);
    char** classes = malloc(entries * sizeof(*classes));
    float** transitionMatrix = calloc(entries, sizeof(float*));
    for (int i = 0; i < entries; i++)
        transitionMatrix[i] = calloc(entries, sizeof(float));

    if (classes == NULL) {
        fprintf(stderr, "Couldn't allocate memory for class identifiers.");
        return NULL;
    }

    populate_classes(sourceText, classes);
    printf("Loaded %d combinations.\n", entries);

    populate_transition_matrix(sourceText, classes, entries, transitionMatrix);
    normalize_matrix(transitionMatrix, entries);

    struct markov_chain* chain = malloc(sizeof(*chain));
    if (!chain) {
        fprintf(stderr, "Couldn't allocate memory for chain.");
        return NULL;
    }

    *chain = createMarkovChain(entries, transitionMatrix);
    chain->labels = classes;
    return chain;
}

static void run_text_markov_chain() {
    struct markov_chain* chain = create_text_markov_chain();

    for (int i = 0; i < 1200; i++) {
        char* classString = strdup(class_to_string(chain->current_class, chain->labels, chain->size));
        if (i == 0)
            printf("%s ", classString);
        else {
            char res[128];
            if (sscanf(classString, "%*s %49s", res) != 1) continue;
            printf("%s ", res);
        }

        if (i > 0 && i % 30 == 0) printf("\n");
        fflush(stdout);
        hop(chain);

        if (chain->current_class == -1 || chain->current_class == chain->size) {chain->current_class = 0;}
    }
}

static void run_random_markov_chain_example() {
    struct markov_chain chain = createRandomChain(5);

    printf("Generated following transition matrix:\n");
    print_2d_array(chain.matrix, chain.size);

    for (int i = 0; i < 50; i++) {
        printf("%d\n", chain.current_class);
        hop(&chain);
    }
}

int main() {
    srand((unsigned int)time(NULL));
    run_text_markov_chain();
}