struct mChain {
    int size;
    float** matrix;
    void (*hop)(struct mChain* self);
};

