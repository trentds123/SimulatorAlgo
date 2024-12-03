#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"

// PSJF scheduler info
typedef struct {
    /* IMPLEMENT THIS */
    list_t* queue;  
    job_t* currentJob;  
    uint64_t lastTimeStarted;// imagine its the arrival time but for everytime it reenters the head
    
} scheduler_PSJF_t;

//compare function will be used to compare the job times
int T(void *data1, void *data2) {
    //get the job and then the time from the job to compare:
    job_t* job1 = (job_t*)data1;
    uint64_t time1 = jobGetJobTime(job1);

    job_t* job2 = (job_t*)data2;
    uint64_t time2 = jobGetJobTime(job2);

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

// Creates and returns scheduler specific info
void* schedulerPSJFCreate()
{
    scheduler_PSJF_t* info = malloc(sizeof(scheduler_PSJF_t));
     if (info == NULL) {
        return NULL;
    }

    info->queue = list_create(T);

    if (info->queue == NULL) {
        free(info); 
        return NULL;
    }

    info->currentJob = NULL;
    info->lastTimeStarted = 0;
    
    /* IMPLEMENT THIS */
    return info;
}

// Destroys scheduler specific info
void schedulerPSJFDestroy(void* schedulerInfo)
{
    scheduler_PSJF_t* info = (scheduler_PSJF_t*)schedulerInfo;
    if (info != NULL) {
        if (info->queue != NULL) {        
            list_destroy(info->queue);
        }

        free(info);
    }
}


// Called to schedule a new job in the queue
// schedulerInfo - scheduler specific info from create function
// scheduler - used to call schedulerScheduleNextCompletion and schedulerCancelNextCompletion
// job - new job being added to the queue
// currentTime - the current simulated time
void schedulerPSJFScheduleJob(void* schedulerInfo, scheduler_t* scheduler, job_t* job, uint64_t currentTime) {
    scheduler_PSJF_t* info = (scheduler_PSJF_t*)schedulerInfo;

 if (info == NULL) {
        return;
    }

    if (jobGetRemainingTime(job) == 0) {
    jobSetRemainingTime(job, jobGetJobTime(job));
    }
    
    // Insert the job into the queue
    list_insert(info->queue, job);

    // If this job is now the head of the queue 

    if (list_head(info->queue)->data == job) {
        if (info->currentJob != NULL && info->currentJob != job) {

            // Preempt the current job
            uint64_t timeElapsed = currentTime - info->lastTimeStarted;
            jobSetRemainingTime(info->currentJob, jobGetRemainingTime(info->currentJob) - timeElapsed);

           //re insert becuase it adds 2%
            list_remove(info->queue, list_find(info->queue, info->currentJob));
            list_insert(info->queue, info->currentJob);

            schedulerCancelNextCompletion(scheduler);
        }

        // Schedule the new job as it has the shortest remaining time
        info->currentJob = job;
        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetRemainingTime(job));
        info->lastTimeStarted = currentTime;
    }
}
// Called to complete a job in response to an earlier call to schedulerScheduleNextCompletion
// schedulerInfo - scheduler specific info from create function
// scheduler - used to call schedulerScheduleNextCompletion and schedulerCancelNextCompletion
// currentTime - the current simulated time
// Returns the job that is being completed
job_t* schedulerPSJFCompleteJob(void* schedulerInfo, scheduler_t* scheduler, uint64_t currentTime) {
    scheduler_PSJF_t* info = (scheduler_PSJF_t*)schedulerInfo;

    if (info->currentJob == NULL) {
        return NULL;
    }
    job_t* completedJob = info->currentJob;

    list_node_t* headNode = list_head(info->queue);
    if (headNode != NULL) {
        list_remove(info->queue, headNode);
    }
    headNode = list_head(info->queue);
    if (headNode != NULL) {
        job_t* nextJob = (job_t*)list_data(headNode);
        
        //set the remainder of the prev job
        
        info->currentJob = nextJob;
        // Schedule the next completion
        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetRemainingTime(nextJob));
        info->lastTimeStarted = currentTime;
    } else {
        // No more jobs in the queue
        info->currentJob = NULL;
    }

    return completedJob;
}

