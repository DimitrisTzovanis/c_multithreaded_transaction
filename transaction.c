#include <stdlib.h>
#include <stdio.h>
#include "p3200195-p3200277-p3200057-res.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t lock;
pthread_mutex_t lock2;
pthread_cond_t cond;
pthread_cond_t cond2;
int tel = ntel;
int tam = ncash;
int zoneA[nzone_a][nseat]; //oles oi theseis ths zwnhs A se oles tis seires
int zoneB[nzone_b][nseat];
int *ticketCount; //pinkas arithmos tickets gia kathe nima (teleutaia thesh)
int *pointerCountseat;
int *pointerCountrow;
int *zoneCount;  //pinakas arithmos zonhs gia kathe nima
int bankaccount = 0; //o logariasmos ths etairias sthn trapeza
int success=0; // periptwseis epituxiwn
int cardDeclined=0; //periptwseis akurwshs kartas
int noSeats=0; //periptwseis na mhn yparxoun theseis
double timeSum=0; // synolikos xronos anamonis ston thlefwnhth
double timeSum2=0; // synolikos xronos anamonis ston tamis
double totalTimeSum=0; //o synolikos xronos exipirethshs pelatwn
clock_t t; //xronos
int *clocktable;


void* theater(void* threadId) {
    //caller
    if (pointerCountrow == NULL || pointerCountseat == NULL || zoneCount == NULL ) {
           printf("Memory not allocated.\n");
        exit(0);}
    int *tid;
    tid = (int *)threadId;
    clocktable[*tid] = clock();
    pthread_mutex_lock(&lock);
    t=0;
    t = clock();
    while (tel == 0 ) {  //perimenoume thlefwnhth
        pthread_cond_wait(&cond,&lock);
    }
    t = clock() - t;
    timeSum+=t;
    pthread_mutex_unlock(&lock);
    pthread_mutex_lock(&lock);
    tel--;    //exhpeiraiteitaio epomenos ara exoume -1 thlefwnhtes
    int tickets = (rand() % nseathigh) + nseatlow; //arithmos eisitiriwn
    ticketCount[*tid] = tickets;
    sleep((rand() % tseathigh) + tseatlow); //perimenei thn apanthsh o pelaths
    int i=0; //row
    int j=0; //seat
    int k, l=0;
    int check = 1;
    if (rand() % 100 <= pzone_a) { //pithanothta gia zoneA
        while(i<nzone_a && check){
            while(j<nseat && check){
                if(zoneA[i][j]==0){
                    if(j+tickets<nseat){
                        check=0;
                        pointerCountrow[*tid]=i;
                        pointerCountseat[*tid]=j;
                        zoneCount[*tid] = 1;
                        for(l=j;l<j+tickets;l++){
                            zoneA[i][l]=1;
                        }
                    }
                }
                j++;
            }
            i++;
            j=0;
        }
        if(check==1){
            printf("No seats available\n");
            noSeats+=1;
            tel++;
            pthread_mutex_unlock(&lock);
            pthread_cond_signal(&cond);
            clocktable[*tid] = clock() - clocktable[*tid];
            totalTimeSum += clocktable[*tid];
            pthread_exit(&tid);
        }
    
    }
    else {//zoneB
        while(i<nzone_b && check){
            while(j<nseat && check){
                if(zoneB[i][j]==0){
                    if(j+tickets<nseat){
                        check=0;
                        pointerCountrow[*tid]=i;
                        pointerCountseat[*tid]=j;
                        zoneCount[*tid] = 2;
                        for(l=j;l<j+tickets;l++){
                            zoneB[i][l]=1;
                        }
                    }
                    
                }
                j++;
            }
            i++;
            j=0;
        }
        if(check==1){
            printf("No seats available\n");
            noSeats+=1;
            tel++;
            pthread_mutex_unlock(&lock);
            pthread_cond_signal(&cond);
            clocktable[*tid] = clock() - clocktable[*tid];
            totalTimeSum += clocktable[*tid];
            pthread_exit(&tid);
        }

    }
    tel++;
    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&cond);



    //cashier
    pthread_mutex_lock(&lock);
    int ticketsCost = 0;
    t=0;
    t = clock();
    while (tam == 0) {
        pthread_cond_wait(&cond2, &lock);
    }
    t = clock() - t;
    timeSum2+=t;
    pthread_mutex_unlock(&lock);
    pthread_mutex_lock(&lock);
    tam--;

    sleep((rand() % tcashhigh) + tcashlow); //o pelaths perimenei thn apanthsh tou tamia

    if (rand() % 100 <= pcardsucces){//pithanothta na ginei dekth h karta

        if (zoneCount[*tid] == 1){
            int ticketsCost = czone_a * ticketCount[*tid];
            bankaccount += ticketsCost;
            printf(">Customer %d your purchase costs <%d> euro\n",*tid, ticketsCost);
            printf("Your purchase is complete, your seat is in Zone A\n");
        }
        else{
            int ticketsCost = czone_b * ticketCount[*tid];
            bankaccount += ticketsCost;
            printf(">Customer %d your purchase costs <%d> euro\n",*tid, ticketsCost);
            printf("Your purchase is complete, your seat is in Zone B \n");
            
        }
        int seira = pointerCountrow[*tid]+1;
        printf(", Row <%d>, number: <", seira);
        int seatCount=0;
        for(seatCount = pointerCountseat[*tid]+1; seatCount<=ticketCount[*tid]+pointerCountseat[*tid];seatCount++){
            printf("%d,", seatCount);
        }
        printf(">\n");
        success+=1;

    }
    else{//h karta aporifthike
        if (zoneCount[*tid]==1){
            for(int j=pointerCountseat[*tid];j<ticketCount[*tid]+pointerCountseat[*tid];j++){
                zoneA[pointerCountrow[*tid]][j]=0;
            }
        }
        else{
            for(int j =pointerCountseat[*tid];j<ticketCount[*tid]+pointerCountseat[*tid];j++){
                zoneB[pointerCountrow[*tid]][j]=0;
            }
        }
        
        cardDeclined +=1;
        printf("Card declined\n");
    }
    
    tam++;
    pthread_cond_signal(&cond2);
    pthread_mutex_unlock(&lock);
    clocktable[*tid] = clock() - clocktable[*tid];
    totalTimeSum += clocktable[*tid];
    pthread_exit(&tid);
}




