#!/bin/sh

# Write a script that takes two arguments
# Argument 1 - filesdir -  specifies a filesystem path
# Argument 2 - searchstr -  specifies a test string to search for in filesdir
# Script should exit with 1 error and error message if any parameter not specified
# Script should exit with 1 error and error message if filesdir dows not exist
# Script should print "The number of files are X and the number of matching lines are Y"

# Check that two arguments were passed to the script
if [ $# -ne 2 ]
then
	# Print failure message to terminal
	echo -e "Invalid number of arguments.\nThis script accepts two arguments.\nfilesDir - The directory to search.\nsearchStr - The string to find at the specified directoy."
	
	# Exit the script with an error
	exit 1
fi

# Check that the filesdir provided is a path that exists
if [ ! -d "$1" ]
then
	# Print failure message to terminal
	echo -e "Invalid directory provided for fileDir.\n$1 is not a directory."

	# Exit the script with an error
	exit 1
fi

# Now that we know we have the proper number of arguments and we know the directory exists, we can move on

# Determine the number of files that contain searchstr in the filesdir
# Pipe the name of all files in $1 which contain $2 to wc which will count the number of files provided 
NUMFILES=$(grep -r -l "$2" "$1"* | wc -l)

# Determine the number of matching patterns
# Pipe all lines from all files in $1 which contain $2 to wc which will count the number of lines provided
NUMMATCH=$(grep -r "$2" "$1"* | wc -l)

# Now print our findings to the Terminal
echo "The number of files are ${NUMFILES} and the number of matching lines are ${NUMMATCH}"

# Exit with success
exit 0
