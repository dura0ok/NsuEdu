struct Queue {
    unsigned char *data;
    int size;
    int front;
    int rear;
};

//int isFull(struct Queue *q);
//int isEmpty(struct Queue *q);
struct Queue * createQueue(int needleSize);
//void Enqueue(struct Queue *q, unsigned char data);
//unsigned char Dequeue(struct Queue *q);
void destroy(struct Queue * q);
void checkBySymbols(struct Queue *q, const char needle[], long long startIndex);
