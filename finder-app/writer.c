// This is a C File for the Writer implementation
//
// Write a script that takes two arguments
// Argument 1 - writeFile -  specifies the full path to a file
// Argument 2 - writeStr -  specifies a text string which will be written to writeFile
// Script should exit with 1 error and error message if any parameter not specified
// Script should overwrite any existing file, or create the file, if writeFile does not exist
// Script should exit with 1 error and error message if file could not be created
// This utility does not need to test for and create directories, we can assume that the caller has created the correct directories
// Must setup syslog Logging using the LOG_USER facility
// Syslog should be used to log "Writing writeStr to writeFile" with the LOG_DEBUG level
// Syslog should be used to log any unexpected errors with the LOG_ERR level
//

//------------------------------------INCLUDES------------------------------------
#include <syslog.h>
#include <stdio.h>

//------------------------------------DEFINES-------------------------------------

// Set to 1 to enable syslog logging
#define ENABLE_LOGGING 1

// Set tp 1 to enable non syslog terminal outputs
#define ENABLE_PRINTING 0


//------------------------------PRIVATE DECLARATIONS------------------------------

/**
 * @brief - Private function that implements writer.sh logic
 * @param - arg1 - The path to the file to write
 * @param - arg2 - The string to write to arg1
 * @return - 0 for Success, 1 Otherwise
 */
int writer(const char* filePath, const char* writeString);

//--------------------------------------MAIN--------------------------------------

/**
 * @brief
 * @param
 * @param
 * @return
 */
int main(int argc, char *argv[])
{
	// If anything other than two arguments were passed to the script
	if(argc != 3)
	{
		// Ifglobal file terminal fprintf logging is enabled
		if(ENABLE_PRINTING)
		{
			// Print an Error Message to the Terminal
			fprintf(stderr, "Invalid number of arguments. \
					\nGot %d arguments. \
					\nExpected 2 arguments. \
					\nwriteFile - The file to write to. \
					\nwriteStr - The string to write to writeFile.", argc);
		}

		// If global file sysLogging is enabled
		if(ENABLE_LOGGING)
		{
			// Open a Log and Log an Error Message
			openlog(NULL, 0, LOG_USER);
			syslog(LOG_ERR, "Invalid number of arguements.  This file accepts 2 arguments.");
		}
		
		// Return with error code
		return 1;
	}

	// Else we are safe to call the writer function
	else
	{
		// argv[1] will contain the first argument passed to the file (filePath)
		// argv[2] will contain the second argument passed to the file (writeStr)
		return writer(argv[1], argv[2]);
	}
}

//-------------------------------PRIVATE DEFINITIONS------------------------------

int writer(const char* filePath, const char* writeStr)
{
	// Lets create a file object to manipulate
	FILE *outFile;

	// Lets open the file for writing, if it exists, overwrite, else create
	outFile = fopen(filePath, "w");
	
	// If the outFile did not open succesffully, we need to log error and exit
	if(!outFile)
	{
		// Print the error to the terminal
		if(ENABLE_PRINTING)
		{
			fprintf(stdout, "Error opening file %s", filePath);
		}

		// Log the error to LOG_ERR
		if(ENABLE_LOGGING)
		{
			openlog(NULL, 0, LOG_USER);
			syslog(LOG_ERR, "Unable to create the requested writeFile - %s", filePath);
		}

		return 1;
	}
	
	// Else we can perform requested write
	else
	{
		// Log the write to LOG_DEBUG
		if(ENABLE_LOGGING)
		{
			openlog(NULL, 0, LOG_USER);
			syslog(LOG_DEBUG, "Writing writeStr to writeFile");
		}

		// Perform the write, close the file, and exit with success
		fputs(writeStr, outFile);
		fclose(outFile);
		return 0;
	}
}

