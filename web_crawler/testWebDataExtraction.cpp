/**
*******************************************************************************
* @file			testWebDataExtraction.cpp
* @brief 		This file provides the main function which extracts the useful 
information from a website. 
* @author		Yifeng He
* @date			Feb. 11, 2014, Version 1.0
*******************************************************************************
**/

#include "HTMLPage.h"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp> 
#include <iostream>

#include <ctime>

using namespace std;
using namespace WebDataExtraction;


/******** global variables ************/
//mutex for exclusive access to resource
boost::mutex mutexLock;
//the set holding the links
set<string> dispatchSet;
//the set to hold the completed links
set<string> completedSet;
//the vector holding the records of product information
vector<Information> productRecordVector;
//the id of the processed html file
int fileID = 0;
//the host name of the searched website
string hostName = "http://www.walmart.ca";



/**
*******************************************************************************
* @brief		This function validates the input URL string.
* @param		string -- url (the http URL of the HTML page)
* @param		string -- hostName (e.g., http://www.walmart.ca/)
* @return		string -- return a complete and valid URL, or "" in case validation 
failure
*******************************************************************************
*/
string validateURL(string url, string hostName)
{
	//the string to be returned
	string validatedURL = "";
	
	if (boost::algorithm::starts_with(url, "/")) {
		validatedURL = hostName + url;
	}
	if (boost::algorithm::starts_with(url, "http") || 
						boost::algorithm::starts_with(url, "www") ) {
		if (boost::algorithm::contains(boost::algorithm::to_lower_copy(url), "walmart.ca") ) {
			validatedURL = url;
		}
		else {
			validatedURL = "";
		}	
	}		
  
  //finally check if the link has been processed
  if (validatedURL != "") {
  	//if the URL has been processed
  	mutexLock.lock();
  	if (completedSet.find(validatedURL) != completedSet.end()) {
  		validatedURL = "";
  	}
  	mutexLock.unlock();
  }
  
  //cout << "validation result: " << validatedURL << endl;
  return validatedURL;
}



/**
*******************************************************************************
* @brief		This function implements the processing thread.
* @param		boost::shared_ptr< boost::asio::io_service >
* @return		void
*******************************************************************************
*/
void processingThread(boost::shared_ptr<boost::asio::io_service> ptrIOService)
{
	mutexLock.lock();
	std::cout << "[" << boost::this_thread::get_id() << "] Thread Start" << std::endl;
	mutexLock.unlock();

	ptrIOService->run();

	mutexLock.lock();
	std::cout << "[" << boost::this_thread::get_id() << "] Thread Finish" << std::endl;
	mutexLock.unlock();
}



/**
*******************************************************************************
* @brief		This function defines the HTML parsing task.
* @param		string -- url (the http URL of the HTML page)
* @return		void
*******************************************************************************
*/
void parsingHTML(string url)
{
	//validate the input url
	string validatedURL = "";
	validatedURL = validateURL(url, hostName);
	//exit if the input URL is invalid
	if (validatedURL == "")
		return;
		
	cout << "[" << boost::this_thread::get_id() << "] is processing " << 
		validatedURL << endl;
		
	//instantiate a HTMLPage object
	HTMLPage htmlPage(validatedURL);
	//init the HTMLPage object
	htmlPage.init();
	
	//save the HTML page into a file
  ofstream htmlFileStream;
	stringstream strStream;
	mutexLock.lock();
	fileID++;
	mutexLock.unlock();
	strStream << "./data/page" << fileID << ".html";
	string fileName=strStream.str();
  htmlFileStream.open(fileName.c_str());
  htmlFileStream << *htmlPage.getPtrHtmlPage();
  htmlFileStream.close();
	
	//extract the links on the page
	int numLinksExtracted;
	numLinksExtracted = htmlPage.extractLinks();

	boost::shared_ptr< set<string> > ptrLinkSet = htmlPage.getPtrLinkSet();
	//insert the links into dispatchSet
	int index = 0;	
	for (set<string>::iterator it = ptrLinkSet->begin(); it != ptrLinkSet->end(); it++) {
		//cout << "validating " << *it << endl;
		string validURL = validateURL(*it, hostName);
		if (validURL != "") {
			index++;
			mutexLock.lock();
			dispatchSet.insert(validURL);
			cout << "The size of the dispatchSet is " << dispatchSet.size() << endl;
			mutexLock.unlock();
		}
	}	
	
	//insert the processed link into the completedSet
	mutexLock.lock();
	completedSet.insert(htmlPage.getURL());
	cout << "The size of completed set is " << completedSet.size() << endl;
	mutexLock.unlock();
	
	//next extract the product information

}



/**
*******************************************************************************
* @brief		This function is the entrance to the program.
* @param		string -- url (the http URL of the HTML page)
* @return		int -- return 0 if successful, or 0 if unsuccessful
*******************************************************************************
*/
int main( )
{
	//start timestamp
	clock_t begin=clock();
	
	//the input URL: the web site that we want to extract the data
	string webSiteURL = "http://www.walmart.ca/en";
	
	//validate the input url
	string validatedURL = validateURL(webSiteURL, hostName);
	//cout << validatedURL << endl;
	
	//pase the input URL
	if (validatedURL != "") {
		parsingHTML(validatedURL);
	}
	
	//variable to control the termination of processing threads
	int isFinished = 0;

	/****Set up multti-thread ***************/
	//instantiate an io_service object
	boost::shared_ptr<boost::asio::io_service> ptrIOService(new boost::asio::io_service);		
	//define the processing thread associated with the io_service object
	boost::shared_ptr<boost::asio::io_service::work> ptrProcessingThread (
		new boost::asio::io_service::work(*ptrIOService) );
	//creat the processing thread pool
	int numberThreads = 4;
	boost::thread_group processingThreadGroup;
	for (int i = 0; i < numberThreads; i++) {
		processingThreadGroup.create_thread( 
			boost::bind(processingThread/*function to create a thread*/, ptrIOService/*argument*/));
	}

	/******* dispatch the tasks **************/
	string selectedURL = "";
	while (isFinished == 0) {
		//pick a task from dispatchSet
		mutexLock.lock();
		if (dispatchSet.size() > 0 ) {
			selectedURL = *dispatchSet.begin();
			dispatchSet.erase(dispatchSet.begin());
		}
	  mutexLock.unlock();	
	  
	  //post the task
	  if (selectedURL != "") {
	  	//cout << "Posting " << selectedURL << endl;
	  	ptrIOService->post(boost::bind(parsingHTML, selectedURL));
	  }
	  
	  //check if the dispatchSet is empty
	  mutexLock.lock();
	  if (dispatchSet.empty() ) {
	  	isFinished = 1;
	  }
	  mutexLock.unlock();	  
	} //end of while-loop

	//signal the processing threads to exit once they finish the ongoing tasks.
	ptrProcessingThread.reset();
	//all threads in the thread_group join
	processingThreadGroup.join_all();
	
	//print out all the processed links
	int indexProcessedLink = 0;
	cout << "The following HTTP links have been processed: " << endl;
	for (set<string>::iterator it = completedSet.begin(); it != completedSet.end(); it++) {
		indexProcessedLink++;
		cout << "Link " << indexProcessedLink << ": " << *it << endl;
	}

	//record the total executation time
  clock_t end = clock();
  double elapsedTime = 1000*(double(end-begin) / CLOCKS_PER_SEC);
  cout << "Done." << "This program totally consumed " << elapsedTime << " ms." << endl;

	return 0;
}



