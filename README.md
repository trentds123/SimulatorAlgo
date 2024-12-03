# Scheduling Policies Simulator

## Overview

This project is a **discrete event simulator** that models various CPU scheduling algorithms. Implemented in **C**, it provides a platform to simulate and analyze how different scheduling policies manage job processing in a computing environment.

---

## Features

- **Event-Driven Simulation**: Utilizes an event-driven framework to handle job arrivals and completions over simulated time.

- **Multiple Scheduling Algorithms**: Supports a variety of scheduling policies, including:
  - First-Come, First-Served (**FCFS**)
  - Last-Come, First-Served (**LCFS**)
  - Shortest Job First (**SJF**)
  - Preemptive LCFS (**PLCFS**)
  - Preemptive Shortest Job First (**PSJF**)
  - Shortest Remaining Processing Time (**SRPT**)
  - Processor Sharing (**PS**)
  - Feedback (**FB**)

- **Custom Linked List Implementation**: Includes a linked list data structure with sorting capabilities based on custom comparison functions.

- **Modular Design**: Easily extendable to add or modify scheduling policies and algorithms.

---

## Implementation Details

- **Event Handling**: Jobs are scheduled based on discrete events, specifically job arrivals and completions.

- **Scheduler Interface**: Each scheduling policy implements a common set of functions:
  - **Create**: Initializes scheduler-specific data structures.
  - **Destroy**: Cleans up resources used by the scheduler.
  - **ScheduleJob**: Handles new job arrivals.
  - **CompleteJob**: Manages job completions.

- **Linked List Data Structure**: Used for managing job queues, supporting sorted insertions and efficient access based on scheduling criteria.

- **Processor Sharing (PS) Policy**: Approximated to handle integer time units by distributing available processing time evenly among all jobs and tracking any leftover time for future allocation.

---

## Technical Highlights

- **Event-Driven Programming**: Employed to simulate asynchronous events in a computing system, providing a realistic scheduling environment.

- **Algorithm Implementation**: Translated complex scheduling algorithms into efficient, working code, handling edge cases and ensuring correctness.

- **Data Structures**: Developed custom linked lists and utilized them to manage job queues effectively.

- **Performance Considerations**: Optimized insertion and deletion operations in the linked list for acceptable performance in the simulation context.

---

## Skills Demonstrated

- **C Programming**: Advanced use of pointers, memory management, and struct manipulation.

- **Algorithmic Thinking**: Implemented various scheduling algorithms and understood their impact on job processing and system performance.

- **Data Structures**: Designed and utilized custom data structures tailored to the needs of scheduling algorithms.

- **Event-Driven Design**: Gained proficiency in event-driven programming paradigms applicable to real-world systems.

---

## Project Structure

```plaintext
├── linked_list.c
├── linked_list.h
├── schedulerFCFS.c
├── schedulerLCFS.c
├── schedulerSJF.c
├── schedulerPLCFS.c
├── schedulerPSJF.c
├── schedulerSRPT.c
├── schedulerPS.c
├── schedulerFB.c
├── simulator.c
├── Makefile
└── README.md
