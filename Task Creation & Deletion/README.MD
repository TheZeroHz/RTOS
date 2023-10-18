# FreeRTOS for ESP32

## Introduction

FreeRTOS is a popular real-time operating system for microcontrollers and embedded systems. This README provides an overview of essential concepts and functions related to FreeRTOS on the ESP32 platform.

## TaskHandle_t

`TaskHandle_t` is a data type in FreeRTOS used to reference tasks. When you create a task using functions like `xTaskCreate`, it returns a `TaskHandle_t` variable that allows you to manage and interact with the task. For example, you can use `TaskHandle_t` as a parameter for the `vTaskDelete` function to delete a task.

## xTaskCreate Function

### Description

The `xTaskCreate` function is used to create a new task in FreeRTOS and add it to the list of tasks that are ready to run. This function is a fundamental part of task management in FreeRTOS.

### Parameters

- `pvTaskCode`: A pointer to the task entry function. This is the function that implements the task's behavior. Tasks in FreeRTOS are typically implemented as infinite loops, and this function must never attempt to return or exit.

- `pcName`: A descriptive name for the task, primarily used for debugging purposes.

- `usStackDepth`: The number of words to allocate for the task's stack. This parameter specifies the stack size.

- `pvParameters`: A value that is passed as a parameter to the created task. This can be used to pass information to the task.

- `uxPriority`: The priority at which the created task will execute.

- `pxCreatedTask`: An optional parameter that allows you to obtain a handle to the created task.

### Return Value

- If the task is created successfully, `pdPASS` is returned. Otherwise, `errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY` is returned if there is insufficient memory.

## vTaskDelete Function

### Description

The `vTaskDelete` function is used to remove a task from the FreeRTOS kernel's management. When you call this function, the task being deleted is removed from all ready, blocked, suspended, and event lists.

### Parameters

- `xTask`: The handle of the task to be deleted. Passing `NULL` will cause the calling task to be deleted.

### Note

- Memory allocated by the task code is not automatically freed and should be released before deleting the task. The idle task is responsible for freeing kernel-allocated memory from deleted tasks.


## Contact

For questions, suggestions, or support, you can contact us at [Rakib Hasan](mailto:rakibhasan01316980149@gmail.com).

