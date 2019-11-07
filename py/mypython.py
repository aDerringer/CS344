#!/usr/bin/Python3

#################################################
#Program: 	Program Py
#Author: 	Andrew Derringer
#Last Modified: 	11/6/2019
#Description: 	Random generation of string to outputfiles and 
#		printing the sum of randomly generated numbers.
#################################################

import random

########################
# function: 	Sum Random Integers
# Summary:	Use of random method
#		to generate and sum random 
#		numbers in passed range
# Param:	Int number of random numbers
#		to generate
# Output:	Prints each number
#		Prints sum
########################
def randomSum(maxRange):
   mySum = 0
   for i in range(1, maxRange + 1):
      x = random.randint(1,42)
      print(x)
      mySum += x
   print(mySum)

########################
# Function: 	Generate Letter
# Summary:	Generates single random
#		lowercase alpha char
# Output:	char lowercase alpha
########################
def generateLetter():
   asciiVal = random.randint(97,122)
   return chr(asciiVal)

########################
# Function:	Generate String
# Summary:	Builds a string of 10
#		random lowercase alpha
#		char
# Output:	string of 10 lowercase
#		alpha char
########################
def generateString(stLen):
   string = ''
   for _ in range(stLen):
      string += generateLetter()
   print(string)
   return string

########################
# Function:	Generate Files
# Summary:	Creates or overwrites
#		a file of passed name
#		and write random string
#		to it
# Param:	String for file name
# Output:	file containing random string
########################
def generateFile(fileName):
   file = open(fileName, "w+")
   file.write(generateString(10) + '\n')
   file.close()

########################
# Function: 	Output Files
# Summary:	Creates and files a
#		number of files as
#		passed by argument
# Param:	int number of files
#		to create
# Output:	Requested number of files
########################
def outputFiles(numFiles):
   for i in range(1, numFiles + 1):
      generateFile("file" + str(i))

#Function calls for script output
outputFiles(3)
randomSum(2)
