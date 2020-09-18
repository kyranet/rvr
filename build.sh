#!/bin/sh

RED='\033[0;31m'
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

build() {
	# Print message:
	printf "${YELLOW}Building $1...\n${NC}"

	# Enter the directory:
	cd $1

	if [ "$?" -ne "0" ]
	then
		# Print an error message.
		printf "${RED}The directory $1 does not exist!\n${NC}"
		return 1
	fi

	# Run the make command:
	make

	# If success:
	if [ "$?" -eq "0" ]
	then
		# Print successful message.
		printf "${GREEN}Successfully built $1!\n${NC}"
	else
		# Else call popd (to exit the previous pushd),
		# then print error message.
		cd ..
		printf "${RED}Failed to build $1!\n${NC}"

		return 1
	fi

	# Go to the previous directory:
	cd ..

	# Return success
	return 0
}

cd practica2.1

for i in 1 2 3 4 5 6 7
do
	build "ejercicio${i}"

	if [ "$?" -ne "0" ]
	then
		return 1
	fi
done

return 0
