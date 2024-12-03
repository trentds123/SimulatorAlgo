#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"
#include <stdio.h>

// SRPT scheduler info
typedef struct {
    /* IMPLEMENT THIS */
    list_t* queue;  
    job_t* currentJob;  
    uint64_t lastTimeStarted;// imagine its the arrival time but for everytime it reenters the head
    
} scheduler_SRPT_t;

//Need this function so that when I call linked lst It will automaticcly sort by time.


int compareRemainingTime(void *data1, void *data2) {
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


// Creates and returns scheduler specific info
void* schedulerSRPTCreate()
{
    scheduler_SRPT_t* info = malloc(sizeof(scheduler_SRPT_t));
    if (info == NULL) {
        return NULL;
    }

    info->queue = list_create(compareRemainingTime);

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
void schedulerSRPTDestroy(void* schedulerInfo)
{
    scheduler_SRPT_t* info = (scheduler_SRPT_t*)schedulerInfo;
    if (info != NULL) {
        if (info->queue != NULL) {        
            list_destroy(info->queue);
        }

        free(info);
    }
}

void schedulerSRPTScheduleJob(void* schedulerInfo, scheduler_t* scheduler, job_t* job, uint64_t currentTime) {
    scheduler_SRPT_t* info = (scheduler_SRPT_t*)schedulerInfo;

   
 if (info == NULL) {
        return;
    }

    if (jobGetRemainingTime(job) == 0) {
    jobSetRemainingTime(job, jobGetJobTime(job));
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

        //check if the job that was just inserted or updated in the quueu is now at the head of the q
            // if (info->currentJob != NULL && info->currentJob != job) {
               
               
                
            //     if(jobGetRemainingTime(info->currentJob) > jobGetRemainingTime(job)){    
                    

            //         schedulerCancelNextCompletion(scheduler);
            //     }
            // }

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
job_t* schedulerSRPTCompleteJob(void* schedulerInfo, scheduler_t* scheduler, uint64_t currentTime) {
    scheduler_SRPT_t* info = (scheduler_SRPT_t*)schedulerInfo;

    // Ensure there is a current job
    if (info->currentJob == NULL) {
        return NULL;
    }
    job_t* completedJob = info->currentJob;

    list_node_t* headNode = list_head(info->queue);
    if (headNode != NULL ) {
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
    
    
    printf("%ld\n", jobGetId(completedJob));
    return completedJob;
}
