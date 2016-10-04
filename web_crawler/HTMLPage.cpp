/**
*******************************************************************************
* @file			HTMLPage.cpp
* @brief 		This file provides the implementations of the class HTMLPage. 
* @author		Yifeng He
* @date			Feb. 11, 2014, Version 1.0
*******************************************************************************
**/

#include "HTMLPage.h"

//namespaces used in this file
using namespace std;
using namespace WebDataExtraction;
using namespace curlpp;



/**
*******************************************************************************
* @brief		This function is a callback function used by curl.perform().
* @param		char* -- data (the data that has just been obtained)
* @param		size_t -- size (the size of each data block)
* @param		size_t -- nmemb (the number of data blocks. multiply this by size to 
obtain the total size of data)
* @param		tidypp::buffer* -- dst (This is output, the destination buffer that 
was assigned to CURLOPT_WRITEDATA)
* @return		int -- the amount of bytes that were dispatched to the destination buffer
*******************************************************************************
*/
int writer(char *data, size_t size, size_t nmemb, tidypp::buffer *dst)
{
    size_t len;

    if (!data)
        return 0;

    len = size * nmemb;
    dst->append(data, len); // append recieved data to the buffer

    return len; // must return the amount of written bytes
}

/**
*******************************************************************************
* @brief		This function is the constructor of the class HTMLPage.
* @param		string -- url (the URL of the html page)
* @return		None
*******************************************************************************
*/
HTMLPage::HTMLPage(string url) : url_(url)
{
	ptrHtmlPage_ = boost::shared_ptr<string> (new string);
	ptrLinkSet_ = boost::shared_ptr< set<string> > (new set<string>);
	ptrProductInfoVector_ = boost::shared_ptr< vector<Information> > (new vector<Information>);
}



/**
*******************************************************************************
* @brief		This function returns the URL.
* @param		none
* @return		string -- the URL of the html page
*******************************************************************************
*/
string HTMLPage::getURL() const
{
	return url_;
}



/**
*******************************************************************************
* @brief		This function returns the host name.
* @param		none
* @return		string -- the host name of the html page
*******************************************************************************
*/
string HTMLPage::getHostName() const
{
	return hostName_;
}



/**
*******************************************************************************
* @brief		This function returns the pointer to the HTML page.
* @param		none
* @return		boost::shared_ptr<string>
*******************************************************************************
*/
boost::shared_ptr<string> HTMLPage::getPtrHtmlPage() const
{
	return ptrHtmlPage_;
}


/**
*******************************************************************************
* @brief		This function returns the pointer to the link set.
* @param		none
* @return		boost::shared_ptr< set<string> >
*******************************************************************************
*/
boost::shared_ptr< set<string> > HTMLPage::getPtrLinkSet() const
{
	return ptrLinkSet_;
}



/**
*******************************************************************************
* @brief		This function returns the pointer to the vector of product information.
* @param		none
* @return		boost::shared_ptr< vector<Information> >
*******************************************************************************
*/
boost::shared_ptr< vector<Information> > HTMLPage::getPtrProductInfoVector() const
{
	return ptrProductInfoVector_;
}

/**
*******************************************************************************
* @brief		This function sets the URL.
* @param		string -- url (the URL of the html page)
* @return		Void
*******************************************************************************
*/
void HTMLPage::setURL(string url)
{
	url_=url;
}



