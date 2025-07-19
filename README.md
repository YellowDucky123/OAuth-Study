# This is a repository containing results from my Study/Research on OAuth2.0
The Study was mostly on Google's OAuth2, However, concepts on flows apply to all OAuth2.0s

## File Explanations
### Script.py
Script.py is a high level version of a google OAuth2 implementation, as the name suggests the implementation is in Python. 
This file also contains an example of getting userinfo.

### Script.c 
Script c is a relatively low level implementation of google's OAuth2, Albeit the code not being as clean as it could be,
it showcases everything from the server backend's socket binding, listening, to the OAuth GET Requests and POST Requests. 

However, this file does not contain an implementation of requesting the user info as it is just a usual Request to an HTTPS server which was implemented on a high level on Script.py

### linkedlist.c and linkedlist.h
These files are simply implementations of a linkedlist data structure.

### makefile
the makefile file in this repository is used to compile the script.c file.


