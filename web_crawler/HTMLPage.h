/**
*******************************************************************************
* @file		HTMLPage.h
* @brief	This file provides the interfaces of the classes HTMLPage.
* @author	Yifeng He
* @date		Feb. 11, 2014, version 1.0
*******************************************************************************
**/

#ifndef _HTMLPAGE_H_
#define _HTMLPAGE_H_

#include <iostream> 
#include <fstream>
#include <string>
#include <list>
#include <sstream>
#include <algorithm>    // std::mismatch
#include <ctime>
#include <set>
#include <vector>


//boost lib
#include <boost/shared_ptr.hpp> 
#include <boost/regex.hpp> 
#include <boost/tuple/tuple.hpp> 
#include <boost/tuple/tuple_io.hpp> 

//curlplusplus lib to download html page
#include <curlplusplus/easy.hpp>

//tidypp lib to download html page 
#include <tidypp/buffer.hpp>
#include <tidypp/document.hpp>
#include <tidypp/attribute.hpp>
#include <tidypp/node.hpp>
#include <curlplusplus/easy.hpp>

using namespace std;

//define the structure to record the useful information extacted on the html page
typedef boost::tuple<long /*1) product id*/, string/*2) product categoty*/, 
	string/*3) product description*/, string/*4) link of the product image*/, 
	float/*5) original price*/, float/*6) current price*/> Information; 

namespace WebDataExtraction 
{


/**
*******************************************************************************
* @class		HTMLPage
* @brief 		This class defines the methods for obtaining the HTML page and
extracting data and links from the page.
*******************************************************************************
*/
class HTMLPage
{
	private:
		//the http URL
		string url_;
		//the host name
		string hostName_;
		//html page
		boost::shared_ptr<string> ptrHtmlPage_; 
		//pointer to the set of links on the page
		boost::shared_ptr< set<string> > ptrLinkSet_;
		//pointer to the vector of product information on the page
		boost::shared_ptr< vector<Information> > ptrProductInfoVector_;

	public:
 		//constructor
 		HTMLPage(string url);
 		//get the http URL
 		string getURL() const;
 		//get host name
 		string getHostName() const;
 		//get the pointer to the HTML page
 		boost::shared_ptr<string> getPtrHtmlPage() const;
 		//get the pointer to the link set
 		boost::shared_ptr< set<string> > getPtrLinkSet() const;
 		//get the pointer to the vector of product information
 		boost::shared_ptr< vector<Information> > getPtrProductInfoVector() const;
 		//set URL
 		void setURL(string url);
 		//initialize: 1) get the host name, 2) download the html page
 		int init();
 		//extract all links on the html page
 		int extractLinks();
 		//extract useful information fron the html page
 		int extractInfo();
 
 		/*friend function: a callback function specified by 
 		curl.setopt(CURLOPT_WRITEFUNCTION, writer) in init(); */
		friend int writer(char* data, size_t size, size_t nmemb, tidypp::buffer *dst);
	
}; //end of class HTMLPage

} //end of namespace WebDataExtraction

#endif //_HTMLPAGE_H_



