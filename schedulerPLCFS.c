#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"

// PLCFS scheduler info
typedef struct {
    list_t* queue;  
    job_t* currentJob; //current job being deleted
    uint64_t lastMarkedTime;
} scheduler_PLCFS_t;

// Creates and returns scheduler specific info
void* schedulerPLCFSCreate()
{
    scheduler_PLCFS_t* info = malloc(sizeof(scheduler_PLCFS_t));
    if (info == NULL) {
        return NULL;
    }
    info->currentJob = NULL;
    info->lastMarkedTime =0;
    //initialze stack
    info->queue = list_create(NULL);
    if(info->queue == NULL){
        free(info);
        return NULL;
    }
    
    return info;
}

// Destroys scheduler specific info
void schedulerPLCFSDestroy(void* schedulerInfo)
{
    scheduler_PLCFS_t* info = (scheduler_PLCFS_t*)schedulerInfo;
     if (info != NULL) {
        
        if (info->queue != NULL) {         
            // Destroy the list itself after all jobs are destroyed
            list_destroy(info->queue);
        }
        free(info);
    }
}


void schedulerPLCFSScheduleJob(void* schedulerInfo, scheduler_t* scheduler, job_t* job, uint64_t currentTime)
{
    //When preumpting The job newest job has highest priority.
    //cacnell the current completion event
   
    scheduler_PLCFS_t* info = (scheduler_PLCFS_t*)schedulerInfo;
 
    // if theres a current job running then cacell it 
    if(list_head(info->queue) != NULL){
        uint64_t remainingTime = jobGetRemainingTime(list_data(list_head(info->queue))) - (currentTime - info->lastMarkedTime);
        jobSetRemainingTime(list_data(list_head(info->queue)), remainingTime);

        schedulerCancelNextCompletion(scheduler);
    }

    list_insert_head(info->queue, job);

    //schedule nezt job completion and update job
    schedulerScheduleNextCompletion(scheduler, currentTime + jobGetJobTime(job));

    info->currentJob = job;
    info->lastMarkedTime = currentTime;
    
}



job_t* schedulerPLCFSCompleteJob(void* schedulerInfo, scheduler_t* scheduler, uint64_t currentTime)
{
    //SAME CODE------
    
    scheduler_PLCFS_t* info = (scheduler_PLCFS_t*)schedulerInfo;
    
    // Check if there are any jobs in the queue
    if (list_count(info->queue) == 0) {
        return NULL;  // No jobs to complete
    }

    //TO BE REMOVED
    // Get the back of the queue 
    list_node_t* node = list_find(info->queue,info->currentJob);
    job_t* completed_job = info->currentJob;

    //remove the completed job    
    list_remove(info->queue, node);

    //if there are more jobs in the Q, have the next head be the one shceduled
    if(list_count(info->queue)>0){

        list_node_t* new_head = list_head(info->queue);
        if(new_head != NULL){
            
            info->lastMarkedTime = currentTime;
            info->currentJob = (job_t*)(list_data(new_head));

            schedulerScheduleNextCompletion(scheduler, jobGetRemainingTime(info->currentJob) + currentTime);

        }

    }


    return completed_job;
}