int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <customers> <seed>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int ncust =atoi(argv[1]);
    unsigned int seedp =atoi(argv[2]);
    srand(seedp);

    if (ncust < 0) {
        printf("ERROR: the number of threads to run should be a positive number. Current number given %d.\n", ncust);
        exit(-1);
    }

    //pinakes:
    pointerCountseat = (int *) malloc(sizeof(int) * (ncust*2));
    pointerCountrow = (int *) malloc(sizeof(int) * (ncust*2));
    zoneCount = (int *) malloc(sizeof(int) * ncust);
    clocktable = (int *) malloc(sizeof(int) * ncust);
    ticketCount = (int *) malloc(sizeof(int) * ncust);

    if (pointerCountrow == NULL) {
           printf("Memory not allocated.\n");
           exit(0);
       }
    
    int i;
    int j;
    for(i=0;i<nzone_a;i++){
        for(j=0;j<nseat;j++){
            zoneA[i][j] = 0;}
    }
    for(i=0;i<nzone_b;i++){
        for(j=0;j<nseat;j++){
            zoneA[i][j] = 0;}
    }
    

    pthread_t* threads;

    threads = malloc(ncust * sizeof(pthread_t));
    if (threads == NULL) {
        printf("NOT ENOUGH MEMORY!\n");
        return -1;
    }

    int rc;
    int threadCount;
    int countArray[ncust];
    for (threadCount = 0; threadCount < ncust; threadCount++) {
        printf("Main: creating thread %d\n", threadCount);
        countArray[threadCount] = threadCount + 1;
        /*dimiourgia tou thread*/
        rc = pthread_create(&threads[threadCount], NULL, theater, &countArray[threadCount]);
        
        /*elegxos oti to thread dimiourgithike swsta.*/
        if (rc != 0) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    
    void* status;
    for (threadCount = 0; threadCount < ncust; threadCount++) {
        rc = pthread_join(threads[threadCount], &status);

        if (rc != 0) {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        /*printf("Main: Thread %d returned %d as status code.\n", countArray[threadCount], (*(int*)status));*/
    }


    //TYPOSEIS
    for(int h=1; h<=ncust; h++){
        if (zoneCount[h]==1){
            printf("Zone A/ Seira: %d /Thesh: %d / customer %d,", pointerCountrow[h]+1,pointerCountseat[h]+1, h);
        }
        else{
            printf("Zone B/ Seira: %d /Thesh: %d / customer %d, ", pointerCountrow[h]+1,pointerCountseat[h]+1, h);
        }
    }
    printf("\n Total revenue: %d \n", bankaccount);
    
    int posostoSuccess = ((success * 100)/ncust); // pososto epitixias
    int posostoCardDeclined = ((cardDeclined * 100)/ncust); // pososto apotixias
    int posostoNoSeats = ((noSeats * 100)/ncust);
    double TimeAvgCaller = (timeSum / CLOCKS_PER_SEC)/ncust;
    double TimeAvgCashier = (timeSum2/ ncust)/CLOCKS_PER_SEC;
    double TimeAvg = (totalTimeSum/ncust)/CLOCKS_PER_SEC; //mesos xronos
    
    

    printf("Success persentage is : %d%%, No Seat Available percentage: %d%%, Card Declined Percentage: %d%%\n", posostoSuccess, posostoNoSeats, posostoCardDeclined);

    printf("Mesos xronos anamonhs pelatwn gia ton thlefonhth: %f seconds, mesos xronos anamonhs tamias: %f seconds\n", TimeAvgCaller, TimeAvgCashier);
    
    printf("Mesos xronos eksipirethshs twn pelatwn: %f seconds\n", TimeAvg);



    
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond2);
    pthread_mutex_destroy(&lock2);
    if (pointerCountrow == NULL || pointerCountseat == NULL || zoneCount == NULL ) {
           printf("Memory not allocated.\n");
        exit(0);}
    free(threads);
    free(ticketCount);
    free(zoneCount);
    free(pointerCountseat);
    free(pointerCountrow);
    free(clocktable);

    return 1;
    
   
};
