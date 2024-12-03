#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"
#include <stdio.h>


typedef struct {
    list_t* queue;  
    job_t* currentJob;
    uint64_t lastTimeRecorded;
    uint64_t leftoverTime;

} scheduler_PS_t;

int compareRemainingT(void *data1, void *data2) {
    //get the job and then the time from the job to compare:
    job_t* job1 = (job_t*)data1;
    uint64_t time1 = jobGetRemainingTime(job1);

    job_t* job2 = (job_t*)data2;
    uint64_t time2 = jobGetRemainingTime(job2);

    //job2 goes AFTER job 1
    if (time1 < time2){
        return -1;
    }

    //job1 goes AFTER job 2
    if (time1 > time2){
        return 1;
    }   

    if (jobGetId(job1) < jobGetId(job2)){
        return -1;
    }

    //job1 goes AFTER job 2
    if (jobGetId(job1) > jobGetId(job2)){
        return 1;
    }   
    //both jobs are EQUAL
    
    return 0; 
}

//needed to find shortest priority 

// Creates and returns scheduler specific info
void* schedulerPSCreate() {
    scheduler_PS_t* info = malloc(sizeof(scheduler_PS_t));
    if (info == NULL) {
        return NULL; 
    }
    //create compare function.
    info->queue = list_create(compareRemainingT);
    
    if (info->queue == NULL) {
        free(info); 
        return NULL;
    }
    info->currentJob = NULL;
    info->lastTimeRecorded = 0;
    info->leftoverTime = 0; 

    return info; 
}


// Destroys scheduler specific info
void schedulerPSDestroy(void* schedulerInfo)
{
    scheduler_PS_t* info = (scheduler_PS_t*)schedulerInfo;
    if (info != NULL) {
        
        if (info->queue != NULL) {         
            // Destroy the list itself after all jobs are destroyed
            list_destroy(info->queue);
        }
        free(info);
    }
}

//helper function 
void updateRemainingTimes(scheduler_PS_t* info, uint64_t currentTime) {
    if (list_count(info->queue) == 0) {
        return;  
    }

    //handle first case 
    uint64_t timeElapsed = currentTime - info->lastTimeRecorded;
    if(timeElapsed == 0 && list_count(info->queue) > 0) {
            timeElapsed = 1;  
    }
printf("Before R time %ld\n", jobGetRemainingTime(info->currentJob));

       // printf("timeElaspes %ld\n", timeElapsed);

    
        uint64_t totalJobs = list_count(info->queue);
      //  printf("totalJobs %ld\n", totalJobs);

    uint64_t sharedWork = timeElapsed / totalJobs;

         //   printf("shared work %ld\n", sharedWork);

    uint64_t leftoverWork = timeElapsed % totalJobs;
    //printf("leftOverWrok %ld\n", leftoverWork);

    

    list_node_t* node = list_head(info->queue);

    while (node != NULL) {

        uint64_t newRemainingTime = jobGetRemainingTime(info->currentJob) - sharedWork;
        jobSetRemainingTime(info->currentJob, newRemainingTime);
        node = list_next(node);
    }

    info->leftoverTime += leftoverWork;
}

void schedulerPSScheduleJob(void* schedulerInfo, scheduler_t* scheduler, job_t* job, uint64_t currentTime) {
    scheduler_PS_t* info = (scheduler_PS_t*)schedulerInfo;
    if (info == NULL) {
        return;
    }

    if (list_count(info->queue) == 0) {

        // Special handling for the very first job
        jobSetRemainingTime(job, jobGetJobTime(job));
        list_insert(info->queue, job);
        
        info->currentJob = job;
        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetRemainingTime(job));
        info->lastTimeRecorded = currentTime;
    } else {

        updateRemainingTimes(info,currentTime);
        list_insert(info->queue, job);

        schedulerCancelNextCompletion(scheduler);

        printf("Test entering %ld\n\n", jobGetArrivalTime(job));

        

    
    
        printf("After %ld\n\n", jobGetRemainingTime(job));

        // list_remove(info->queue, list_find(info->queue, info->currentJob));
        // list_insert(info->queue, info->currentJob);
        
        // Cancel any existing completion event because the system state is about to change
       
        //Uuse this to find the shared work and leftovertime

        // Always reschedule based on the new head of the queue
        info->currentJob = list_data(list_head(info->queue));
        uint64_t newSliceSize = jobGetRemainingTime(list_data(list_head(info->queue)) + (info->leftoverTime / list_count(info->queue))); 
        schedulerScheduleNextCompletion(scheduler, currentTime + newSliceSize);
        info->lastTimeRecorded = currentTime;
    }
}




// Called to complete a job in response to an earlier call to schedulerScheduleNextCompletion
// schedulerInfo - scheduler specific info from create function
// scheduler - used to call schedulerScheduleNextCompletion and schedulerCancelNextCompletion
// currentTime - the current simulated time
// Returns the job that is being completed
job_t* schedulerPSCompleteJob(void* schedulerInfo, scheduler_t* scheduler, uint64_t currentTime) {
    scheduler_PS_t* info = (scheduler_PS_t*)schedulerInfo;

    if (info == NULL) {
        return NULL; 
    }
    
    job_t* completedJob = info->currentJob;

    list_node_t* headNode = list_head(info->queue);
    if (headNode != NULL ) {
        list_remove(info->queue, headNode);
    }

    
    //UPDATE the nodes
    if (list_count(info->queue) > 0) {
        //Uuse this to find the shared work and leftovertime

        uint64_t timeElapsed = currentTime - info->lastTimeRecorded;
    uint64_t totalJobs = list_count(info->queue);
    uint64_t sharedWork = timeElapsed / totalJobs;
    uint64_t leftoverWork = timeElapsed % totalJobs;

    list_node_t* node = list_head(info->queue);
    while (node != NULL) {
        uint64_t newRemainingTime = jobGetRemainingTime(info->currentJob) - sharedWork;
        jobSetRemainingTime(info->currentJob, newRemainingTime);
        node = list_next(node);
    }

    info->leftoverTime += leftoverWork;
        job_t* nextJob = (job_t*)list_data(list_head(info->queue));
        
        info->currentJob = nextJob;
        // Schedule the next completion
        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetRemainingTime(nextJob));
        info->lastTimeRecorded = currentTime;
    }
    else {
        info->currentJob = NULL; 
    }


    return completedJob; 
}

