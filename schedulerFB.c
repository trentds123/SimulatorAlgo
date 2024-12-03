#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"

// FB scheduler info
typedef struct {
     list_t* queue;  
    job_t* currentJob;  
    uint64_t lastTimeStarted;// imagine its the arrival time but for everytime it reenters the head
    
} scheduler_FB_t;

// Creates and returns scheduler specific info
void* schedulerFBCreate()
{
    scheduler_FB_t* info = malloc(sizeof(scheduler_FB_t));
  if (info == NULL) {
        return NULL;
    }

    info->queue = list_create(NULL);

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
void schedulerFBDestroy(void* schedulerInfo)
{
    scheduler_FB_t* info = (scheduler_FB_t*)schedulerInfo;
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
void schedulerFBScheduleJob(void* schedulerInfo, scheduler_t* scheduler, job_t* job, uint64_t currentTime)
{
    scheduler_FB_t* info = (scheduler_FB_t*)schedulerInfo;
if (info == NULL) {
        return;
    }

    
    // Insert the job into the queue
    list_insert(info->queue, job);

    // If this job is now the head of the queue 
    
    if(list_count(info->queue) == 1){
        info->currentJob = job;
        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetRemainingTime(job));
        info->lastTimeStarted = currentTime;
    }
    else{

        //Here its already automatically going to prempt removing and re adding it automatically
        uint64_t timeElapsed = currentTime - info->lastTimeStarted;
        jobSetRemainingTime(info->currentJob, jobGetRemainingTime(info->currentJob) - timeElapsed);

        list_remove(info->queue, list_find(info->queue, info->currentJob));
        list_insert(info->queue, info->currentJob);
        schedulerCancelNextCompletion(scheduler);

        // Schedule the new job as it has the shortest remaining time
        info->currentJob = list_data(list_head(info->queue));
        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetRemainingTime(list_data(list_head(info->queue))));
        info->lastTimeStarted = currentTime;
    }
}

// Called to complete a job in response to an earlier call to schedulerScheduleNextCompletion
// schedulerInfo - scheduler specific info from create function
// scheduler - used to call schedulerScheduleNextCompletion and schedulerCancelNextCompletion
// currentTime - the current simulated time
// Returns the job that is being completed
job_t* schedulerFBCompleteJob(void* schedulerInfo, scheduler_t* scheduler, uint64_t currentTime)
{
    scheduler_FB_t* info = (scheduler_FB_t*)schedulerInfo;
    /* IMPLEMENT THIS */
    return NULL;
}
