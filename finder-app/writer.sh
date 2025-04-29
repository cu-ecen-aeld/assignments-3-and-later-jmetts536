# Write a script that takes two arguments
# Argument 1 - writeFile -  specifies the full path to a file
# Argument 2 - writeStr -  specifies a text string which will be written to writeFile
# Script should exit with 1 error and error message if any parameter not specified
# Script should overwrite any existing file, or create the file, if writeFile does not exist
# Script should exit with 1 error and error message if file could not be created

#SHEBANG
#!/bin/sh

# Check that two arguments were passed to the script
if [ $# -ne 2 ]
then
        # Print failure message to terminal
        echo -e "Invalid number of arguments.\nThis script accepts two arguments.\nwriteFile - The file to write to.\nwriteStr - The string to write to writeFile."

        # Exit the script with an error
        exit 1
fi

# Lets extract the parent directory tree from the file path and if it doesn't exist, lets create it
if [ ! -d "$(dirname "$1")" ]
then
	mkdir -p "$(dirname "$1")"
fi

# Go ahead and verify the requested writeFile exists, and if it doesnt, create it
touch $1

# If the exit status of touch is anything other than 0, touch failed, therefore file was inaccessable or otherwise uncreatable
if [ $? -ne 0 ]
then
	# Print failure message to terminal
	echo -e "Unable to create the requested writeFile.\n$1"
	
	# Exit the script with an error
	exit 1
fi

# Now that we know we have the proper number of arguments and we know the file exists, we can move on

# Lets echo the writeStr to the writeFile
# ">" To Overwrite, ">>" To Append
echo "$2" > $1

# Exit with success
exit 0
