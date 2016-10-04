/*
how to install libxml++?
#sudo apt-get install libxml++2.6-dev libxml++2.6-doc

How to install libtidy? (libtidy cannot parse html5 file, we need to install libtidy html5)
#sudo apt-get install -y php5-dev libtidy-dev

How to install libtidy-html5? 
http://therealmarv.blogspot.ca/2012/12/installing-tidy-html5-on-ubuntu-first.html
*/

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include <tidy.h>
#include <buffio.h>

#include <libxml++/libxml++.h>
#include "testutilities.h"
#include <cstdlib>

using namespace std;
 
void print_node(const xmlpp::Node* node, bool substitute_entities, unsigned int indentation = 0)
{  
  const Glib::ustring indent(indentation, ' ');
  //std::cout << std::endl; //Separate nodes by an empty line.

  if (substitute_entities)
  {
    // Entities have been substituted. Print the text nodes.
    const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(node);
    if (nodeText && !nodeText->is_white_space())
    {
      std::cout /*<< CatchConvertError(nodeText->get_parent()->get_name()) << "\t" */<< CatchConvertError(nodeText->get_content()) << "\t";
    }
  }
  else
  {
    // Entities have not been substituted. Print the entity reference nodes.
    const xmlpp::EntityReference* nodeEntityReference = dynamic_cast<const xmlpp::EntityReference*>(node);
    if (nodeEntityReference)
    {
      std::cout << indent << "entity reference name = " << CatchConvertError(nodeEntityReference->get_name()) << std::endl;
      std::cout << indent << "  resolved text = " << CatchConvertError(nodeEntityReference->get_resolved_text()) << std::endl;
      std::cout << indent << "  original text = " << CatchConvertError(nodeEntityReference->get_original_text()) << std::endl;
    }
  } // end if (substitute_entities)

  const xmlpp::ContentNode* nodeContent = dynamic_cast<const xmlpp::ContentNode*>(node);
  if(!nodeContent)
  {
    //Recurse through child nodes:
    xmlpp::Node::NodeList list = node->get_children();
    for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
    {   
      print_node(*iter, substitute_entities, indentation + 2); //recursive
    }
  }
}

int main()
{
	//create an instance of XML DOM parser
	xmlpp::DomParser doc; 
	
	// 'canadiantire' contains your HTML
	//string str=CleanHTML("canadiantire");
	
	
	std::ifstream t("products.xml");
	std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  //cout << str << endl;
	
	//Parse an XML document from a string
	doc.parse_memory(str);
	
	//Document is a class which represents a XML document in the Document Object Model (DOM)
	xmlpp::Document* document = doc.get_document();
	
	//Element is a class. Element nodes have attributes as well as child nodes. 
	xmlpp::Element* root = document->get_root_node(); //root node

	
	
	xmlpp::NodeSet elemns = root->find("/PRODUCTS/PRODUCT");
	
	
	//cout << "There are " << elemns.size() << " products as follows:" << endl; 
	cout << "ID\t\t" << "CATEGORY\t\t" << "SKU\t\t" << "SUB_CATEGORY\t\t" << "NAME\t\t" << "PRICE" << endl;
	int i;
	for (i = 0; i < elemns.size(); i++) {
		cout << i << "\t";
		const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(elemns[i]);
		//cout << "Category: " ;
		const xmlpp::Attribute* attribute = nodeElement->get_attribute("category");
    if(attribute)
    {
      std::cout << attribute->get_value() << "\t";
      
    }
		print_node(elemns[i], true);
		cout << endl;
	}
	
  if(doc)
  {
    //Walk the tree:
    const xmlpp::Node* pNode = doc.get_document()->get_root_node(); //deleted by DomParser.
    print_node(pNode, false);
  }
      
	return 0;
}



