#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"

// FCFS scheduler info
typedef struct {
    list_t* queue;  // essential

} scheduler_FCFS_t;

// Creates and returns scheduler specific info
void* schedulerFCFSCreate() {
    scheduler_FCFS_t* info = (scheduler_FCFS_t*)malloc(sizeof(scheduler_FCFS_t));
   
    if (info == NULL) {
        return NULL;  
    }
    //initialize data
    info->queue = list_create(NULL);  
    if (info->queue == NULL) {
        free(info);  
        return NULL;
    }

    return info;
}


// Destroys scheduler specific info
void schedulerFCFSDestroy(void* schedulerInfo) {

    scheduler_FCFS_t* info = (scheduler_FCFS_t*)schedulerInfo;
    if (info != NULL) {
        if (info->queue != NULL) {        
            list_destroy(info->queue);
        }

        free(info);
    }
}


void schedulerFCFSScheduleJob(void* schedulerInfo, scheduler_t* scheduler, job_t* job, uint64_t currentTime) {
    
    scheduler_FCFS_t* info = (scheduler_FCFS_t*)schedulerInfo;
    
    list_insert_tail(info->queue,job);

    if (list_count(info->queue) == 1) {

        // Schedule the completion of the job
        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetJobTime(job));
    }

}


job_t* schedulerFCFSCompleteJob(void* schedulerInfo, scheduler_t* scheduler, uint64_t currentTime) {
    scheduler_FCFS_t* info = (scheduler_FCFS_t*)schedulerInfo;
    
    // Check if there are any jobs in the queue
    if (list_count(info->queue) == 0) {
        return NULL;  // No jobs to complete
    }

    // Get the job at the front of the queue 
    list_node_t* node = list_head(info->queue);
    job_t* completed_job = (job_t*)list_data(node);

    // Remove the completed job from the queue
    list_remove(info->queue, node);

    // Check if there are more jobs in the queue to be scheduled for completion
    if (list_count(info->queue) > 0) {
        // Get the next job in the queue
        job_t* nextJob = (job_t*)list_data(list_head(info->queue));
        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetRemainingTime(nextJob));
    }

    return completed_job;
}