/**
*******************************************************************************
* @brief		This function gets the host name and downloads the html page.
* @param		none
* @return		int -- return 0 if the init succeeds, and 1 if the init fails.
*******************************************************************************
*/
int HTMLPage::init()
{
	//step 1): get the host name
	/*get the host name from the URL, for example:
	URL: http://www.walmart.ca/en/health-beauty/pharmacy/foot-care/N-1441
	host name: http://www.walmart.ca */
	boost::regex hostNamePattern("(https?:\\/\\/)?([\\da-z\\.-]+)\\.([a-z\\.]{2,6})");
	string::const_iterator start, end;
	start = url_.begin();
	end = url_.end();
	boost::match_results<std::string::const_iterator> what;
	boost::match_flag_type flags = boost::match_default;
	string matchedStr="";
	long index=0;
	while (boost::regex_search(start, end, what, hostNamePattern, flags))
	{
		index++;
		matchedStr=what.str();//the whole matched string
		//cout << matchedStr << endl;
		start = what[0].second;
	}
	if (matchedStr == "") {
		cout << "Failed to extract the host name." << endl;
	}	
	else {
		hostName_ = matchedStr;
	}
	
	//step 2): download the HTML page via cURL lib
	//store our html code
	tidypp::buffer html; 
	//tore the warnings and errors encountered by html tidy
 	tidypp::buffer errbuf;

	try
	{
		//handle to the curl easy interface
	  curlpp::easy curl; 
	  //store the result of the curl request
	  long http_status; 

	  //std::cout << "Obtaining page..." << std::endl;
	  //assign URL
	  curl.setopt(CURLOPT_URL, url_.c_str()); 
	  //set options
	  curl.setopt(CURLOPT_FOLLOWLOCATION, 1);
	  //assign write callback
	  curl.setopt(CURLOPT_WRITEFUNCTION, writer); 

	  //assign write buffer (our tidypp buffer)
	  curl.setopt(CURLOPT_WRITEDATA, &html); 
	  //perform curl to download the html page
	  curl.perform(); 
	  //get result http status returned by the HTTP server
	  curl.getinfo(CURLINFO_RESPONSE_CODE, &http_status); 

		//code 200 means sucessful download, other code means failure
	  if (http_status != 200) 
	  {
			std::cerr << "Expecting HTTP 200 OK, got " << http_status << std::endl;
      return 1;
	  }
	}
	//catch exceptions and print the error on screen
	catch (const curlpp::exception &e) 
	{
  	std::cerr << "Exception in obtaining the page: " << e.what() << std::endl;
	  return 1;
	}
	//store the the obtained html page
	std::ostringstream ss;
	ss << html.ptr();
	*ptrHtmlPage_ = ss.str();
	
	return 0;
}



/**
*******************************************************************************
* @brief		This function extracts all links on the HTML page.
* @param		none
* @return		int -- return the number of extracted links.
*******************************************************************************
*/
int HTMLPage::extractLinks()
{
	//number of links
	int numberLinks = 0;
	//regular expression pattern for the links on the HTML page
	boost::regex linkPattern("[hH][rR][eE][fF]\\s*=\\s*\"([^\"]+)\"");
	
	string::const_iterator start, end;
	start = (*ptrHtmlPage_).begin();
	end = (*ptrHtmlPage_).end();
	boost::match_results<std::string::const_iterator> what;
	boost::match_flag_type flags = boost::match_default;
	string matchedStr;
	string matchedstrgroup1;
	while (boost::regex_search(start, end, what, linkPattern, flags)) {
		numberLinks++;
		//the matched string
		matchedStr=what.str();
		//the matched group-1 substring
		matchedstrgroup1=what[1].str(); 
		//store the links into the link set
		ptrLinkSet_->insert(matchedstrgroup1);
		//cout << "Link " << index << " : " << matchedstrgroup1 << endl;
		start = what[0].second;
	}	
	
	return numberLinks;
}



/**
*******************************************************************************
* @brief		This function extracts the useful information on the HTML page.
* @param		none
* @return		int -- return the number of records.
*******************************************************************************
*/
int HTMLPage::extractInfo()
{
	//to be filled by Xiaoming and Ning
	//a simple example
	//number of records
	int numberRecords = 0;	
	
	//typedef boost::tuple<long /*1) product id*/, string/*2) product categoty*/, 
	//string/*3) product description*/, string/*4) link of the product image*/, 
	//float/*5) original price*/, float/*6) current price*/> Information;
	Information infoRecord1(
		101, 
		"Electronics", 
		"PROSCAN 7\" Android 4.1 Tablet with Case & Keyboard (PLT7-K)",
		"http://i-store.walmart.ca/images/WMTCNPE/NewImages/be51e2bbbd63432db8b7c26581ca8649/Large_.jpeg",
		78.00f,
		64.00f );
	
	numberRecords++;
	//push it into the vector
	ptrProductInfoVector_->push_back(infoRecord1);	
	
	return numberRecords;
}



