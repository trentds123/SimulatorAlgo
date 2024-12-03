#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"

// SJF scheduler info
typedef struct {
    list_t* queue; 
    job_t* currentJob; 
} scheduler_SJF_t;

// Creates and returns scheduler specific info

//compare function will be used to compare the job times
int compare(void *data1, void *data2) {
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


void* schedulerSJFCreate()
{
    scheduler_SJF_t* info = malloc(sizeof(scheduler_SJF_t));
    if (info == NULL) {
        return NULL;
    }
    
    info->queue = list_create(compare);

    if (info->queue == NULL) {
        free(info); 
        return NULL;
    }

    info->currentJob = NULL;

    return info;

}

// Destroys scheduler specific info
void schedulerSJFDestroy(void* schedulerInfo)
{
    scheduler_SJF_t* info = (scheduler_SJF_t*)schedulerInfo;
    if (info != NULL) {
        if (info->queue != NULL) {        
            list_destroy(info->queue);
        }

        free(info);
    }
}





void schedulerSJFScheduleJob(void* schedulerInfo, scheduler_t* scheduler, job_t* job, uint64_t currentTime)
{
    scheduler_SJF_t* info = (scheduler_SJF_t*)schedulerInfo;
   
    if(info == NULL){
        return;
    }
    
    
    //insert does the sorting. asume already sorted after calling this function
    list_insert(info->queue,job);

    //if no jobs are curr running then schedule it. 
    //dont want to have it sit idle. when epty imediatly grab new task.
    if(info->currentJob == NULL){

        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetJobTime(job));
        info->currentJob = job;
    }
    //DO nothing here becuase in non preemptive the jobs will just wait for it to finish and then go


}


job_t* schedulerSJFCompleteJob(void* schedulerInfo, scheduler_t* scheduler, uint64_t currentTime) {
    scheduler_SJF_t* info = (scheduler_SJF_t*)schedulerInfo;

    // Check if there are any jobs in the queue
    if (list_count(info->queue) == 0) {
        return NULL;  // No jobs to complete
    }

    //SJF shortest will always be the head
    list_node_t* head = list_head(info->queue);
    job_t* completed_job = info->currentJob;

    list_remove(info->queue, head);
    
     if(list_count(info->queue)>0){

        //The shortest will always be the head so get the next head if there is one.
        list_node_t* new_head = list_head(info->queue);

        if(new_head != NULL){
            //update the current job and shcedule the next
            info->currentJob = (job_t*)(list_data(new_head));
            schedulerScheduleNextCompletion(scheduler, jobGetRemainingTime(info->currentJob) + currentTime);

        }

    }
   

    return completed_job;
}

