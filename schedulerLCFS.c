#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"

// LCFS scheduler info
typedef struct {
    list_t* queue;  
    job_t* currentJob; //current job being deleted
} scheduler_LCFS_t;


// Creates and returns scheduler specific info
void* schedulerLCFSCreate() {
    
    scheduler_LCFS_t* info = (scheduler_LCFS_t*)malloc(sizeof(scheduler_LCFS_t));
    if (info == NULL){
        return NULL;  
    }
    info->currentJob = NULL;

    //initialze stack
    info->queue = list_create(NULL);
    if(info->queue == NULL){
        free(info);
        return NULL;
    }
    
    return info;
}

// Destroys scheduler specific info
void schedulerLCFSDestroy(void* schedulerInfo) {
    scheduler_LCFS_t* info = (scheduler_LCFS_t*)schedulerInfo;
      if (info != NULL) {
        
        if (info->queue != NULL) {         
            // Destroy the list itself after all jobs are destroyed
            list_destroy(info->queue);
        }
        free(info);
    }
}

    

void schedulerLCFSScheduleJob(void* schedulerInfo, scheduler_t* scheduler, job_t* job, uint64_t currentTime) {
    scheduler_LCFS_t* info = (scheduler_LCFS_t*)schedulerInfo;
    
    //add the head
    list_insert_head(info->queue, job);

    //if count is at 1, then its readyto be scheduled
    if (list_count(info->queue) == 1) {
        info->currentJob = job;
        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetJobTime(job));

    }
    
}


job_t* schedulerLCFSCompleteJob(void* schedulerInfo, scheduler_t* scheduler, uint64_t currentTime) {
    scheduler_LCFS_t* info = (scheduler_LCFS_t*)schedulerInfo;

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
            
            info->currentJob = (job_t*)(list_data(new_head));

            schedulerScheduleNextCompletion(scheduler, jobGetRemainingTime(info->currentJob) + currentTime);

        }

    }


    return completed_job;
}

