# File Forensic Analysis

A C program that analyze the content of a given file/dir

## Instalation and Usage

 1. Download/clone the content of this repo
 2. Run the command 'make' in the directory
 3. If you want to use log functionalities, run 'export LOGFILENAME=$(readlink -f log.txt)'
 4. And then run the program itself!
    * forensic [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>
    * Options:
      - r: recursive execution as the program analyses the content of subfolders
      - h: the program calculates the cryptographic hash of the files, receives as argument the wanted algorithm comma-separated 
      - o: stores data of the analysis in a file passed as argument
      - v: some important actions of the execution are written in a file, the file used is obtained by the program in the environment variable LOGFILENAME. NOTE: this feature was not largely developed and extended to its maximum potential

## Motivation

This project was developed during the classes of Operating Systems, and the goal was to learn to handle and operate with Linux API tools and functionalities