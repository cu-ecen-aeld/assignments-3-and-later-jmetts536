#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

	// TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    	// hint: use a cast like the one below to obtain thread arguments from your parameter
    	//struct thread_data* thread_func_args = (struct thread_data *) thread_param;

	// Lets safely handle NULL pointers before we do anything else
	if(thread_param == NULL)
	{
		// Log an Error indicating that the function was passed a NULL Pointer
		ERROR_LOG("Provided a NULL thread_param Pointer to function threadFunc.  Exiting with failure.");

		// Exit and return the same NULL pointer provided
		return thread_param;
	}

	// Else go ahead and cast the provided void pointer to a thread_data pointer so we can access the members of the thread_data struct
	struct thread_data* thread_func_args = (struct thread_data*) thread_param;

        // Log a Debug Message to Keep Track of Status
        DEBUG_LOG("Thread ID: %lu: Successful entrance to function threadfunc", *thread_func_args->thread_data_thread_id);
        DEBUG_LOG("Thread ID: %lu: Successfully obtained thread_data pointer", *thread_func_args->thread_data_thread_id);
	DEBUG_LOG("Thread ID: %lu: Sleeping for %d ms before acquired Mutex.", *thread_func_args->thread_data_thread_id, thread_func_args->thread_data_wait_to_obtain_ms);

	// Sleep before obtaining Mutex
	if(usleep(thread_func_args->thread_data_wait_to_obtain_ms) != 0)
	{
                // Log an Error indicating a Mutex Error was never handled
                ERROR_LOG("Thread ID: %lu: usleep before Mutex Acquisition failed.", *thread_func_args->thread_data_thread_id);

                // Now that we have logged that the Mutex Error, lets go ahead and clear the error so we can move on
                thread_func_args->thread_data_mutex_error = 0;

                // Lets now indicate that this function had failed using the thread_complete_success Flag in thread_data to FALSE
                thread_func_args->thread_complete_success = false;

                // Exit and return the thread_data struct pointer
                return thread_func_args;
	}

	// Log a Debug Message to Keep Track of Status
	DEBUG_LOG("Thread ID: %lu: Slept for %d ms.  Attempting to acquire Mutex.", *thread_func_args->thread_data_thread_id, thread_func_args->thread_data_wait_to_obtain_ms);
	
	// If the mutex has an outstanding unhandled error
	if(thread_func_args->thread_data_mutex_error != 0)
	{
		// Log an Error indicating a Mutex Error was never handled
		ERROR_LOG("Thread ID: %lu: Mutex Acquisition Blocked by outstanding error: %d", *thread_func_args->thread_data_thread_id, thread_func_args->thread_data_mutex_error);

		// Now that we have logged that the Mutex Error, lets go ahead and clear the error so we can move on
		thread_func_args->thread_data_mutex_error = 0;

		// Lets now indicate that this function had failed using the thread_complete_success Flag in thread_data to FALSE
		thread_func_args->thread_complete_success = false;

		// Exit and return the thread_data struct pointer
		return thread_func_args;
	}

	// Else the Mutex Is safe to acquire, lets grab it before entering our critical section
	thread_func_args->thread_data_mutex_error = pthread_mutex_lock(thread_func_args->thread_data_mutex);

	// If an Error occurred on Mutex Acquisition
	if(thread_func_args->thread_data_mutex_error != 0)
	{
		// Tried to acquire lock, but failed, log an error message
		ERROR_LOG("Thread ID: %lu: Attempted to acquire Mutex. Failed with Error: %d", *thread_func_args->thread_data_thread_id, thread_func_args->thread_data_mutex_error);
		
                // Now that we have logged that the Mutex Error, lets go ahead and clear the error so we can move on
                thread_func_args->thread_data_mutex_error = 0;

                // Lets now indicate that this function had failed using the thread_complete_success Flag in thread_data to FALSE
                thread_func_args->thread_complete_success = false;

                // Exit and return the thread_data struct pointer
                return thread_func_args;
	}

	// Log a Debug Message to Keep Track of Status
	DEBUG_LOG("Thread ID: %lu: Mutex Acquired!", *thread_func_args->thread_data_thread_id);
	DEBUG_LOG("Thread ID: %lu: Entering Critical Section.", *thread_func_args->thread_data_thread_id);
	DEBUG_LOG("Thread ID: %lu: Sleeping for %d ms before releasing Mutex.", *thread_func_args->thread_data_thread_id, thread_func_args->thread_data_wait_to_release_ms);

	// -------------------------------------------------------------------------------------------------------------------------------------
	// Start Critical Section
	// -------------------------------------------------------------------------------------------------------------------------------------

	// Sleep before releasing Mutex
	usleep(thread_func_args->thread_data_wait_to_release_ms);

        // Log a Debug Message to Keep Track of Status
        DEBUG_LOG("Thread ID: %lu: Slept for %d ms.  Attempting to release Mutex.", *thread_func_args->thread_data_thread_id, thread_func_args->thread_data_wait_to_release_ms);

	// -------------------------------------------------------------------------------------------------------------------------------------
	// End Critical Section
	// -------------------------------------------------------------------------------------------------------------------------------------

	// Lets release our Mutex on exit from our critical section
    	thread_func_args->thread_data_mutex_error = pthread_mutex_unlock(thread_func_args->thread_data_mutex);

	// If an Error Occurred on Mutex Release
	if(thread_func_args->thread_data_mutex_error != 0)
	{
		// Tried to release lock, but failed, sful entrance to function threadfunc
		ERROR_LOG("Thread ID: %lu: Attempted to release Mutex.  Failed with Error: %d", *thread_func_args->thread_data_thread_id, thread_func_args->thread_data_mutex_error);
		
                // Now that we have logged that the Mutex Error, lets go ahead and clear the error so we can move on
                thread_func_args->thread_data_mutex_error = 0;

                // Lets now indicate that this function had failed using the thread_complete_success Flag in thread_data to FALSE
                thread_func_args->thread_complete_success = false;

                // Exit and return the thread_data struct pointer
                return thread_func_args;
	}

        // Log a Debug Message to Keep Track of Status
        DEBUG_LOG("Thread ID: %lu: Mutex Released!", *thread_func_args->thread_data_thread_id);
        DEBUG_LOG("Thread ID: %lu: Exited Critical Section.", *thread_func_args->thread_data_thread_id);
        DEBUG_LOG("Thread ID: %lu: threadfunc executed successfully, setting TRUE success status and returning to calling function.", *thread_func_args->thread_data_thread_id);

	// Lets now inficate that this function succeeded using the thread_complete_success Flag in thread_data to TRUE
	thread_func_args->thread_complete_success = true;

	// Exit and return the thread_data struct pointer
	return thread_func_args;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

	// Log a Debug Message to Keep Track of Status
	DEBUG_LOG("Successful entrance to function start_thread_obtaining_mutex");
	DEBUG_LOG("Checking for NULL parameters");

        // Lets safely handle NULL pointers before we do anything else
        if(thread == NULL)
        {
                // Log an Error indicating that the function was passed a NULL Pointer
                ERROR_LOG("Provided a NULL thread Pointer to function start_thread_obtaining_mutex.  Exiting with failure.");

                // Exit with failure status
                return false;
        }
        if(mutex == NULL)
        {
                // Log an Error indicating that the function was passed a NULL Pointer
                ERROR_LOG("Provided a NULL mutex Pointer to function start_thread_obtaining_mutex.  Exiting with failure.");

                // Exit with failure status
                return false;
        }

	// Log a Debug Message to Keep Track of Status
	DEBUG_LOG("No NULL parameters!  Safe to continue");
	DEBUG_LOG("Attempting to allocate memory for thread_data");

	// Lets go ahead and instantiate a thread_data struct pointer for all our thread data
	struct thread_data *local_thread_data_ptr;

    	// Now lets allocate memory for the new struct pointer
	local_thread_data_ptr = (struct thread_data*)malloc(sizeof(struct thread_data));

	// Lets make sure the pointer points somewhere before continuing
	if(local_thread_data_ptr == NULL)
	{
                // Log an Error indicating that we failed to create a new instance of thread_data
                ERROR_LOG("Failed to create a thread_data struct.  Exiting with failure.");

                // Exit with failure status
                return false;
	}

	// Log a Debug Message to Keep Track of Status
	DEBUG_LOG("Successfully allocated memory for thread_data!");
	DEBUG_LOG("Initializing thread_data content");

	// Now lets populate the struct with initial values
	local_thread_data_ptr->thread_data_thread_id = thread;						// Assign thread address to Thread ID Pointer
	local_thread_data_ptr->thread_data_thread_attr = NULL;						// Assign NULL to Thread Attributes to get default attributes
	local_thread_data_ptr->thread_data_thread_error = 0;						// No Thread Error has yet Occurred
	local_thread_data_ptr->thread_data_mutex = mutex;						// Assign Mutex Pointer
	local_thread_data_ptr->thread_data_mutex_error = 0;						// No Mutex Error has yet occurred
	local_thread_data_ptr->thread_data_wait_to_obtain_ms = (unsigned int)wait_to_obtain_ms;		// Cast to Unsigned Int since this is the type usleep takes
	local_thread_data_ptr->thread_data_wait_to_release_ms = (unsigned int)wait_to_release_ms;	// Cast to Unsigned Int since this is the type usleep takes
	local_thread_data_ptr->thread_complete_success = false;						// Thread has not yet completed successfully

	// Log a Debug Message to Keep Track of Status
	DEBUG_LOG("Attempting to create a new thread");

	// Lets go ahead and launch a new thread
	local_thread_data_ptr->thread_data_thread_error = pthread_create(local_thread_data_ptr->thread_data_thread_id,
									local_thread_data_ptr->thread_data_thread_attr, 
		       							threadfunc,
									local_thread_data_ptr);

	// If we encountered an error during thread creation
	if(local_thread_data_ptr->thread_data_thread_error != 0)
	{
                // Log an Error indicating that we failed to create a new thread
                ERROR_LOG("Attempted to create thread.  Failed with Error: %d", local_thread_data_ptr->thread_data_thread_error);

                // Exit with failure status
                return false;
	}

	// Log a Debug Message to Keep Track of Status
	DEBUG_LOG("Thread %lu created successfully!", *local_thread_data_ptr->thread_data_thread_id);
	DEBUG_LOG("Calling function will NOT wait for thread %lu to join.", *local_thread_data_ptr->thread_data_thread_id);

	// If we encountered an error during thread joining
        if(local_thread_data_ptr->thread_data_thread_error != 0)
        {
                // Log an Error indicating that we failed to create a new thread
                ERROR_LOG("Attempted to create thread.  Failed with Error: %d", local_thread_data_ptr->thread_data_thread_error);

                // Exit with failure status
                return false;
        }

	// Log a Debug Message to Keep Track of Status
	DEBUG_LOG("Exiting from function start_thread_obtaining_mutex");

	// Exit with Success Status, we started a thread!
	return true;
}

